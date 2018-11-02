/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include "TB_DataOutCache.h"
#include "DataOutCache.h"

int sc_main(int argc, char* arcv[])
{
	//include TB
	auto tb_doc = new cgra::TB_DataOutCache{"TestBench_DataOutCache"};

//#############################################################################

	//instantiate modules
	auto doc = new cgra::DataOutCache<16, 4, 2>{"DataOutCache"};

//#############################################################################

	//signals
	sc_core::sc_clock clk{"clk", 4, sc_core::SC_NS};
	sc_core::sc_signal<cgra::TB_DataOutCache::stream_type_t> s_dataStream{"DataStream"};
	sc_core::sc_signal<cgra::TB_DataOutCache::select_lines_type_t> s_slt_in{"Select_in"};
	sc_core::sc_signal<cgra::TB_DataOutCache::select_lines_type_t> s_slt_out{"Select_out"};
	sc_core::sc_signal<cgra::TB_DataOutCache::select_value_type_t> s_slt_place{"Select_Place"};
	sc_core::sc_signal<cgra::TB_DataOutCache::update_type_t> s_update{"Update"};
	sc_core::sc_signal<cgra::TB_DataOutCache::load_enable_type_t> s_load{"Load"};
	sc_core::sc_signal<cgra::TB_DataOutCache::ack_type_t> s_ack{"Acknowledge"};
	std::array<sc_core::sc_signal<cgra::TB_DataOutCache::value_type_t>, 4> s_currentResults;

//#############################################################################

	//Port bindings

	//clock
	tb_doc->sClk.bind(clk);
	doc->clk.bind(clk);

	//control signals
	tb_doc->sSlt_in.bind(s_slt_in);
	tb_doc->sSlt_out.bind(s_slt_out);
	tb_doc->sSlt_place.bind(s_slt_place);
	tb_doc->sUpdate.bind(s_update);
	tb_doc->sLoad.bind(s_load);
	tb_doc->sAck.bind(s_ack);
	doc->slt_in.bind(s_slt_in);
	doc->slt_out.bind(s_slt_out);
	doc->slt_place.bind(s_slt_place);
	doc->update.bind(s_update);
	doc->load.bind(s_load);
	doc->ack.bind(s_ack);

	//data signals
	tb_doc->sDataOutStream.bind(s_dataStream);
	doc->dataOutStream.bind(s_dataStream);
	for(uint32_t i = 0; 4 > i; ++i)
	{
		tb_doc->sCurrentResults.at(i).bind(s_currentResults.at(i));
		doc->currentResults.at(i).bind(s_currentResults.at(i));
	}

//#############################################################################

	//create and setup trace file;
	auto fp_doc = sc_core::sc_create_vcd_trace_file("doc_test");

	sc_core::sc_trace(fp_doc,clk,"clock");
	sc_core::sc_trace(fp_doc,s_slt_in,"Select_In");
	sc_core::sc_trace(fp_doc,s_slt_out,"Select_Out");
	sc_core::sc_trace(fp_doc,s_slt_place,"Select_Place");
	sc_core::sc_trace(fp_doc,s_update,"Update");
	sc_core::sc_trace(fp_doc,s_load,"Load");
	sc_core::sc_trace(fp_doc,s_ack,"Acknowledge");
	sc_core::sc_trace(fp_doc,s_dataStream,"DataStream");

	{	//Lambda function to create port name in trace file
		uint8_t i = 0;
		auto concat = [](uint8_t i, const char* preamble){
			std::ostringstream s{""};
			s << preamble << static_cast<short>(i);
			return s.str();
		};

		//Trace inputs
		for(auto& in : s_currentResults)
			sc_core::sc_trace(fp_doc,in, concat(i++, "place_"));

	}
//#############################################################################

	//Run simulation
	sc_core::sc_start(200,sc_core::SC_NS);

#ifdef DEBUG
	tb_doc->dump();
	doc->dump();
#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_doc);

	return EXIT_SUCCESS;
};
