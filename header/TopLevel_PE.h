#ifndef TOPLEVEL_H_
#define TOPLEVEL_H_

#include <systemc>
#include "TestBench_PE.h"
#include "ProcessingElement.h"

namespace cgra {

class TopLevel : public sc_core::sc_module {
public:
	TopLevel(sc_core::sc_module_name nameA);
	virtual ~TopLevel();

	//modules
	TestBench* tb;
	Processing_Element<8,8>* pe;
private:
	static unsigned int pe_generation_counter;

	//internal signals
	sc_core::sc_signal<Processing_Element<8,8>::input_type_t> s_in1{"In_01"};
	sc_core::sc_signal<Processing_Element<8,8>::input_type_t> s_in2{"In_02"};
	sc_core::sc_signal<Processing_Element<8,8>::output_type_t> s_res{"Res"};
	sc_core::sc_signal<Processing_Element<8,8>::config_type_t> s_op{"Opcode"};
	sc_core::sc_clock s_clk{"clk", 2, sc_core::SC_NS};
	sc_core::sc_signal<Processing_Element<8,8>::enable_type_t> s_enable{"Enable"};
	sc_core::sc_signal<Processing_Element<8,8>::valid_type_t> s_valid{"Valid"};

	//Forbidden Constructors
	TopLevel() = delete;
	TopLevel(const TopLevel& src) = delete;
	TopLevel(TopLevel&& src) = delete;
	TopLevel& operator=(const TopLevel& src) = delete;
	TopLevel& operator=(TopLevel&& src) = delete;
};

} //end namespace cgra

#endif
