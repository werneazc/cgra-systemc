#include "Management_Unit.h"

namespace cgra {

ManagementUnit::ManagementUnit(const sc_core::sc_module_name& nameA)
	: sc_core::sc_module(nameA), m_programPointer(&m_programMemory[0]), m_current_state(STATE::NOOP)
{
	//Register state machine process to simulator
	SC_METHOD(state_machine);
	sensitive << clk.pos();

	//Wire signal connections
	m_cInterpreter.assembler.bind(m_currentAssembler);
	m_cInterpreter.command.bind(m_currentCommand);
	m_cInterpreter.address.bind(m_currentAddress);
	m_cInterpreter.line.bind(m_currentLine);
	m_cInterpreter.place.bind(m_currentPlace);

}

void ManagementUnit::state_machine()
{
	switch (m_current_state)
	{
		case STATE::NOOP:
			break;
		case STATE::FETCH:
			break;
		case STATE::DECODE:
			break;
		case STATE::ADAPT_PP:
			break;
		case STATE::START:
			break;
		case STATE::WAIT_READY:
			break;
		case STATE::LOADD:
			break;
		case STATE::LOADDA:
			break;
		case STATE::LOADPC:
			break;
		case STATE::LOADCC:
			break;
		case STATE::STORED:
			break;
		case STATE::STOREDA:
			break;
		default:
			break;
	}
}

void ManagementUnit::fetch() {
}

void ManagementUnit::decode() {
}

void ManagementUnit::adapt_prog_pointer() {
}

void ManagementUnit::start_state() {
}

void ManagementUnit::wait_ready() {
}

void ManagementUnit::load_data() {
}

void ManagementUnit::load_data_array() {
}

void ManagementUnit::store_data() {
}

void ManagementUnit::store_data_array() {
}

void ManagementUnit::load_pe_cc() {
}

} // end namespace cgra
