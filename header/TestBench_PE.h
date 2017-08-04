#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
#include <iostream>

namespace cgra {


class TestBench : public sc_core::sc_module {
public:
	//Entity ports:
	sc_core::sc_out<sc_dt::sc_int<8>> s_in1{"s_In1"};
	sc_core::sc_out<sc_dt::sc_int<8>> s_in2{"s_In2"};
	sc_core::sc_in<sc_dt::sc_int<8>> s_res{"s_res"};
	sc_core::sc_out<sc_dt::sc_lv<2>> s_enable{"s_enable"};
	sc_core::sc_in<bool> s_valid{"s_valid"};
	sc_core::sc_out<sc_dt::sc_lv<4>> s_op{"s_op"};

	SC_HAS_PROCESS(TestBench);
	/*!
	 * \brief Constsructor
	 *
	 * \param nameA sc_module name of TestBench;
	 */
	TestBench();

	/*
	 * \brief stimuli for PE
	 */
	void stimuli();

	/*!
	 * \brief write current signals to output
	 */
	friend std::ostream& operator<<(std::ostream& os, const TestBench& tb);

private:
	void print();
};

} // end namespace cgra


#endif //TESTBENCH_H_
