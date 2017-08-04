/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include "Multiplexer.h"
#include "TestBenchMUX.h"


int sc_main(int argc, char* arcv[])
{
	//instantiate modules
	auto mux = new cgra::Multiplexer<4,3>("DUT");
	auto tb = new cgra::TestBench_MUX("TB");

	//signals
	std::array<sc_core::sc_signal<cgra::Multiplexer<4,3>::data_type_t>, 4> data_lines;
	std::array<sc_core::sc_signal<cgra::Multiplexer<4,3>::valid_type_t>, 4> valid_lines;
	sc_core::sc_signal<cgra::Multiplexer<4,3>::select_type_t> select_lines{"select_lines"};
	sc_core::sc_signal<cgra::Multiplexer<4,3>::data_type_t> result_data_line{"result_data"};
	sc_core::sc_signal<cgra::Multiplexer<4,3>::valid_type_t> result_valid_line{"result_valid"};

	//bind mux and testbench ports
	for(uint8_t i = 0; i < 4; ++i)
	{
		mux->data_inputs[i].bind(data_lines[i]);
		mux->valid_inputs[i].bind(valid_lines[i]);
		tb->s_mux_in[i].bind(data_lines[i]);
		tb->s_valid_in[i].bind(valid_lines[i]);
	}

	tb->s_select.bind(select_lines);
	mux->select.bind(select_lines);
	tb->r_sel_data.bind(result_data_line);
	mux->sel_data.bind(result_data_line);
	tb->r_sel_valid.bind(result_valid_line);
	mux->sel_valid.bind(result_valid_line);

	//create and setup trace file;
	auto fp = sc_core::sc_create_vcd_trace_file("mux_test");

	sc_core::sc_trace(fp, mux->data_inputs[0], "data_line_0");
	sc_core::sc_trace(fp, mux->data_inputs[1], "data_line_1");
	sc_core::sc_trace(fp, mux->data_inputs[2], "data_line_2");
	sc_core::sc_trace(fp, mux->data_inputs[3], "data_line_3");
	sc_core::sc_trace(fp, mux->valid_inputs[0], "valid_line_0");
	sc_core::sc_trace(fp, mux->valid_inputs[1], "valid_line_1");
	sc_core::sc_trace(fp, mux->valid_inputs[2], "valid_line_2");
	sc_core::sc_trace(fp, mux->valid_inputs[3], "valid_line_3");
	sc_core::sc_trace(fp, mux->sel_data, "seleted_data");
	sc_core::sc_trace(fp, mux->sel_valid, "seleted_valid");
	sc_core::sc_trace(fp, mux->select, "select");

	sc_core::sc_start(100, sc_core::SC_NS);

	sc_core::sc_close_vcd_trace_file(fp);

	return EXIT_SUCCESS;
};
