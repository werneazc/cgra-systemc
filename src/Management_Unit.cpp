#include "Management_Unit.h"
#include <bitset>

namespace cgra {

/*!
 * \brief Build binary C-string from integer number
 *
 * \param [in] numberA Integer number to be transformed
 * \tparam bitlengthA Length of binary result string
 *
 * \return C-string with binary representation of numberA
 */
template < uint32_t bitwidth = 32 >
static constexpr const char* integer2binaryCstr(const uint32_t numberA)
{
	return std::bitset<bitwidth>(numberA).to_string().c_str();
}



ManagementUnit::ManagementUnit(const sc_core::sc_module_name& nameA)
	: sc_core::sc_module(nameA), m_programPointer(&m_programMemory.front()),
	   m_activeState(ACTIVE_STATE::STOP), m_current_state(STATE::NOOP)
{
	//Register state machine process to simulator
	SC_METHOD(state_machine);
	sensitive << clk.pos();
	SC_METHOD(wait_ready);
	sensitive << ready.pos();
	SC_METHOD(pause_interrupt);
	sensitive << pause.value_changed();
	SC_METHOD(reset_interrupt);
	sensitive << rst.neg();
	SC_METHOD(run_interrupt);
	sensitive << run.pos();
	SC_METHOD(wait_mmu_ready);
	sensitive << mmu_ready.neg();

	//Wire signal connections
	m_cInterpreter.assembler.bind(m_currentAssembler);
	m_cInterpreter.command.bind(m_currentCommand);
	m_cInterpreter.address.bind(m_currentAddress);
	m_cInterpreter.line.bind(m_currentLine);
	m_cInterpreter.place.bind(m_currentPlace);
}

void ManagementUnit::state_machine()
{
	if(ACTIVE_STATE::RUN == m_activeState)
	{
		switch (m_current_state)
		{
			case STATE::NOOP:
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
				/*
				 * If assembler commands are performed in parallel to
				 * a processing VCGRA, an interrupt is stored. If the
				 * assembler program reaches a WAIT READY command for the
				 * next time, it is checked if an interrupt already occurs.
				 * Then, the ManagementUnit will not go into active state WAIT,
				 * it will start with the next assembler program step.
				 *
				 * Currently its not allowed to start a VCGRA again if you not clear the
				 * interrupt before.
				 */
				if(m_readyInterrupt.read())
				{
					m_readyInterrupt.write(false);
					m_current_state = STATE::ADAPT_PP;
					break;
				}
				else
				{
				m_activeState = ACTIVE_STATE::WAIT;
				break;
				}
			case STATE::WAIT_MMU:
				m_activeState = ACTIVE_STATE::WAIT;
				break;
			case STATE::LOADD:
				start_mmu();
				break;
			case STATE::LOADDA:
				start_mmu();
				break;
			case STATE::LOADPC:
				start_mmu();
				break;
			case STATE::LOADCC:
				start_mmu();
				break;
			case STATE::STORED:
				start_mmu();
				break;
			case STATE::STOREDA:
				start_mmu();
				break;
			default:
				m_current_state = STATE::NOOP;
				SC_REPORT_WARNING("MU State","Unknown State. No operation performed.");
				break;
		}
	}
	else if(ACTIVE_STATE::ERROR == m_activeState)
	{
		//#TODO: Create State machine diagram and error dump
		SC_REPORT_FATAL("MU", "Runtime error");
	}
}

void ManagementUnit::fetch()
{
	//Load current assembler command to decode module
	m_currentAssembler.write(*m_programPointer);
	m_current_state = STATE::DECODE;
}

void ManagementUnit::decode()
{
	//Decode next command and update state machine state
	m_current_state = static_cast<enum STATE>(m_currentCommand.read().to_uint());

	//Update to address lines according to next command
	switch(m_current_state)
	{
	case STATE::LOADD:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::DATA_INPUT));
		place.write(m_currentPlace);
		break;
	case STATE::LOADDA:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::DATA_INPUT));
		place.write(integer2binaryCstr<7>(127));
		break;
	case STATE::STORED:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::DATA_OUTPUT));
		place.write(m_currentPlace);
		break;
	case STATE::STOREDA:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::DATA_OUTPUT));
		place.write(integer2binaryCstr<7>(127));
		break;
	case STATE::LOADPC:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::CONF_PE));
		place.write(m_currentPlace);
		break;
	case STATE::LOADCC:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::CONF_CC));
		place.write(m_currentPlace);
		break;
	default:
		cache_select.write(integer2binaryCstr<3>(MMU::CACHE_TYPE::NONE));
		place.write(m_currentPlace);
		break;
	}
	address.write(m_currentAddress);
	line.write(m_currentLine);
}

void ManagementUnit::adapt_prog_pointer()
{
	++m_programPointer;
	m_current_state = STATE::FETCH;
}

void ManagementUnit::start_state()
{
	//Check if ready signal is proven before
	//starting next VCGRA processing round.
	if(m_readyInterrupt.read())
		m_activeState = ACTIVE_STATE::ERROR;

	//Start VCGRA
	//Signal High for one clock cycle
	else if(start.read())
	{
		start.write(false);
		m_current_state = STATE::ADAPT_PP;
	}
	//TODO: There is no signal which shows that the VCGRA is currently in process. The assembler needs to ensure
	else
		start.write(true);
}

void ManagementUnit::wait_ready()
{
	//If former processing state is WAIT
	//resume execution if positive edge at
	//ready port occurs.
	if(ACTIVE_STATE::WAIT == m_activeState)
	{
		m_activeState = ACTIVE_STATE::RUN;
		m_current_state = STATE::ADAPT_PP;
	}
	else
	{
		if(!m_readyInterrupt.read())
		m_readyInterrupt.write(true);
	}
}

void ManagementUnit::start_mmu()
{
	//Set start signal for MMU High for one clock periode.
	if(!mmu_start.read())
		mmu_start.write(true);
	else
	{
		mmu_start.write(false);
		m_current_state = WAIT_MMU;
	}
}

void ManagementUnit::pause_interrupt()
{
	//Positive edge: Pause processing
	//Negative edge: Resume processing
	if(pause.read())
		m_activeState = ACTIVE_STATE::PAUSE;
	else
		m_activeState = ACTIVE_STATE::RUN;
}

void ManagementUnit::run_interrupt()
{
	//Enable processing of state machine
	m_activeState = ACTIVE_STATE::RUN;
	m_current_state = STATE::FETCH;
}

void ManagementUnit::reset_interrupt()
{
	//Stop execution of ManagementUnit
	m_activeState = ACTIVE_STATE::STOP;

	//Reset internal registers and program pointer
	m_current_state = STATE::NOOP;
	m_programPointer = &m_programMemory.front();
	m_currentAssembler.write(0);

}

void ManagementUnit::wait_mmu_ready()
{
	//WAIT for MMU to finish processing
	if(ACTIVE_STATE::WAIT == m_activeState)
	{
		m_current_state = STATE::ADAPT_PP;
		m_activeState = ACTIVE_STATE::RUN;
	}
	else
		m_activeState = ACTIVE_STATE::ERROR;
}


} // end namespace cgra
