/*
 * TestBench_VCGRA.cpp
 *
 *  Created on: May 1, 2018
 *      Author: andrewerner
 */

#include "TB_VCGRA.h"

namespace cgra {

static const std::array<const char*, 6> pe_configuration{
	"0x13","0x24","0x63","0x58","0x88","0x78"};
//!< \brief Stream elements for Processing_Element configuration
	static const std::array<const char*, 8> ch_configuration{
	"0x05","0x39","0x77","0x11","0xBB","0x05","0xBF","0x00"};
//!< \brief Stream elements for VirtualChannel configuration


TestBench_VCGRA::TestBench_VCGRA(const sc_core::sc_module_name& nameA)
: sc_core::sc_module(nameA)
{
	SC_THREAD(stimuli);
}

void TestBench_VCGRA::stimuli(void)
{

	wait(5, sc_core::SC_NS);

	//Write configuration to caches:
	//------------------------------
	for(uint8_t i = 0; 6 > i; ++i)
	{
		pe_config_stream.write(pe_configuration.at(i));
		wait(clk.posedge_event());
		write_pe_cc.write(true);
		wait(ack_pe_cc.posedge_event());
		write_pe_cc.write(false);
	}
	for(uint8_t j = 0; 8 > j; ++j)
	{
		ch_config_stream.write(ch_configuration.at(j));
		wait(clk.posedge_event());
		write_ch_cc.write(true);
		wait(ack_ch_cc.posedge_event());
		write_ch_cc.write(false);
	}

	//checkout new connection
	slct_in_ch_cc.write('0');
	slct_out_ch_cc.write('1');
	slct_in_pe_cc.write('0');
	slct_out_pe_cc.write('1');

	//Start VCGRA
	start.write(true);
	wait(clk.posedge_event());
	start.write(false);

	//Wait for Results from VCGRA
	wait(ready.posedge_event());

	//Reset VCGRA
	rst.write(false);
	wait(clk.posedge_event());
	rst.write(true);

	//End simulation
	wait(10, sc_core::SC_NS);
	sc_core::sc_stop();

	return;
}

void TestBench_VCGRA::dump(std::ostream& os) const
{

}

void TestBench_VCGRA::end_of_elaboration()
{
	//Initialize test bench signals
	start.initialize(false);
	rst.initialize(true);
	write_pe_cc.initialize(false);
	write_ch_cc.initialize(false);
	slct_in_pe_cc.initialize('1');
	slct_out_pe_cc.initialize('0');
	slct_in_ch_cc.initialize('1');
	slct_out_ch_cc.initialize('0');
	pe_config_stream.initialize("0x00");
	ch_config_stream.initialize("0x00");

	//Initialize input signals for data
	std::array<uint8_t,8> t_initial_values{1, 2, 3, 4, 6 , 5, 8, 4};
	uint8_t i = 0;
	for(auto& input : inputs)
		input.initialize(t_initial_values.at(i++));
}

} // End of namespace cgra

std::ostream& operator<<(std::ostream& os, const cgra::TestBench_VCGRA& vcgra)
{
	return os;
}
