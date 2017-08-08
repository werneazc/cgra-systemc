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
#include "VirtualChannel.h"
#include "TestBenchVC.h"


int sc_main(int argc, char* arcv[])
{
	//instantiate modules
	auto mux = new cgra::Multiplexer<4,3>{"MUX_DUT"};
	auto tb_mux = new cgra::TestBench_MUX{"TB_MUX"};
	auto vc = new cgra::VirtualChannel<4, 8, 8, 8, 2>{"DUT_VC"};
	auto tb_vc = new cgra::TestBenchVC{"TB_VC"};

	//signals
	std::array<sc_core::sc_signal<cgra::Multiplexer<4,3>::data_type_t>, 4> data_lines;
	std::array<sc_core::sc_signal<cgra::Multiplexer<4,3>::valid_type_t>, 4> valid_lines;
	sc_core::sc_signal<cgra::Multiplexer<4,3>::select_type_t> select_lines{"select_lines"};
	sc_core::sc_signal<cgra::Multiplexer<4,3>::data_type_t> result_data_line{"result_data"};
	sc_core::sc_signal<cgra::Multiplexer<4,3>::valid_type_t> result_valid_line{"result_valid"};
	//-----------------------------------------------------------------------------------------------
	sc_core::sc_signal<cgra::VirtualChannel<4, 8, 8, 8, 2>::conf_type_t> confi_lines{"config_line"};
	sc_core::sc_signal<cgra::VirtualChannel<4, 8, 8, 8, 2>::reset_type_t> rst_line{"rst_line"};
	sc_core::sc_clock clk{"clock", 2, sc_core::SC_NS};
	std::array<sc_core::sc_signal<cgra::VirtualChannel<4, 8, 8, 8, 2>::enables_type_t>, 8> enable_lines;
	std::array<sc_core::sc_signal<cgra::VirtualChannel<4, 8, 8, 8, 2>::output_type_t>, 8> output_lines;
	std::array<sc_core::sc_signal<cgra::VirtualChannel<4, 8, 8, 8, 2>::valid_type_t>, 4> valid_lines_vc;
	std::array<sc_core::sc_signal<cgra::VirtualChannel<4, 8, 8, 8, 2>::input_type_t>, 4> input_lines;

	//bind mux and testbench ports
	for(uint8_t i = 0; i < 4; ++i)
	{
		mux->data_inputs[i].bind(data_lines[i]);
		mux->valid_inputs[i].bind(valid_lines[i]);
		tb_mux->s_mux_in[i].bind(data_lines[i]);
		tb_mux->s_valid_in[i].bind(valid_lines[i]);
	}

	tb_mux->s_select.bind(select_lines);
	mux->select.bind(select_lines);
	tb_mux->r_sel_data.bind(result_data_line);
	mux->sel_data.bind(result_data_line);
	tb_mux->r_sel_valid.bind(result_valid_line);
	mux->sel_valid.bind(result_valid_line);

	//---------------------------------------------------------------------------------------------------

	for(uint8_t i, j = 0; i < 4; ++i)
	{
		vc->channel_inputs[i].bind(input_lines[i]);
		vc->valids[i].bind(valid_lines_vc[i]);
		vc->channel_outputs[j].bind(output_lines[j]);
		vc->enables[j].bind(enable_lines[j]);
		vc->channel_outputs[j+1].bind(output_lines[j+1]);
		vc->enables[j+1].bind(enable_lines[j+1]);
		tb_vc->s_inData[i].bind(input_lines[i]);
		tb_vc->s_inValid[i].bind(valid_lines_vc[i]);
		tb_vc->r_outData[j].bind(output_lines[j]);
		tb_vc->r_enables[j].bind(enable_lines[j]);
		tb_vc->r_outData[j+1].bind(output_lines[j+1]);
		tb_vc->r_enables[j+1].bind(enable_lines[j+1]);
		j+=2;
	}

	vc->conf.bind(confi_lines);
	tb_vc->s_config.bind(confi_lines);
	vc->clk(clk);
	vc->rst.bind(rst_line);
	tb_vc->s_rst.bind(rst_line);


	//create and setup trace file;
	auto fp_mux = sc_core::sc_create_vcd_trace_file("mux_test");
	auto fp_vc = sc_core::sc_create_vcd_trace_file("vc_test");

	sc_core::sc_trace(fp_mux, mux->data_inputs[0], "data_line_0");
	sc_core::sc_trace(fp_mux, mux->data_inputs[1], "data_line_1");
	sc_core::sc_trace(fp_mux, mux->data_inputs[2], "data_line_2");
	sc_core::sc_trace(fp_mux, mux->data_inputs[3], "data_line_3");
	sc_core::sc_trace(fp_mux, mux->valid_inputs[0], "valid_line_0");
	sc_core::sc_trace(fp_mux, mux->valid_inputs[1], "valid_line_1");
	sc_core::sc_trace(fp_mux, mux->valid_inputs[2], "valid_line_2");
	sc_core::sc_trace(fp_mux, mux->valid_inputs[3], "valid_line_3");
	sc_core::sc_trace(fp_mux, mux->sel_data, "seleted_data");
	sc_core::sc_trace(fp_mux, mux->sel_valid, "seleted_valid");
	sc_core::sc_trace(fp_mux, mux->select, "select");
	//----------------------------------------------------------------
	sc_core::sc_trace(fp_vc, vc->channel_inputs[0], "data_In_00");
	sc_core::sc_trace(fp_vc, vc->channel_inputs[1], "data_In_01");
	sc_core::sc_trace(fp_vc, vc->channel_inputs[2], "data_In_02");
	sc_core::sc_trace(fp_vc, vc->channel_inputs[3], "data_In_03");
	sc_core::sc_trace(fp_vc, vc->valids[0], "valid_00");
	sc_core::sc_trace(fp_vc, vc->valids[1], "valid_01");
	sc_core::sc_trace(fp_vc, vc->valids[2], "valid_02");
	sc_core::sc_trace(fp_vc, vc->valids[3], "valid_03");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[0], "data_out_00");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[1], "data_out_01");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[2], "data_out_02");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[3], "data_out_03");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[4], "data_out_04");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[5], "data_out_05");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[6], "data_out_06");
	sc_core::sc_trace(fp_vc, vc->channel_outputs[7], "data_out_07");
	sc_core::sc_trace(fp_vc, vc->enables[0], "enables_00");
	sc_core::sc_trace(fp_vc, vc->enables[1], "enables_01");
	sc_core::sc_trace(fp_vc, vc->enables[2], "enables_02");
	sc_core::sc_trace(fp_vc, vc->enables[3], "enables_03");
	sc_core::sc_trace(fp_vc, vc->enables[4], "enables_04");
	sc_core::sc_trace(fp_vc, vc->enables[5], "enables_05");
	sc_core::sc_trace(fp_vc, vc->enables[6], "enables_06");
	sc_core::sc_trace(fp_vc, vc->enables[7], "enables_07");
	sc_core::sc_trace(fp_vc, vc->conf, "configuration");
	sc_core::sc_trace(fp_vc, vc->rst, "reset");
	sc_core::sc_trace(fp_vc, vc->clk, "clock");

	sc_core::sc_start(100, sc_core::SC_NS);

	sc_core::sc_close_vcd_trace_file(fp_mux);
	sc_core::sc_close_vcd_trace_file(fp_vc);

	return EXIT_SUCCESS;
};
