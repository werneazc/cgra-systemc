#ifndef TOPLEVEL_H_
#define TOPLEVEL_H_

#include <TestBenchPE_PE.h>
#include <systemc>
#include "ProcessingElement.h"

namespace cgra {

/*!
 * \class TopLevel
 *
 * \brief Creates a test environment with a processing-
 * element (PE) and a corresponding test bench (TB).
 */
class TopLevel : public sc_core::sc_module {
public:
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA Name of SystemC module
	 */
	TopLevel(sc_core::sc_module_name nameA);
	virtual ~TopLevel();

	//modules
	TestBench_PE* tb;
	//!< \brief Points to a TB of a PE
	Processing_Element<8,8>* pe;
	//!< \brief Points to a PE
private:
	static unsigned int pe_generation_counter;

	//internal signals
	sc_core::sc_signal<Processing_Element<8,8>::input_type_t> s_in1{"In_01"};
	//!< \brief Connects PE and TB input operand one
	sc_core::sc_signal<Processing_Element<8,8>::input_type_t> s_in2{"In_02"};
	//!< \brief Connects PE and TB input operand two
	sc_core::sc_signal<Processing_Element<8,8>::output_type_t> s_res{"Res"};
	//!< \brief Connects PE and TB ouput result
	sc_core::sc_signal<Processing_Element<8,8>::config_type_t> s_op{"Opcode"};
	//!< \brief Connects PE and TB operation-select
	sc_core::sc_clock s_clk{"clk", 2, sc_core::SC_NS};
	//!< \brief Clock for the PE
	std::array<sc_core::sc_signal<Processing_Element<8,8>::enable_type_t>,2> s_enable;
	//!< \brief Connects PE and TB enable ports
	sc_core::sc_signal<Processing_Element<8,8>::valid_type_t> s_valid{"Valid"};
	//!< \brief Connects PE and TB valid ports

	//Forbidden Constructors
	TopLevel() = delete;
	TopLevel(const TopLevel& src) = delete;				//!< \brief because sc_module could not be copied
	TopLevel(TopLevel&& src) = delete;					//!< \brief because move not implemented for sc_module
	TopLevel& operator=(const TopLevel& src) = delete;	//!< \brief because sc_module could not be copied
	TopLevel& operator=(TopLevel&& src) = delete;		//!< \brief because move not implemented for sc_module
};

} //end namespace cgra

#endif
