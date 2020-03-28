/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include <fstream>
#include "TopLevel.h"
#include "Testbench_TopLevel.h"
#include "Assembler.hpp"

int sc_main(int argc, char* arcv[])
{
	//include TB
	auto tb_toplevel = new cgra::Testbench_TopLevel{"Architecture_TestBench"};

//#############################################################################

    /* 
    This small assembler program load data and configuration from main memory. 
    Afterwards, the preloaded configuration is chosen for the architecture 
    configuration, before the VCGRA is started. The architecture process the data,
    then the results are stored back into the main memory. 
    */
    // std::array<cgra::TopLevel::assembler_type_t, 12> assembly {
    //     "0x00001FC6", //LOADDA 0 0
    //     "0x00321FC9", //LOADPE 50 0
    //     "0x00281FCA", //LOADCH 40 0
    //     "0x00000010", //SLCT_DOC_LINE 0
    //     "0x0000000F", //SLCT_DIC_LINE 0
    //     "0x00000011", //SLCT_PECC_LINE 0
    //     "0x00000012", //SLCT_CHCC_LINE 0
    //     "0x0000000B", //START
    //     "0x00000004", //WAIT_READY
    //     "0x00002010", //SLCT_DOC_LINE 1
    //     "0x00101FC8", //STOREDA 16 0
    //     "0x0000000C"  //FINISH
    // };
	//instantiate modules
	auto toplevel = new cgra::TopLevel{"TopLevel", cgra::assembly.data(), cgra::assembly.size()};

//#############################################################################

	//signals
 	sc_core::sc_clock clk{"clk", 2, sc_core::SC_NS};
 	sc_core::sc_signal<cgra::TopLevel::run_type_t> run{"run", true};
 	sc_core::sc_signal<cgra::TopLevel::reset_type_t> rst{"rst", false};
 	sc_core::sc_signal<cgra::TopLevel::finish_type_t> finish{"finish"};
 	sc_core::sc_signal<cgra::TopLevel::pause_type_t> pause{"pause", false};

//#############################################################################

	//Port bindings

 	//clock
 	toplevel->clk.bind(clk);
    tb_toplevel->clk.bind(clk);

	//control signals
 	toplevel->run.bind(run);
 	toplevel->finish.bind(finish);
 	toplevel->rst.bind(rst);
    toplevel->pause.bind(pause);
 	tb_toplevel->run.bind(run);
 	tb_toplevel->finish.bind(finish);
 	tb_toplevel->rst.bind(rst);
    tb_toplevel->pause.bind(pause);
    
//#############################################################################

	//create and setup trace file;
	auto fp_toplevel = sc_core::sc_create_vcd_trace_file("architecture_test");

	sc_core::sc_trace(fp_toplevel,clk,"clock");
	sc_core::sc_trace(fp_toplevel,run,"run");
	sc_core::sc_trace(fp_toplevel,rst,"rst");
	sc_core::sc_trace(fp_toplevel,pause,"pause");
	sc_core::sc_trace(fp_toplevel,finish,"finish");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.m_currentAssembler, "assembler");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.address, "address");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.place, "place");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.cache_select, "cache_select");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.mmu_start, "mmu_start");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.mmu_ready, "mmu_ready");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.start, "vcgra_start");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.ready, "vcgra_ready");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.write_enable, "mmu_we");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::DATA_INPUT), "we_dataIn");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::DATA_OUTPUT), "we_dataOut");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::CONF_PE), "we_pe_cc");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::CONF_CC), "we_vc_cc");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.ack, "cache_ack");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::DATA_INPUT), "ack_dataIn");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::DATA_OUTPUT), "ack_dataOut");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::CONF_PE), "ack_pe_cc");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::CONF_CC), "ack_vc_cc");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.cache_place, "cache_place");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.data_value_out_stream, "data_cache_outstream");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.data_value_in_stream, "data_cache_instream");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.conf_cache_stream, "conf_cache_stream");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.dic_select_lines.first, "dic_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.dic_select_lines.second, "dic_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.doc_select_lines.first, "doc_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.doc_select_lines.second, "doc_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.pe_cc_select_lines.first, "pe_cc_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.pe_cc_select_lines.second, "pe_cc_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.ch_cc_select_lines.first, "ch_cc_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.ch_cc_select_lines.second, "ch_cc_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->vcgra.ch_config, "vch_configuration");
    sc_core::sc_trace(fp_toplevel, toplevel->vcgra.pe_config, "pe_configuration");


// 	{	//Lambda function to create port name in trace file
// 		uint8_t i = 0;
// 		auto concat = [](uint8_t i, const char* preamble){
// 			std::ostringstream s{""};
// 			s << preamble << static_cast<short>(i);
// 			return s.str();
// 		};
// 	}
//#############################################################################

    //Initialize configuration and data
    {
        uint16_t tdataValues[] = {10, 20, 50, 30, 16, 4, 64, 8};
        toplevel->mmu.write_shared_memory(0,tdataValues,sizeof(tdataValues));
        uint8_t tPeConfig[] = {0x12, 0x34, 0x86, 0x87, 0x88, 0x58, 0x88, 0x88};
        toplevel->mmu.write_shared_memory(50, tPeConfig, sizeof(tPeConfig));
        uint8_t tChConfig[] = {0x05, 0x39, 0x77, 0x01, 0xAB, 0x05, 0x6F, 0x05, 0xAF, 0x00};
        toplevel->mmu.write_shared_memory(40, tChConfig, sizeof(tChConfig));
    }
    
	//Run simulation
  	sc_core::sc_start(10000, sc_core::SC_NS);

    
//#ifdef DEBUG
    std::ofstream fp_dump{"simulation_dump.log", std::ios_base::out};
 	toplevel->dump(fp_dump);
    fp_dump << "Memory Dump" << std::endl;
    toplevel->mmu.dump_memory<uint16_t>(0, 30, sc_dt::SC_DEC, true, fp_dump);
    toplevel->mmu.dump_memory<uint16_t>(40, 60, sc_dt::SC_HEX, true, fp_dump);
    fp_dump.close();
//#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_toplevel);

	return EXIT_SUCCESS;
};
