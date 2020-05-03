#include "Management_Unit.h"
#include <sstream>
#include <iomanip>

namespace cgra {

/*!
 * \brief Build binary string from integer number
 *
 * \param[in] numberA Integer number to be transformed
 * \tparam bitwidth Length of binary result string
 *
 * \return String with binary representation of numberA
 */
template < uint32_t bitwidth = 32 >
static std::string integer2binaryCstr(const uint32_t numberA)
{
	std::ostringstream t_value{};
	
	for(uint32_t idx = 0; bitwidth > idx; ++idx)
	{
		if((numberA>>idx)&1)
			t_value << '1';
		else
			t_value << '0';
	}
	
	return t_value.str();
}



ManagementUnit::ManagementUnit(const sc_core::sc_module_name& nameA,
		CommandInterpreter::assembler_type_t* const program_assemblyA,
		const uint64_t sizeA)
	: sc_core::sc_module(nameA), m_programPointer(&m_programMemory.front()),
	   m_activeState(ACTIVE_STATE::STOP), m_current_state(STATE::NOOP)
{
	//Register state machine process to simulator
	SC_METHOD(state_machine);
	sensitive << clk.pos();
	SC_METHOD(wait_ready);
	sensitive << ready.pos();
	dont_initialize();
	SC_METHOD(pause_interrupt);
	sensitive << pause.value_changed();
	dont_initialize();
	SC_METHOD(reset_interrupt);
	sensitive << rst.neg();
	dont_initialize();
	SC_METHOD(run_interrupt);
	sensitive << run.pos();
	dont_initialize();
	SC_METHOD(wait_mmu_ready);
	sensitive << mmu_ready.neg();
	dont_initialize();

	//Wire signal connections
	m_cInterpreter.assembler.bind(m_currentAssembler);
	m_cInterpreter.command.bind(m_currentCommand);
	m_cInterpreter.address.bind(m_currentAddress);
	m_cInterpreter.line.bind(m_currentLine);
	m_cInterpreter.place.bind(m_currentPlace);

	//Store Assembler commands form source in program memory
	uint64_t t_size;
	//auto t_ptr = const_cast<CommandInterpreter::assembler_type_t*>(program_assemblyA);

	//Limit program size to program memory.
	if(sizeA > cgra::cProgramMemorySize)
	{
		t_size = cgra::cProgramMemorySize;
		SC_REPORT_WARNING("ManagementUnit", "Program size exceeds program memory.");
	}
	else
		t_size = sizeA;

	for(uint64_t i = 0; t_size > i; ++i)
		m_programMemory.at(i) = *(program_assemblyA + i);
	//memcpy(m_programMemory.data(), t_ptr, t_size * sizeof(CommandInterpreter::assembler_type_t));

	return;
}

void ManagementUnit::state_machine()
{
	if(ACTIVE_STATE::RUN == m_activeState)
	{
		switch (m_current_state)
		{
			case STATE::NOOP:
				m_current_state = STATE::ADAPT_PP;
				break;
			case STATE::FETCH:
				fetch();
				break;
			case STATE::DECODE:
				decode();
				break;
			case STATE::ADAPT_PP:
				adapt_prog_pointer();
				break;
			case STATE::START:
				start_state();
				break;
			case STATE::WAIT_READY:
			{
				/*
				 * If assembler commands are performed in parallel to
				 * a processing VCGRA, an interrupt is stored. If the
				 * assembler program reaches a WAIT READY command for the
				 * next time, it is checked if an interrupt already occurred.
				 * Then, the ManagementUnit will not go into active state WAIT,
				 * it will start with the next assembler program step.
				 *
				 * Currently its not allowed to start a VCGRA again if you not clear the
				 * interrupt before.
				 */
				bool unusedReturn;
				if(m_readyInterrupt.nb_read(unusedReturn))
				{
					m_current_state = STATE::ADAPT_PP;
				}
				else
					m_activeState = ACTIVE_STATE::WAIT;
				break;
			}
			case STATE::WAIT_MMU:
				m_activeState = ACTIVE_STATE::WAIT;
				break;
			case STATE::CONT_MMU:
				start_mmu();
				m_current_state = STATE::ADAPT_PP;
				break;
			case STATE::LOADD:
			case STATE::LOADDA:
			case STATE::LOADPC:
			case STATE::LOADCC:
			case STATE::STORED:
			case STATE::STOREDA:
				start_mmu();
				break;
			case STATE::FINISH:
			{
				show_finish_state();
				break;
			}
			default:
				m_current_state = STATE::NOOP;
				SC_REPORT_WARNING("MU State","Unknown State. No operation performed.");
				break;
		}
	}
	else if(ACTIVE_STATE::ERROR == m_activeState)
	{
		this->dump();
		SC_REPORT_FATAL("MU", "Runtime error");
	}

	return;
}

void ManagementUnit::fetch()
{
	//Load current assembler command to decode module
	m_currentAssembler.write(*m_programPointer);
	m_current_state = STATE::DECODE;

	return;
}

void ManagementUnit::decode()
{
	//Decode next command and update state machine state
	m_current_state = static_cast<enum STATE>(m_currentCommand.read().to_uint());

	//Update to address lines according to next command
	switch(m_current_state)
	{
	case STATE::LOADD:
		cache_select.write(MMU::CACHE_TYPE::DATA_INPUT);
		dic_select_lines.first.write(m_cInterpreter.line.read().to_uint());
		place.write(m_currentPlace);
		break;
	case STATE::LOADDA:
		cache_select.write(MMU::CACHE_TYPE::DATA_INPUT);
		dic_select_lines.first.write(m_cInterpreter.line.read().to_uint());
		place.write(127);
		break;
	case STATE::SLCT_DIC_LINE:
		dic_select_lines.second.write(m_cInterpreter.line.read().to_uint());
		m_current_state = STATE::ADAPT_PP;
		break;
	case STATE::STORED:
		cache_select.write(MMU::CACHE_TYPE::DATA_OUTPUT);
		doc_select_lines.second.write(m_cInterpreter.line.read().to_uint());
		place.write(m_currentPlace);
		break;
	case STATE::STOREDA:
		cache_select.write(MMU::CACHE_TYPE::DATA_OUTPUT);
		doc_select_lines.second.write(m_cInterpreter.line.read().to_uint());
		place.write(127);
		break;
	case STATE::SLCT_DOC_LINE:
		doc_select_lines.first.write(m_cInterpreter.line.read().to_uint());
		m_current_state = STATE::ADAPT_PP;
		break;
	case STATE::LOADPC:
		cache_select.write(MMU::CACHE_TYPE::CONF_PE);
		pe_cc_select_lines.first.write(m_cInterpreter.line.read().to_uint());
		place.write(m_currentPlace);
		break;
	case STATE::SLCT_PECC_LINE:
		pe_cc_select_lines.second.write(m_cInterpreter.line.read().to_uint());
		m_current_state = STATE::ADAPT_PP;
		break;
	case STATE::LOADCC:
		cache_select.write(MMU::CACHE_TYPE::CONF_CC);
		ch_cc_select_lines.first.write(m_cInterpreter.line.read().to_uint());
		place.write(m_currentPlace);
		break;
	case STATE::SLCT_CHCC_LINE:
		ch_cc_select_lines.second.write(m_cInterpreter.line.read().to_uint());
		m_current_state = STATE::ADAPT_PP;
		break;
	default:
		cache_select.write(MMU::CACHE_TYPE::NONE);
		place.write(m_currentPlace);
		break;
	}
	address.write(m_currentAddress);

	return;
}

void ManagementUnit::adapt_prog_pointer()
{
	++m_programPointer;
	m_current_state = STATE::FETCH;

	return;
}

void ManagementUnit::start_state()
{
	//Check if ready signal is proven before
	//starting next VCGRA processing round.
	bool unusedReturn;
	if(m_readyInterrupt.nb_read(unusedReturn))
		m_activeState = ACTIVE_STATE::ERROR;

	//Start VCGRA
	//Signal High for one clock cycle
	else if(start.read())
	{
		start.write(false);
		m_current_state = STATE::ADAPT_PP;
	}
	/*There is no signal which shows that the VCGRA is currently in process.
	* The assembler needs to ensure that the VCGRA is started again only if the ready signal already occurred.
	*/
	else
		start.write(true);

	return;
}

void ManagementUnit::wait_ready()
{
	//If former processing state is WAIT
	//resume execution if positive edge at
	//ready port occurs.
	// "!mmu_start" is used to control, that the ManagementUnit is active waiting
	// for the trigger from the VCGRA. WAIT state is also used, if MMU is triggert.
	if(ACTIVE_STATE::WAIT == m_activeState && !mmu_start.read())
	{
		m_activeState = ACTIVE_STATE::RUN;
	}

	m_readyInterrupt.nb_write(true);

	return;
}

void ManagementUnit::end_of_elaboration()
{
	start.write(false);
	mmu_start.write(false);
	address.write(0);
	place.write(0);
	finish.write(false);
	cache_select.write(MMU::CACHE_TYPE::NONE);
	m_currentCommand.write(0);

	dic_select_lines.first.write(0);
	dic_select_lines.second.write(1);
	doc_select_lines.first.write(0);
	doc_select_lines.second.write(1);
	pe_cc_select_lines.first.write(0);
	pe_cc_select_lines.second.write(1);
	ch_cc_select_lines.first.write(0);
	ch_cc_select_lines.second.write(1);
}

void ManagementUnit::dump(std::ostream& os) const
{
	os << name() << ": " << kind() << std::endl;
	os << "Current start status:\t\t" << std::setw(3) << std::boolalpha << start.read() << std::endl;
	os << "Current ready status:\t\t" << std::setw(3) << std::boolalpha << ready.read() << std::endl;
	os << "Current run status:\t\t" << std::setw(3) << std::boolalpha << run.read() << std::endl;
	os << "Current pause status:\t\t" << std::setw(3) << std::boolalpha << pause.read() << std::endl;
	os << "Current reset status:\t\t" << std::setw(3) << std::boolalpha << rst.read() << std::endl;
	os << "Current MMU start status:\t" << std::setw(3) << std::boolalpha << mmu_start.read() << std::endl;
	os << "Current MMU ready status:\t" << std::setw(3) << std::boolalpha << mmu_ready.read() << std::endl;
	os << "Current RUN STATE:\t\t";
	switch(static_cast<enum STATE>(m_currentCommand.read().to_uint()))
	{
		case STATE::NOOP:
			os << "NOOP" << std::endl;
			break;
		case STATE::FETCH:
			os << "FETCH" << std::endl;
			break;
		case STATE::DECODE:
			os << "DECODE" << std::endl;
			break;
		case STATE::ADAPT_PP:
			os << "ADAPT_PROGRAM_POINTER" << std::endl;
			break;
		case STATE::LOADD:
			os << "Load Data Value" << std::endl;
			break;
		case STATE::LOADDA:
			os << "Load data value for a cache line" << std::endl;
			break;
		case STATE::LOADCC:
			os << "Load virtual channel configuration cache" << std::endl;
			break;
		case STATE::LOADPC:
			os << "Load processing element configuration cache" << std::endl;
			break;
		case STATE::START:
			os << "Start VCGRA processing" << std::endl;
			break;
		case STATE::STORED:
			os << "Read data value" << std::endl;
			break;
		case STATE::STOREDA:
			os << "Read data values of cache line" << std::endl;
			break;
		case STATE::WAIT_MMU:
			os << "Wait for MMU to be ready" << std::endl;
			break;
		case STATE::CONT_MMU:
			os << "Continue execution after wait for MMU." << std::endl;
			break;
		case STATE::WAIT_READY:
			os << "Wait for VCGRA to be ready" << std::endl;
			break;
		case STATE::SLCT_DIC_LINE:
			os << "Select active data input cache outline." << std::endl;
			break;
		case STATE::SLCT_DOC_LINE:
			os << "Select active data output cache inline." << std::endl;
			break;
		case STATE::SLCT_PECC_LINE:
			os << "Select active PE configuration cache outline." << std::endl;
			break;
		case STATE::SLCT_CHCC_LINE:
			os << "Select active virtual channel configuration cache outline." << std::endl;
			break;
		default:
			os << "Unknown cache type." << std::endl;
			break;
	}
	os << "Current Address:\t\t" << std::setw(3) << address.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Current place:\t\t\t" << std::setw(3) << place.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Current Cache:\t\t\t";
	switch(static_cast<MMU::CACHE_TYPE>(cache_select.read().to_uint()))
	{
	case MMU::CACHE_TYPE::DATA_INPUT:
		os << "DATA_INPUT";
		break;
	case MMU::CACHE_TYPE::DATA_OUTPUT:
		os << "DATA_OUTPUT";
		break;
	case MMU::CACHE_TYPE::CONF_PE:
		os << "CONF_PE";
		break;
	case MMU::CACHE_TYPE::CONF_CC:
		os << "DATA_CC";
		break;
	case MMU::CACHE_TYPE::NONE:
		os << "NONE";
		break;
	}
	os << std::endl;

	os << "Current line selections for caches:\n";
	os << "===================================" << std::endl;
	os << "Data Input Cache: Select in: " << dic_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << dic_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
	os << "Data Output Cache: Select in: " << doc_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << doc_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
	os << "PE Configuration Cache: Select in: " << pe_cc_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << pe_cc_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
	os << "vCh Configuration Cache: Select in: " << ch_cc_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << ch_cc_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;


	os << "Current ACTIVE STATE:\t\t";
	switch(m_activeState)
	{
	case ACTIVE_STATE::RUN:
		os << "RUN" << std::endl;
		break;
	case ACTIVE_STATE::PAUSE:
		os << "PAUSE" << std::endl;
		break;
	case ACTIVE_STATE::STOP:
		os << "STOP" << std::endl;
		break;
	case ACTIVE_STATE::WAIT:
		os << "WAIT" << std::endl;
		break;
	case ACTIVE_STATE::ERROR:
		os << "ERROR" << std::endl;
		break;
	default:
		os << "Unknown ACTIVE STATE" << std::endl;
	}

	return;

}

void ManagementUnit::start_mmu()
{
	//Set start signal for MMU High for one clock periode.
	if(!mmu_start.read())
	{
		mmu_start.write(true);
		m_current_state = STATE::WAIT_MMU;
	}
	else
		mmu_start.write(false);

	return;
}

void ManagementUnit::pause_interrupt()
{

	if(ACTIVE_STATE::RUN == m_activeState)
	{
		//Positive edge: Pause processing
		if(pause.read())
			m_activeState = ACTIVE_STATE::PAUSE;
	}
	else if(ACTIVE_STATE::PAUSE == m_activeState)
	{
		//Negative edge: Resume processing
		if(!pause.read())
			m_activeState = ACTIVE_STATE::RUN;
	}
	else
		SC_REPORT_WARNING("MU", "PAUSE function only available in previous state RUN");

	return;
}

void ManagementUnit::run_interrupt()
{
	//Enable processing of state machine
	m_activeState = ACTIVE_STATE::RUN;
	m_current_state = STATE::FETCH;

	return;
}

void ManagementUnit::reset_interrupt()
{
	//Stop execution of ManagementUnit
	m_activeState = ACTIVE_STATE::STOP;

	//Reset internal registers and program pointer
	m_current_state = STATE::NOOP;
	m_programPointer = &m_programMemory.front();

	return;
}

void ManagementUnit::wait_mmu_ready()
{
	//WAIT for MMU to finish processing
	if(ACTIVE_STATE::WAIT == m_activeState && mmu_start.read())
	{
		m_current_state = STATE::CONT_MMU;
		m_activeState = ACTIVE_STATE::RUN;
	}
	else
		m_activeState = ACTIVE_STATE::ERROR;

	return;
}

void ManagementUnit::show_finish_state()
{
	if(finish.read())
	{
		m_activeState = ACTIVE_STATE::STOP;
		m_current_state = STATE::NOOP;
		m_programPointer = m_programMemory.data();
		finish.write(false);
	}
	else
		finish.write(true);

	return;
}

} // end namespace cgra
