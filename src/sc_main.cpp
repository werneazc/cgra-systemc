/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include "TB_DataInCache.h"
#include "DataInCache.h"

int sc_main(int argc, char* arcv[])
{
	//include TB
	auto tb_dic = new cgra::TB_DataInCache{"TestBench_DataInCache"};

//#############################################################################

	//instantiate modules
	auto dic = new cgra::DataInCache<16, 4, 2>{"DataInCache"};

//#############################################################################

	//signals
	sc_core::sc_clock clk{"clk", 4, sc_core::SC_NS};
	sc_core::sc_signal<cgra::TB_DataInCache::stream_type_t> s_dataStream{"DataStream"};
	sc_core::sc_signal<cgra::TB_DataInCache::select_lines_type_t> s_slt_in{"Select_in"};
	sc_core::sc_signal<cgra::TB_DataInCache::select_lines_type_t> s_slt_out{"Select_out"};
	sc_core::sc_signal<cgra::TB_DataInCache::select_value_type_t> s_slt_place{"Select_Place"};
	sc_core::sc_signal<cgra::TB_DataInCache::write_enable_type_t> s_write{"Write"};
	sc_core::sc_signal<cgra::TB_DataInCache::ack_type_t> s_ack{"Acknowledge"};
	std::array<sc_core::sc_signal<cgra::TB_DataInCache::value_type_t>, 4> s_currentValues;

//#############################################################################

	//Port bindings

	//clock
	tb_dic->sClk.bind(clk);
	dic->clk.bind(clk);

	//control signals
	tb_dic->sSlt_in.bind(s_slt_in);
	tb_dic->sSlt_out.bind(s_slt_out);
	tb_dic->sSlt_place.bind(s_slt_place);
	tb_dic->sWrite.bind(s_write);
	tb_dic->sAck.bind(s_ack);
	dic->slt_in.bind(s_slt_in);
	dic->slt_out.bind(s_slt_out);
	dic->slt_place.bind(s_slt_place);
	dic->write.bind(s_write);
	dic->ack.bind(s_ack);

	//data signals
	tb_dic->sDataInStream.bind(s_dataStream);
	dic->dataInStream.bind(s_dataStream);
	for(uint32_t i = 0; 4 > i; ++i)
	{
		tb_dic->sCurrentValues.at(i).bind(s_currentValues.at(i));
		dic->currentValues.at(i).bind(s_currentValues.at(i));
	}

//#############################################################################

	//create and setup trace file;
	auto fp_vcgra = sc_core::sc_create_vcd_trace_file("dic_test");

	sc_core::sc_trace(fp_vcgra,clk,"clock");
	sc_core::sc_trace(fp_vcgra,s_slt_in,"Select_In");
	sc_core::sc_trace(fp_vcgra,s_slt_out,"Select_Out");
	sc_core::sc_trace(fp_vcgra,s_slt_place,"Select_Place");
	sc_core::sc_trace(fp_vcgra,s_write,"Write");
	sc_core::sc_trace(fp_vcgra,s_ack,"Acknowledge");
	sc_core::sc_trace(fp_vcgra,s_dataStream,"DataStream");

	{	//Lambda function to create port name in trace file
		uint8_t i = 0;
		auto concat = [](uint8_t i, const char* preamble){
			std::ostringstream s{""};
			s << preamble << static_cast<short>(i);
			return s.str();
		};

		//Trace inputs
		for(auto& in : s_currentValues)
			sc_core::sc_trace(fp_vcgra,in, concat(i++, "place_"));

	}
//#############################################################################

	//Run simulation
	sc_core::sc_start(1,sc_core::SC_MS);

#ifdef DEBUG
	tb_dic->dump();
	dic->dump();
#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_vcgra);

	return EXIT_SUCCESS;
};
