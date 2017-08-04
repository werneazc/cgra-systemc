/*
 * TestBenchMUX.cpp
 *
 *  Created on: Aug 4, 2017
 *      Author: andrewerner
 */

#include <TestBenchMUX.h>
#include <iostream>

namespace cgra {

TestBench_MUX::TestBench_MUX(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA) {

	for(uint8_t i = 0; i < 4; ++i)
	{
		s_mux_in[i].initialize((i+1)*10);
		if (i % 2)
			s_valid_in[i].initialize(false);
		else
			s_valid_in[i].initialize(true);
	}

	s_select.initialize("00");

	SC_THREAD(stimuli);
//	sensitive << r_sel_data << r_sel_valid.pos();

}

void TestBench_MUX::stimuli() {


	// switch between all inputs
	for (uint8_t i = 1; i <= 4; ++i)
	{
		using namespace std;

		cout << "@ " << sc_core::sc_time_stamp();
		cout << " MUX - Status before next switch" << endl;

		cout << "selected data in: " << std::dec << s_mux_in[i-1].read() << "\n";
		cout << "selected valid in: "  << s_valid_in[i-1].read() << "\n";

		wait(5, sc_core::SC_NS);
		cout << "selected data out: " << std::dec << r_sel_data.read() << "\n";
		cout << "selected valid out: "  << r_sel_valid.read() << "\n";

		if(i == 4)
			break;
		s_select.write(i);
	}

	//switch input values and hold select
	for (uint8_t i = 0; i <= 4; ++i)
	{
		using namespace std;

		cout << "@ " << sc_core::sc_time_stamp();
		cout << " MUX - Status before next value" << endl;

		s_mux_in[3].write((i+1)*20);
		s_valid_in[3].write((i%2) ? true : false);

		wait(5, sc_core::SC_NS);
		cout << "selected data in: " << std::dec << s_mux_in[3].read() << "\n";
		cout << "selected valid in: "  << s_valid_in[3].read() << "\n";
		cout << "selected data out: " << std::dec << r_sel_data.read() << "\n";
		cout << "selected valid out: "  << r_sel_valid.read() << "\n";
	}

	//Hurt border of used data-ports
	//This raises an error and stops simulation
	wait(5, sc_core::SC_NS);
	s_select.write("111");

	wait();
}

} /* namespace cgra */
