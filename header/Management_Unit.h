#ifndef HEADER_MANAGEMENT_UNIT_H_
#define HEADER_MANAGEMENT_UNIT_H_

#include <systemc>
#include <cstdint>
#include <iostream>
#include <array>
#include "CommandInterpreter.h"

namespace cgra {

constexpr uint32_t c_programMemorySize{10};
//!< \brief Set program memory size for assembler commands


/*!
 * \class ManagementUnit
 *
 * \brief Control behavior of VCGRA
 *
 * \details
 * This module is used to control the behavior of a VCGRA.
 * Assembler commands, stored in a dedicated memory control
 * the ManagementUnit. It can load (and store) configuration
 * or process data, start the array or pause the execution
 * until the VCGRA finishes its processing.
 *
 */
class ManagementUnit : public sc_core::sc_module
{
public:
	typedef bool clock_type_t;
	//!< \brief ManagementUnit clock type
	typedef bool ready_type_t;
	//!< \brief Synchronization signal type with VCGRA
	typedef bool start_type_t;
	//!< \brief Start type for VCGRA
	typedef CommandInterpreter::assembler_type_t assembler_type_t;
	//!< \brief Type definition for assembler command

	//Entity Ports
	//============
	sc_core::sc_in<clock_type_t> clk{"clock"};
	//!< \brief ManagementUnit clock port
	sc_core::sc_in<ready_type_t> ready{"ready"};
	//!< \brief (Synchronization) Ready signal port of VCGRA
	sc_core::sc_in<start_type_t> start{"start"};
	//!< \brief (Synchronization) Start signal port for VCGRA


	//Methods
	//=======

	SC_HAS_PROCESS(ManagementUnit);
	/*!
	 * \brief Constructor
	 *
	 * \details
	 * General Constructor of ManagementUnit
	 *
	 * \param nameA SystemC module name
	 */
	ManagementUnit(const sc_core::sc_module_name& nameA);

	/*!
	 * \brief Destructor
	 */
	virtual ~ManagementUnit() = default;


	/*!
	 * \brief
	 */
	void state_machine();

private:

	//Private Members
	//===============
	CommandInterpreter m_cInterpreter{"cInterpreter"};
	//!< \brief Select parts from Assembler Command
	std::array<assembler_type_t, c_programMemorySize> m_programMemory;
	//!< \brief Program memory for assembler program
	assembler_type_t* m_programPointer;
	//!< \brief Program pointer for assembler program

	//Internal signals
	sc_core::sc_buffer<CommandInterpreter::assembler_type_t> m_currentAssembler;
	//!< \brief Local buffer for latest assembler command
	sc_core::sc_buffer<CommandInterpreter::command_type_t> m_currentCommand;
	//!< \brief Local buffer for latest command
	sc_core::sc_buffer<CommandInterpreter::address_type_t> m_currentAddress;
	//!< \brief Local buffer for latest address in shared memory
	sc_core::sc_buffer<CommandInterpreter::line_type_t> m_currentLine;
	//!< \brief Local buffer for latest line number in data cache
	sc_core::sc_buffer<CommandInterpreter::place_type_t> m_currentPlace;
	//!< \brief Local buffer for latest place number in data cache

	/*! ManagementUnit state machine states */
	enum STATE : uint8_t
	{
		NOOP,			//!< \brief No operation
		START,			//!< \brief Start VCGRA
		WAIT_READY,		//!< \brief Pause until VCGRA ready signal
		LOADD,			//!< \brief Load data to data cache
		LOADDA,			//!< \brief Load a whole cache line to data cache
		STORED,			//!< \brief Store data in shared memory
		STOREDA,		//!< \brief Store a whole cache line to shared memory
		LOADPC,			//!< \brief Load complete configuration cache of PEs
		LOADCC,			//!< \brief Load complete configuration cache of vCh
		FETCH,			//!< \brief Load next command from assembler command memory
		DECODE,			//!< \brief Decode assembler command and set next state
		ADAPT_PP		//!< \brief Adapt program pointer (PP)
	} m_current_state;
	//!< Current state of ManagementUnit state machine

	//Private Functions
	//=================

	//Forbidden Constructors
	//----------------------
	ManagementUnit() = delete;
	ManagementUnit(const ManagementUnit& src) = delete;
	ManagementUnit& operator=(const ManagementUnit& src) = delete;
	ManagementUnit(ManagementUnit&& src) = delete;
	ManagementUnit& operator=(ManagementUnit&& src) = delete;

	//state functions
	//---------------
	/*!
	 * \brief Load new assembler command from command memory
	 */
	void fetch();

	/*!
	 * \brief Decode assembler command to set next state
	 */
	void decode();

	/*!
	 * \brief Set program pointer to load next assembler command
	 */
	void adapt_prog_pointer();

	/*!
	 * \brief Start VCGRA processing
	 */
	void start_state();

	/*!
	 * \brief Pause until VCGRA has finished its processing
	 */
	void wait_ready();

	/*!
	 * \brief Load data from shared memory
	 */
	void load_data();

	/*!
	 * \brief Load data cache line from shared memory
	 */
	void load_data_array();

	/*!
	 * \brief store data to shared memory
	 */
	void store_data();

	/*!
	 * \brief Store data cache line to shared memory
	 */
	void store_data_array();

	/*!
	 * \brief Load ProcessingElement configuration cache
	 */
	void load_pe_cc();

	/*!
	 * \brief Load VirtualChannel configuration cache
	 */
	void load_vch_cc();
};

} // namespace cgra

#endif /* HEADER_MANAGEMENT_UNIT_H_ */
