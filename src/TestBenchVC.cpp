/*
 * TestBenchVC.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: andrewerner
 */

#include "TestBenchVC.h"

namespace cgra {

TestBenchVC::TestBenchVC(sc_core::sc_module_name nameA) :
		sc_core::sc_module(nameA)
{
	SC_THREAD(stimuli);
}

void TestBenchVC::stimuli() {

//	using namespace std;
//
//	cout << "Initial Values:\n" << "=========================" << std::endl;
//	print(cout);

	// Rotate configuration left
	{
		sc_dt::sc_lv<2> tmp_val{"00"};
		sc_dt::sc_lv<16> tmp_config{0x0000};

		for(uint8_t i = 0; i < 8; ++i)
		{

			wait(	r_enables[0].value_changed_event() | \
					r_enables[1].value_changed_event() | \
					r_enables[2].value_changed_event() | \
					r_enables[3].value_changed_event() | \
					r_enables[4].value_changed_event() | \
					r_enables[5].value_changed_event() | \
					r_enables[6].value_changed_event() | \
					r_enables[7].value_changed_event());

//			cout << "Values with configuration " << i << "\n====================================" << endl;
//			print(cout);

			tmp_val = s_config.read().range(15,14);
			tmp_config = s_config.read();
			tmp_config = tmp_config.lrotate(2);
			tmp_config |= tmp_val;
			s_config.write(tmp_config);
		}
	}

	//Change input signals while a fixed configuration
	for(uint8_t i = 0; i < 4; ++i)
	{

		s_inData[i].write((i+1) * ((20 * i + 1) % 4) );
		s_inValid[i].write(((i*i) % 5) ? true : false );

		wait(	r_outData[0].value_changed_event() | \
				r_outData[1].value_changed_event() | \
				r_outData[2].value_changed_event() | \
				r_outData[3].value_changed_event() | \
				r_outData[4].value_changed_event() | \
				r_outData[5].value_changed_event() | \
				r_outData[6].value_changed_event() | \
				r_outData[7].value_changed_event());
	}

	s_rst.write(false);

	wait(	r_enables[0].value_changed_event() | \
			r_enables[1].value_changed_event() | \
			r_enables[2].value_changed_event() | \
			r_enables[3].value_changed_event() | \
			r_enables[4].value_changed_event() | \
			r_enables[5].value_changed_event() | \
			r_enables[6].value_changed_event() | \
			r_enables[7].value_changed_event());

	s_rst.write(true);

	wait(	r_enables[0].value_changed_event() | \
			r_enables[1].value_changed_event() | \
			r_enables[2].value_changed_event() | \
			r_enables[3].value_changed_event() | \
			r_enables[4].value_changed_event() | \
			r_enables[5].value_changed_event() | \
			r_enables[6].value_changed_event() | \
			r_enables[7].value_changed_event());

	sc_core::sc_stop();
}

void TestBenchVC::end_of_elaboration() {

	for(uint8_t i = 0; i < 4; ++i)
	{
		s_inData[i].write(10 * i);
		s_inValid[i].write((i % 2) ? false : true);
	}

	s_rst.write(true);

	s_config.write(0xFA50);


}

void TestBenchVC::print(std::ostream& os) {

	sc_module::print(os);

	os << "\n------------------------------------------" << std::endl;

	os << s_rst.name() << ": " << s_rst.read() << "\n";
	os << s_config.name() << ": " << s_config.read().to_string() << "\n";
	for(uint8_t i = 0; i < 4; ++i)
	{
		os << s_inData[i].name() << ": " << s_inData[i].read().to_string() << "\n";
		os << s_inValid[i].name() << ": " << s_inValid[i].read() << "\n";
	}
	for(uint8_t i = 0; i < 8; ++i)
	{
		os << r_outData[i].name() << ": " << r_outData[i].read().to_string() << "\n";
		os << r_enables[i].name() << ": " << r_enables[i].read() << "\n";
	}

	os << std::endl;

}


} /* namespace cgra */

