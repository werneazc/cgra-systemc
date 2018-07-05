#ifndef HEADER_MANAGEMENT_UNIT_H_
#define HEADER_MANAGEMENT_UNIT_H_

#include <systemc>
#include <cstdint>
#include <iostream>
#include <array>
#include "CommandInterpreter.h"
#include "MMU.h"

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
 * or process data, start the array or pause its execution
 * until the VCGRA finishes its processing.
 *
 */
class ManagementUnit : public sc_core::sc_module
{
public:
	typedef bool clock_type_t;
	//!< \brief ManagementUnit clock type
	typedef bool run_type_t;
	//!< \brief ManagementUnit run type
	typedef bool pause_type_t;
	//!< \brief ManagementUnit pause type
	typedef bool reset_type_t;
	//!< \brief ManagementUnit reset type
	typedef bool ready_type_t;
	//!< \brief Synchronization signal type with VCGRA
	typedef bool start_type_t;
	//!< \brief Start type for VCGRA
	typedef CommandInterpreter::assembler_type_t assembler_type_t;
	//!< \brief Type definition for assembler command

	//Entity Ports
	//============
	sc_core::sc_in<run_type_t> run{"run"};
	//!< \brief ManagementUnit run port to start execution
	sc_core::sc_in<pause_type_t> pause{"pause"};
	//!< \brief ManagementUnit pause port to pause execution
	sc_core::sc_in<reset_type_t> rst{"reset"};
	//!< \brief ManagementUnit reset port to restart execution
	sc_core::sc_in<clock_type_t> clk{"clock"};
	//!< \brief ManagementUnit clock port
	sc_core::sc_in<ready_type_t> ready{"ready"};
	//!< \brief (Synchronization) Ready signal port of VCGRA
	sc_core::sc_out<start_type_t> start{"start"};
	//!< \brief (Synchronization) Start signal port for VCGRA
	sc_core::sc_out<CommandInterpreter::address_type_t> address{"Address"};
	//!< \brief Shared memory (start) address of data (sequence)
	sc_core::sc_out<CommandInterpreter::line_type_t> line{"Line"};
	//!< \brief Cache line number of source/target cache
	sc_core::sc_out<CommandInterpreter::place_type_t> place{"Place"};
	//!< \brief Place in cache line to store data from or load data to
	sc_core::sc_out<MMU::cache_select_type_t> cache_select{"Cache_Select"};
	//!< \brief Selection of source/target cache
	sc_core::sc_in<MMU::ready_type_t> mmu_ready{"MMU_Ready"};
	//!< \brief If false, shared memory ready to process new command, else shared memory is processing.
	sc_core::sc_out<MMU::start_type_t> mmu_start{"MMU_start"};
	//!< \brief If false, shared memory ready to process new command, else shared memory is processing.


	//Methods
	//=======

	SC_HAS_PROCESS(ManagementUnit);
	/*!
	 * \brief Constructor
	 *
	 * \details
	 * General Constructor of ManagementUnit
	 *
	 * \param [in] nameA SystemC module name
	 */
	ManagementUnit(const sc_core::sc_module_name& nameA);

	/*!
	 * \brief Destructor
	 */
	virtual ~ManagementUnit() = default;


	/*!
	 * \brief Internal state machine of ManagingUnit
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
	sc_core::sc_buffer<CommandInterpreter::assembler_type_t> m_currentAssembler{"CurrentAssembler"};
	//!< \brief Local buffer for latest assembler command
	sc_core::sc_buffer<CommandInterpreter::command_type_t> m_currentCommand{"CurrentCommand"};
	//!< \brief Local buffer for latest command
	sc_core::sc_buffer<CommandInterpreter::address_type_t> m_currentAddress{"CurrentAddress"};
	//!< \brief Local buffer for latest address in shared memory
	sc_core::sc_buffer<CommandInterpreter::line_type_t> m_currentLine{"CurrentLine"};
	//!< \brief Local buffer for latest line number in data cache
	sc_core::sc_buffer<CommandInterpreter::place_type_t> m_currentPlace{"CurrentPlace"};
	//!< \brief Local buffer for latest place number in data cache
	sc_core::sc_signal<bool> m_readyInterrupt{"VCGRA_Ready_Interrupt"};
	//!< \brief Interrupt buffer for VCGRA ready signal if ManagementUnit does not wait active.


	/*! ManagementUnit processing status */
	enum ACTIVE_STATE : uint8_t
	{
		PAUSE,		//!< \brief Pause ManagementUnit processing
		RUN,		//!< \brief ManagementUnit is processing
		STOP, 		//!< \brief ManagementUnit stops
		WAIT,		//!< \brief ManagementUnit waits for ready signal
		ERROR		//!< \brief Error State for ManagementUnit
	} m_activeState;
	//!< Processing status of ManagementUnit


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
		ADAPT_PP,		//!< \brief Adapt program pointer (PP)
		WAIT_MMU		//!< \brief Synchronization state with MMU
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
	 * \brief Start execution of ManagementUnit
	 *
	 * \brief
	 * A positive edge sets active state of
	 * ManagementUnit to RUN.
	 */
	void run_interrupt();

	/*!
	 * \brief Pause execution of ManagingUnit
	 *
	 * \details
	 * As long as pause signal is set to high, the ManagementUnit
	 * will pause its execution. If it is pulled to low, it
	 * will resume its execution at its last state.
	 */
	void pause_interrupt();

	/*!
	 * \brief Reset ManagingUnit
	 *
	 * \details
	 * If reset port drops to low, The execution will stop,
	 * the program pointer is set to the first assembler command.
	 * All internal registers are cleared. The ManagementUnit will halt.
	 */
	void reset_interrupt();

	/*!
	 * \brief Wait for MMU to finish command processing
	 *
	 * \details
	 * When MMU starts processing, it raises up mmu_ready signal.
	 * On negative edge of mmu_ready signal, this function set
	 * the active status to RUN and increases the program pointer to
	 * address next assembler command. If the latest active state of
	 * the ManagementUnit is not WAIT, the active state is set to ERROR.
	 * In general, the ManagementUnit needs to wait until the MMU stops
	 * processing.
	 *
	 */
	void wait_mmu_ready();

	/*!
	 * \brief Start performing of MMU
	 *
	 * \details
	 * If Assembler Command is decoded and MMU is ready to process new data,
	 * raising mmu_start signal high starts processing of MMU with new
	 * data at address, line, place and cache selection.
	 *
	 */
	void start_mmu();
};

} // namespace cgra

#endif /* HEADER_MANAGEMENT_UNIT_H_ */
