/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include "TopLevel_PE.h"



int sc_main(int argc, char* arcv[])
{
	//include TB
	cgra::TopLevel TopLevel("PE_Test");

	//trace signals
	auto fp = sc_core::sc_create_vcd_trace_file("pe_test");

	sc_core::sc_trace(fp, TopLevel.pe->clk, "clock");
	sc_core::sc_trace(fp, TopLevel.pe->in1, "in1");
	sc_core::sc_trace(fp, TopLevel.pe->in2, "in2");
	sc_core::sc_trace(fp, TopLevel.pe->res, "res");
	sc_core::sc_trace(fp, TopLevel.pe->valid, "valid");
	sc_core::sc_trace(fp, TopLevel.pe->enable, "enable");
	sc_core::sc_trace(fp, TopLevel.pe->conf, "conf");

	sc_core::sc_start(100, sc_core::SC_NS);

	sc_core::sc_close_vcd_trace_file(fp);

	return 0;
}



