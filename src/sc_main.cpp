/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include "General_DeMux.h"
#include "General_Mux.h"
#include "TB_MMU.h"
#include "MMU.h"

int sc_main(int argc, char* arcv[])
{
	//include TB
	auto tb_mmu = new cgra::TB_MMU{"MMU_TestBench"};

//#############################################################################

	//instantiate modules
	auto mmu = new cgra::MMU{"mmu", cgra::cCacheFeatures};
	//< instance memory management unit
	auto pe_cc = new cgra::pe_config_cache_type_t{"pe_confCache", cgra::cPeConfigBitWidth};
	//< instance pe configuration cache
	auto vc_cc = new cgra::ch_config_cache_type_t{"vCh_confCache", cgra::cVChConfigBitWidth};
	//< instance virtual channel and mask configuration cache
	auto dic = new cgra::data_input_cache_type_t{"dataInCache"};
	//< instance data input cache
	auto doc = new cgra::data_output_cache_type_t{"dataOutCache"};
	//< instance data output cache
	auto ack_mux = new cgra::General_Mux<cgra::MMU::ack_type_t, 4, 3>{"Mux_ACK"};
	//< instance demultiplex configuration cache stream
	auto we_dmux = new cgra::General_DeMux<cgra::MMU::write_type_t, 4, 3>{"DeMux_WE"};
	//< instance demultiplex write_enable


//#############################################################################

	//signals
	sc_core::sc_clock s_clk{"clk", 4, sc_core::SC_NS};
	sc_core::sc_signal<cgra::MMU::address_type_t> s_address{"Address"};
	sc_core::sc_signal<cgra::MMU::place_type_t> s_place{"Place"};
	sc_core::sc_signal<cgra::MMU::cache_select_type_t> s_cacheSelect{"Cache_Select"};
	sc_core::sc_signal<cgra::MMU::start_type_t> s_start{"Start"};
	sc_core::sc_signal<cgra::MMU::ack_type_t> s_ack{"Acknowledge"};
	sc_core::sc_signal<cgra::MMU::data_stream_type_t> s_dataValueInStream{"DataOutCacheToMemory"};
	sc_core::sc_signal<cgra::MMU::ready_type_t> s_ready{"Ready"};
	sc_core::sc_signal<cgra::data_output_cache_type_t::update_type_t> s_update{"Update"};
	sc_core::sc_signal<cgra::MMU::write_type_t> s_writeEnable{"WriteEnable"};
	sc_core::sc_signal<cgra::MMU::conf_stream_type_t> s_confCacheStream{"ConfigCacheStream"};
	sc_core::sc_signal<cgra::MMU::data_stream_type_t> s_dataValueOutStream{"MemoryToDataInCache"};
	sc_core::sc_signal<cgra::MMU::cache_place_type_t> s_cachePlace{"CachePlace"};
	sc_core::sc_signal<cgra::pe_config_cache_type_t::select_type_t> s_Pe_SltIn{"PeCache_SelectIn"};
	sc_core::sc_signal<cgra::pe_config_cache_type_t::select_type_t> s_Pe_SltOut{"PeCache_SelectOut"};
	sc_core::sc_signal<cgra::ch_config_cache_type_t::select_type_t> s_VCh_SltIn{"vChCache_SelectIn"};
	sc_core::sc_signal<cgra::ch_config_cache_type_t::select_type_t> s_VCh_SltOut{"vChCache_SelectOut"};
	sc_core::sc_signal<cgra::data_input_cache_type_t::select_lines_type_t> s_Dic_SltIn{"DataInCache_SelectIn"};
	sc_core::sc_signal<cgra::data_input_cache_type_t::select_lines_type_t> s_Dic_SltOut{"DataInCache_SelectOut"};
	sc_core::sc_signal<cgra::data_output_cache_type_t::select_lines_type_t> s_Doc_SltIn{"DataOutCache_SelectIn"};
	sc_core::sc_signal<cgra::data_output_cache_type_t::select_lines_type_t> s_Doc_SltOut{"DataOutCache_SelectOut"};
	std::array<sc_core::sc_signal<cgra::MMU::ack_type_t>, 4> s_acks;
	std::array<sc_core::sc_signal<cgra::MMU::write_type_t>, 4> s_writeEnables;
	sc_core::sc_signal<cgra::pe_config_cache_type_t::config_type_t> s_PeCcConfiguration{"PE_CC_Configuration"};
	sc_core::sc_signal<cgra::ch_config_cache_type_t::config_type_t> s_VcCcConfiguration{"VCH_CC_Configuration"};
	std::array<sc_core::sc_signal<cgra::TB_MMU::value_type_t>, cgra::cNumberOfValuesPerCacheLine> s_Values;
	std::array<sc_core::sc_signal<cgra::TB_MMU::result_type_t>, cgra::cNumberOfValuesPerCacheLine> s_Results;

//#############################################################################

	//Port bindings

	//clock
	pe_cc->clk.bind(s_clk);
	vc_cc->clk.bind(s_clk);
	dic->clk.bind(s_clk);
	doc->clk.bind(s_clk);
	mmu->clk.bind(s_clk);

	//control signals
	tb_mmu->sStart.bind(s_start);
	tb_mmu->sReady.bind(s_ready);
	tb_mmu->sPe_sltIn.bind(s_Pe_SltIn);
	tb_mmu->sPe_sltOut.bind(s_Pe_SltOut);
	tb_mmu->sVCh_sltIn.bind(s_VCh_SltIn);
	tb_mmu->sVCh_sltOut.bind(s_VCh_SltOut);
	tb_mmu->sDic_sltIn.bind(s_Dic_SltIn);
	tb_mmu->sDic_sltOut.bind(s_Dic_SltOut);
	tb_mmu->sDoc_sltIn.bind(s_Doc_SltIn);
	tb_mmu->sDoc_sltOut.bind(s_Doc_SltOut);
	tb_mmu->sUpdate.bind(s_update);
	mmu->start.bind(s_start);
	mmu->ready.bind(s_ready);
	pe_cc->slt_in.bind(s_Pe_SltIn);
	pe_cc->slt_out.bind(s_Pe_SltOut);
	vc_cc->slt_in.bind(s_VCh_SltIn);
	vc_cc->slt_out.bind(s_VCh_SltOut);
	dic->slt_in.bind(s_Dic_SltIn);
	dic->slt_out.bind(s_Dic_SltOut);
	doc->slt_in.bind(s_Doc_SltIn);
	doc->slt_out.bind(s_Doc_SltOut);
	doc->update.bind(s_update);

	//data signals
	tb_mmu->sAddress.bind(s_address);
	tb_mmu->sPlace.bind(s_place);
	tb_mmu->sCache_select.bind(s_cacheSelect);
	tb_mmu->sPeCcCurrentConf.bind(s_PeCcConfiguration);
	tb_mmu->sVChCcCurrentConf.bind(s_VcCcConfiguration);
	mmu->data_value_in_stream.bind(s_dataValueOutStream);
	mmu->data_value_out_stream.bind(s_dataValueInStream);
	mmu->conf_cache_stream(s_confCacheStream);
	mmu->address.bind(s_address);
	mmu->place.bind(s_place);
	mmu->cache_select.bind(s_cacheSelect);
	mmu->cache_place.bind(s_cachePlace);
	dic->slt_place.bind(s_cachePlace);
	dic->dataInStream.bind(s_dataValueInStream);
	doc->slt_place.bind(s_cachePlace);
	doc->dataOutStream.bind(s_dataValueOutStream);
	pe_cc->dataInStream.bind(s_confCacheStream);
	vc_cc->dataInStream.bind(s_confCacheStream);
	pe_cc->currentConfig.bind(s_PeCcConfiguration);
	vc_cc->currentConfig.bind(s_VcCcConfiguration);

	for(uint8_t idx = 0; cgra::cNumberOfValuesPerCacheLine > idx; ++idx)
	{
		doc->currentResults.at(idx).bind(s_Results.at(idx));
		tb_mmu->sResults.at(idx).bind(s_Results.at(idx));

		dic->currentValues.at(idx).bind(s_Values.at(idx));
		tb_mmu->sValues.at(idx).bind(s_Values.at(idx));
	}

	//Multiplexer acknowledges
	ack_mux->select.bind(s_cacheSelect);
	ack_mux->inputs.at(0).bind(s_acks.at(0));	//Data Input Cache
	dic->ack.bind(s_acks.at(0));
	ack_mux->inputs.at(1).bind(s_acks.at(1));	//Data Output Cache
	doc->ack.bind(s_acks.at(1));
	ack_mux->inputs.at(2).bind(s_acks.at(2));	//PE Configuration Cache
	pe_cc->ack.bind(s_acks.at(2));
	ack_mux->inputs.at(3).bind(s_acks.at(3));	//vCh Configuration Cache
	vc_cc->ack.bind(s_acks.at(3));
	ack_mux->output.bind(s_ack);
	mmu->ack.bind(s_ack);


	//Demultiplexer write enable
	we_dmux->select.bind(s_cacheSelect);
	mmu->write_enable.bind(s_writeEnable);
	we_dmux->input.bind(s_writeEnable);
	we_dmux->outputs.at(0).bind(s_writeEnables.at(0)); //Data Input Cache
	dic->write.bind(s_writeEnables.at(0));
	we_dmux->outputs.at(1).bind(s_writeEnables.at(1)); //Data Output Cache
	doc->load.bind(s_writeEnables.at(1));
	we_dmux->outputs.at(2).bind(s_writeEnables.at(2)); //PE Configuration Cache
	pe_cc->write.bind(s_writeEnables.at(2));
	we_dmux->outputs.at(3).bind(s_writeEnables.at(3)); //vCh Configuration Cache
	vc_cc->write.bind(s_writeEnables.at(3));


//#############################################################################

	//create and setup trace file;
	auto fp_mmu = sc_core::sc_create_vcd_trace_file("mmu_test");

	sc_core::sc_trace(fp_mmu, s_clk, "clock");
	sc_core::sc_trace(fp_mmu, s_Dic_SltIn, "DIC_Slt_In");
	sc_core::sc_trace(fp_mmu, s_Dic_SltOut, "DIC_Slt_Out");
	sc_core::sc_trace(fp_mmu, s_Doc_SltIn, "DOC_Slt_In");
	sc_core::sc_trace(fp_mmu, s_Doc_SltOut, "DOC_Slt_Out");
	sc_core::sc_trace(fp_mmu, s_Pe_SltIn, "PE_CC_Slt_In");
	sc_core::sc_trace(fp_mmu, s_Pe_SltOut, "PE_CC_Slt_Out");
	sc_core::sc_trace(fp_mmu, s_VCh_SltIn, "VC_CC_Slt_In");
	sc_core::sc_trace(fp_mmu, s_VCh_SltOut, "VC_CC_Slt_Out");
	sc_core::sc_trace(fp_mmu, s_address, "Address");
	sc_core::sc_trace(fp_mmu, s_place, "Place");
	sc_core::sc_trace(fp_mmu, s_cacheSelect, "Cache_Select");
	sc_core::sc_trace(fp_mmu, s_cachePlace, "Cache_Place");
	sc_core::sc_trace(fp_mmu, s_ack, "Acknowledge");
	sc_core::sc_trace(fp_mmu, s_dataValueInStream, "DataInStream");
	sc_core::sc_trace(fp_mmu, s_dataValueOutStream, "DataOutStream");
	sc_core::sc_trace(fp_mmu, s_confCacheStream, "ConfStream");
	sc_core::sc_trace(fp_mmu, s_writeEnable, "Write_Enable");
	sc_core::sc_trace(fp_mmu, s_ready, "Ready");
	sc_core::sc_trace(fp_mmu, s_start, "Start");
	sc_core::sc_trace(fp_mmu, s_PeCcConfiguration, "PE_CC_CurrentConfig");
	sc_core::sc_trace(fp_mmu, s_VcCcConfiguration, "VCH_CC_CurrentConfig");
	sc_core::sc_trace(fp_mmu, s_update, "Update");

	{	//Lambda function to create port name in trace file
		uint8_t i = 0;
		auto concat = [](uint8_t i, const char* preamble){
			std::ostringstream s{""};
			s << preamble << static_cast<short>(i);
			return s.str();
		};

		//Trace write_enables and acknowledges
		for(auto& we : s_writeEnables)
			sc_core::sc_trace(fp_mmu, we, concat(i++, "we_"));

		i = 0;
		for(auto& ack : s_acks)
			sc_core::sc_trace(fp_mmu, ack, concat(i++, "ack_"));

		//Trace values and results
		i = 0;
		for(auto& val : s_Values)
			sc_core::sc_trace(fp_mmu, val, concat(i++, "Value_"));

		i = 0;
		for(auto& res : s_Results)
			sc_core::sc_trace(fp_mmu, res, concat(i++, "Result_"));
	}
//#############################################################################

	//Write content in shared memory
	uint16_t tcontent[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	//configuration data type needs to be a byte type, otherwise configuration bitstreams will have the wrong bit order
	uint8_t tconfig[] = {0xaf, 0xfe, 0xaf, 0xfe, 0xde, 0xde, 0x00, 0x00, 0x12, 0x34, 0x56, 0x78, 0xAB, 0xCD, 0xEF, 0x00};
	mmu->write_shared_memory<uint16_t>(0, tcontent, 16);
	mmu->write_shared_memory<uint8_t>(80, tconfig, 16);


	//Run simulation
	sc_core::sc_start(1500, sc_core::SC_NS);

#ifdef DEBUG

	std::cout << "Dump after Test" << std::endl;
	tb_mmu->dump();
	std::cout << std::endl;
	mmu->dump();
	std::cout << std::endl;
	mmu->dump_memory<uint16_t>(0, 80, sc_dt::SC_DEC);
	std::cout << std::endl;
	dic->dump();
	std::cout << std::endl;
	we_dmux->dump();
	std::cout << std::endl;
	ack_mux->dump();
	std::cout << std::endl;
	doc->dump();
	std::cout << std::endl;
	pe_cc->dump();
	std::cout << std::endl;
	vc_cc->dump();
	std::cout << std::endl;

#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_mmu);

	return EXIT_SUCCESS;
};
