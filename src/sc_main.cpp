/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include "MU_TB.h"

int sc_main(int argc, char* arcv[])
{
	//include TB
	auto tb_mu = new cgra::TB_MU{"MU_TestBench"};

//#############################################################################

	//instantiate modules

	std::array<cgra::CommandInterpreter::assembler_type_t, 10> assembly {
		"0x00000000", //NOOP
		"0x00802005", //LOADD
		"0x01001FC6", //LOADDA
		"0x00400087", //STORED
		"0x01403FC8", //STOREDA
		"0x10003FC9", //LOADPC
		"0x11403FCA", //LOADCC
		"0x0000000B", //START
		"0x00000004", //WAIT_READY
		"0x0000000C"  //FINISH
	};

	auto mu = new cgra::ManagementUnit{"MU", assembly.data(), assembly.size()};

//#############################################################################

	//Type definitions
	typedef std::pair<sc_core::sc_signal<cgra::data_input_cache_type_t::select_lines_type_t>,
			sc_core::sc_signal<cgra::data_input_cache_type_t::select_lines_type_t>> dic_select_type_t;
	typedef std::pair<sc_core::sc_signal<cgra::data_output_cache_type_t::select_lines_type_t>,
			sc_core::sc_signal<cgra::data_output_cache_type_t::select_lines_type_t>> doc_select_type_t;
	typedef std::pair<sc_core::sc_signal<cgra::pe_config_cache_type_t::select_type_t>,
			sc_core::sc_signal<cgra::pe_config_cache_type_t::select_type_t>> pe_cc_select_type_t;
	typedef std::pair<sc_core::sc_signal<cgra::ch_config_cache_type_t::select_type_t>,
			sc_core::sc_signal<cgra::ch_config_cache_type_t::select_type_t>> ch_cc_select_type_t;


	//signals
	sc_core::sc_clock clk{"clk", 4, sc_core::SC_NS};
	sc_core::sc_signal<cgra::ManagementUnit::run_type_t> run{"run"};
	sc_core::sc_signal<cgra::ManagementUnit::finish_type_t> finish{"finish"};
	sc_core::sc_signal<cgra::ManagementUnit::pause_type_t> pause{"pause"};
	sc_core::sc_signal<cgra::reset_type_t> rst{"reset"};
	sc_core::sc_signal<cgra::ready_type_t> ready{"ready"};
	sc_core::sc_signal<cgra::start_type_t> start{"start"};
	sc_core::sc_signal<cgra::CommandInterpreter::address_type_t> address{"Address"};
	sc_core::sc_signal<cgra::CommandInterpreter::place_type_t> place{"Place"};
	sc_core::sc_signal<cgra::MMU::cache_select_type_t> cache_select{"Cache_Select"};
	sc_core::sc_signal<cgra::MMU::ready_type_t> mmu_ready{"MMU_Ready"};
	sc_core::sc_signal<cgra::MMU::start_type_t> mmu_start{"MMU_start"};
	dic_select_type_t dic_select_lines;
	doc_select_type_t doc_select_lines;
	pe_cc_select_type_t pe_cc_select_lines;
	ch_cc_select_type_t ch_cc_select_lines;


//#############################################################################

	//Port bindings

	//clock
	mu->clk.bind(clk);

	//control signals
	tb_mu->start.bind(start);
	tb_mu->ready.bind(ready);
	tb_mu->rst.bind(rst);
	tb_mu->finish.bind(finish);
	tb_mu->pause.bind(pause);
	tb_mu->run.bind(run);
	tb_mu->mmu_ready.bind(mmu_ready);
	tb_mu->mmu_start.bind(mmu_start);
	mu->start.bind(start);
	mu->ready.bind(ready);
	mu->rst.bind(rst);
	mu->finish.bind(finish);
	mu->pause.bind(pause);
	mu->run.bind(run);
	mu->mmu_ready.bind(mmu_ready);
	mu->mmu_start.bind(mmu_start);

	//data signals
	tb_mu->address.bind(address);
	tb_mu->place.bind(place);
	tb_mu->cache_select.bind(cache_select);
	tb_mu->dic_select_lines.first.bind(dic_select_lines.first);
	tb_mu->dic_select_lines.second.bind(dic_select_lines.second);
	tb_mu->doc_select_lines.first.bind(doc_select_lines.first);
	tb_mu->doc_select_lines.second.bind(doc_select_lines.second);
	tb_mu->pe_cc_select_lines.first.bind(pe_cc_select_lines.first);
	tb_mu->pe_cc_select_lines.second.bind(pe_cc_select_lines.second);
	tb_mu->ch_cc_select_lines.first.bind(ch_cc_select_lines.first);
	tb_mu->ch_cc_select_lines.second.bind(ch_cc_select_lines.second);
	mu->address.bind(address);
	mu->place.bind(place);
	mu->cache_select.bind(cache_select);
	mu->dic_select_lines.first.bind(dic_select_lines.first);
	mu->dic_select_lines.second.bind(dic_select_lines.second);
	mu->doc_select_lines.first.bind(doc_select_lines.first);
	mu->doc_select_lines.second.bind(doc_select_lines.second);
	mu->pe_cc_select_lines.first.bind(pe_cc_select_lines.first);
	mu->pe_cc_select_lines.second.bind(pe_cc_select_lines.second);
	mu->ch_cc_select_lines.first.bind(ch_cc_select_lines.first);
	mu->ch_cc_select_lines.second.bind(ch_cc_select_lines.second);

//#############################################################################

	//create and setup trace file;
	auto fp_mu = sc_core::sc_create_vcd_trace_file("mu_test");

	sc_core::sc_trace(fp_mu,clk,"clock");
	sc_core::sc_trace(fp_mu,start,"start");
	sc_core::sc_trace(fp_mu,rst,"rst");
	sc_core::sc_trace(fp_mu,ready,"ready");
	sc_core::sc_trace(fp_mu,finish,"finish");
	sc_core::sc_trace(fp_mu,run,"run");
	sc_core::sc_trace(fp_mu,mmu_ready,"mmu_ready");
	sc_core::sc_trace(fp_mu,mmu_start,"mmu_start");
	sc_core::sc_trace(fp_mu,pause,"pause");
	sc_core::sc_trace(fp_mu,address,"address");
	sc_core::sc_trace(fp_mu,place,"place");
	sc_core::sc_trace(fp_mu,cache_select,"cache_select");
	sc_core::sc_trace(fp_mu,dic_select_lines.first,"dic_select_in");
	sc_core::sc_trace(fp_mu,dic_select_lines.second,"dic_select_out");
	sc_core::sc_trace(fp_mu,doc_select_lines.first,"doc_select_in");
	sc_core::sc_trace(fp_mu,doc_select_lines.second,"doc_select_out");
	sc_core::sc_trace(fp_mu,pe_cc_select_lines.first,"pe_cc_select_in");
	sc_core::sc_trace(fp_mu,pe_cc_select_lines.second,"pe_cc_select_out");
	sc_core::sc_trace(fp_mu,ch_cc_select_lines.first,"ch_cc_select_in");
	sc_core::sc_trace(fp_mu,ch_cc_select_lines.second,"ch_cc_select_out");
	sc_core::sc_trace(fp_mu, mu->m_currentCommand, "currentCommand");


//#############################################################################

	//Run simulation
	sc_core::sc_start(1,sc_core::SC_MS);

#ifdef DEBUG
	mu->dump();
#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_mu);

	return EXIT_SUCCESS;
};
