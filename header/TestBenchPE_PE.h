#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
#include <iostream>

namespace cgra {

/*!
 * \class TestBench_PE
 *
 * \brief Test bench for processing-element function-tests
 *
 * \details
 * Test bench creates stimuli-signals to test a processing-
 * element (PE). All supported PE operations are tested
 * sequentially.
 */
class TestBench_PE : public sc_core::sc_module {
public:
	//Entity ports:
	sc_core::sc_out<sc_dt::sc_int<8>> s_in1{"s_In1"};
	//!< \brief Input signal for operand one of a PE
	sc_core::sc_out<sc_dt::sc_int<8>> s_in2{"s_In2"};
	//!< \brief Input signal for operand two of a PE
	sc_core::sc_in<sc_dt::sc_int<8>> s_res{"s_res"};
	//!< \brief Output signal for result of a PE
	sc_core::sc_out<sc_dt::sc_lv<2>> s_enable{"s_enable"};
	//!< \brief Input signals of a PE to mark an input as valid
	sc_core::sc_in<bool> s_valid{"s_valid"};
	//!< \brief Output signal of a PE to mark current value as valid
	sc_core::sc_out<sc_dt::sc_lv<4>> s_op{"s_op"};
	//!< \brief Selects the operation of a PE

	SC_HAS_PROCESS(TestBench_PE);
	/*!
	 * \brief Constsructor
	 *
	 * \param nameA sc_module name of TestBench;
	 */
	TestBench_PE();

	/*
	 * \brief stimuli for PE
	 */
	void stimuli();

	/*!
	 * \brief write current signals to output
	 */
	friend std::ostream& operator<<(std::ostream& os, const TestBench_PE& tb);

private:
	/*!
	 * \brief Print current status of PE signals
	 *
	 * \param os out-stream where to write to (default: std::cout)
	 */
	void print(std::ostream& os = ::std::cout);
};

} // end namespace cgra


#endif //TESTBENCH_H_
