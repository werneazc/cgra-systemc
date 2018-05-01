/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include "TB_VCGRA.h"

int sc_main(int argc, char* arcv[])
{
	//include TB
	auto tb_vcgra = new cgra::TestBench_VCGRA{"VCGRA_TestBench"};

//#############################################################################

	//instantiate modules
	auto vcgra = new cgra::VCGRA{"Small_VCGRA"};

//#############################################################################

	//signals
	sc_core::sc_clock clk{"clk", 4, sc_core::SC_NS};
	sc_core::sc_signal<cgra::VCGRA::start_type_t> start{"start"};
	sc_core::sc_signal<cgra::VCGRA::reset_type_t> rst{"rst"};
	sc_core::sc_signal<cgra::VCGRA::ready_type_t> ready{"ready"};
	std::array<sc_core::sc_signal<cgra::VCGRA::input_type_t>, 8> inputs;
	std::array<sc_core::sc_signal<cgra::VCGRA::output_type_t>, 4> results;
	sc_core::sc_signal<cgra::VCGRA::pe_config_cache_type_t::write_enable_type_t> write_pe{"write_pe"};
	sc_core::sc_signal<cgra::VCGRA::pe_config_cache_type_t::select_type_t> slct_in_pe{"slct_in_pe"};
	sc_core::sc_signal<cgra::VCGRA::pe_config_cache_type_t::select_type_t> slct_out_pe{"slct_out_pe"};
	sc_core::sc_signal<cgra::VCGRA::ch_config_cache_type_t::write_enable_type_t> write_ch{"write_ch"};
	sc_core::sc_signal<cgra::VCGRA::ch_config_cache_type_t::select_type_t> slct_in_ch{"slct_in_ch"};
	sc_core::sc_signal<cgra::VCGRA::ch_config_cache_type_t::select_type_t> slct_out_ch{"slct_out_ch"};
	sc_core::sc_signal<cgra::VCGRA::pe_config_cache_type_t::stream_type_t> pe_config_stream{"pe_config_stream"};
	sc_core::sc_signal<cgra::VCGRA::ch_config_cache_type_t::stream_type_t> ch_config_stream{"ch_config_stream"};
	sc_core::sc_signal<cgra::VCGRA::pe_config_cache_type_t::ack_type_t> ack_pe{"ack_pe"};
	sc_core::sc_signal<cgra::VCGRA::ch_config_cache_type_t::ack_type_t> ack_ch{"ack_ch"};

//#############################################################################

	//Port bindings

	//clock
	tb_vcgra->clk.bind(clk);
	vcgra->clk.bind(clk);

	//control signals
	tb_vcgra->start.bind(start);
	tb_vcgra->ready.bind(ready);
	tb_vcgra->rst.bind(rst);
	tb_vcgra->write_pe_cc.bind(write_pe);
	tb_vcgra->ack_pe_cc.bind(ack_pe);
	tb_vcgra->slct_in_pe_cc.bind(slct_in_pe);
	tb_vcgra->slct_out_pe_cc.bind(slct_out_pe);
	tb_vcgra->write_ch_cc.bind(write_ch);
	tb_vcgra->slct_in_ch_cc.bind(slct_in_ch);
	tb_vcgra->slct_out_ch_cc.bind(slct_out_ch);
	tb_vcgra->ack_ch_cc.bind(ack_ch);
	vcgra->start.bind(start);
	vcgra->ready.bind(ready);
	vcgra->rst.bind(rst);
	vcgra->write_pe_cc.bind(write_pe);
	vcgra->ack_pe_cc.bind(ack_pe);
	vcgra->slct_in_pe_cc.bind(slct_in_pe);
	vcgra->slct_out_pe_cc.bind(slct_out_pe);
	vcgra->write_ch_cc.bind(write_ch);
	vcgra->slct_in_ch_cc.bind(slct_in_ch);
	vcgra->slct_out_ch_cc.bind(slct_out_ch);
	vcgra->ack_ch_cc.bind(ack_ch);

	//data signals
	tb_vcgra->pe_config_stream.bind(pe_config_stream);
	tb_vcgra->ch_config_stream.bind(ch_config_stream);
	vcgra->pe_config_stream.bind(pe_config_stream);
	vcgra->ch_config_stream.bind(ch_config_stream);
	for(uint32_t i = 0; 8 > i; ++i)
	{
		tb_vcgra->inputs.at(i).bind(inputs.at(i));
		vcgra->inputs.at(i).bind(inputs.at(i));
	}
	for(uint32_t i = 0; 4 > i; ++i)
	{
		tb_vcgra->results.at(i).bind(results.at(i));
		vcgra->results.at(i).bind(results.at(i));
	}

//#############################################################################

	//create and setup trace file;
	auto fp_vcgra = sc_core::sc_create_vcd_trace_file("vcgra_test");

	sc_core::sc_trace(fp_vcgra,clk,"clock");
	sc_core::sc_trace(fp_vcgra,start,"start");
	sc_core::sc_trace(fp_vcgra,rst,"rst");
	sc_core::sc_trace(fp_vcgra,ready,"ready");
	sc_core::sc_trace(fp_vcgra,write_pe,"write_pe");
	sc_core::sc_trace(fp_vcgra,ack_pe,"ack_pe,");
	sc_core::sc_trace(fp_vcgra,slct_in_pe,"slct_in_pe");
	sc_core::sc_trace(fp_vcgra,slct_out_pe,"slct_out_pe");
	sc_core::sc_trace(fp_vcgra,write_ch,"write_ch");
	sc_core::sc_trace(fp_vcgra,ack_ch,"ack_ch,");
	sc_core::sc_trace(fp_vcgra,slct_in_ch,"slct_in_ch");
	sc_core::sc_trace(fp_vcgra,slct_out_ch,"slct_out_ch");
	sc_core::sc_trace(fp_vcgra,pe_config_stream,"pe_config_stream");
	sc_core::sc_trace(fp_vcgra,ch_config_stream,"ch_config_stream");


	{	//Lambda function to create port name in trace file
		uint8_t i = 0;
		auto concat = [](uint8_t i, const char* preamble){
			std::ostringstream s{""};
			s << preamble << static_cast<short>(i);
			return s.str();
		};

		//Trace inputs
		for(auto& in : inputs)
			sc_core::sc_trace(fp_vcgra,in, concat(i++, "input_"));

		i = 0;
		for(auto& res : results)
			sc_core::sc_trace(fp_vcgra, res, concat(i++, "result_"));
	}
//#############################################################################

	//Run simulation
	sc_core::sc_start(1,sc_core::SC_MS);

#ifdef DEBUG
	vcgra->dump();
#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_vcgra);

	return EXIT_SUCCESS;
};
