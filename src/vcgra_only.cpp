/*
 * vcgra_only.cpp
 *
 *  Created on: Dec 04, 2020
 *      Author: andrewerner
 */

#include "testbench_vcgra_only.hpp"
#include <systemc>
#include <type_traits>
#include <fstream>

#ifdef GSYSC
#include <gsysc.h>
#endif

namespace
{

const std::vector<std::vector<uint8_t>> cPeConfigs = {
    {0x33, 0x33, 0x01, 0x01, 0x00, 0x10, 0x00, 0x80},
    {0x38, 0x80, 0x01, 0x80, 0x00, 0x10, 0x00, 0x80},
};
//! Vector of PE configurations

const std::vector<std::vector<uint8_t>> cChConfigs = {
    {0x05, 0x39, 0x77, 0x01, 0xAB, 0x05, 0x7F, 0x05, 0xAF, 0xB0},
    {0x05, 0x26, 0xE4, 0x01, 0xAF, 0x05, 0x6F, 0x05, 0xAF, 0xB0},
};
//! Vector of channel configurations

} // namespace

#ifdef GSYSC
namespace cgra {
//gSysC renaming vector
//--------------------------------------------------

/*!
 * \var gsysc_renaming_strings
 *
 * \brief Stored pointer to strings for port and signal renaming
 *
 * \details
 * gSysC ports and signals store its corresponding name as a pointer to a char-array.
 * If a string name needs to be constructed dynamically, it needs to be stored somewhere during
 * simulation run. The strings are constructed on the heap and the pointer to the char-arrays
 * are stored within this vector. They are deleted at the end of the simulation run.
 */
std::vector<char*> gsysc_renaming_strings{};
}
#endif

auto sc_main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) -> int
{
    cgra::Testbench testbench("vcgra_testbench", "../demo/lena.pgm");
    cgra::VCGRA vcgra("VCGRA_Instance");
#ifdef GSYSC
    REG_MODULE(&vcgra, const_cast<char*>(vcgra.basename()), nullptr);
    REG_MODULE(&testbench, const_cast<char*>(testbench.basename()), nullptr);
#endif

    // Load configurations into testbench
    for (const auto &peConf : cPeConfigs) {
        testbench.appendPeConfiguration(peConf);
    }

    for (const auto &chConf : cChConfigs) {
        testbench.appendChConfiguration(chConf);
    }

#ifndef GSYSC
    // Signals
    sc_core::sc_clock s_clk("clk", 200, sc_core::SC_NS);
    sc_core::sc_signal<cgra::start_type_t> s_start("start");
    sc_core::sc_signal<cgra::reset_type_t> s_rst("rst");
    sc_core::sc_signal<cgra::ready_type_t> s_ready("ready");
    sc_core::sc_signal<cgra::pe_config_type_t> s_peConfig("peConfig");
    sc_core::sc_signal<cgra::ch_config_type_t> s_chConfig("chConfig");
    sc_core::sc_vector<sc_core::sc_signal<cgra::VCGRA::data_input_type_t>> s_inputs("inputs", vcgra.data_inputs.size());
    sc_core::sc_vector<sc_core::sc_signal<cgra::VCGRA::data_output_type_t>> s_outputs("outputs",
                                                                                      vcgra.data_outputs.size());
#else
    // Signals
    sc_clock s_clk("clk", 200, SC_NS);
    sc_signal<cgra::start_type_t> s_start("start");
    sc_signal<cgra::reset_type_t> s_rst("rst");
    sc_signal<cgra::ready_type_t> s_ready("ready");
    sc_signal<cgra::pe_config_type_t> s_peConfig("peConfig");
    sc_signal<cgra::ch_config_type_t> s_chConfig("chConfig");
    sc_vector<sc_signal<cgra::VCGRA::data_input_type_t>> s_inputs("inputs", vcgra.data_inputs.size());
    sc_vector<sc_signal<cgra::VCGRA::data_output_type_t>> s_outputs("outputs",
                                                                                vcgra.data_outputs.size());
#endif

    // Connect toplevel and testbench
    vcgra.clk.bind(s_clk);
    testbench.clk.bind(s_clk);
    vcgra.pe_config.bind(s_peConfig);
    testbench.pe_config.bind(s_peConfig);
    vcgra.ch_config.bind(s_chConfig);
    testbench.ch_config.bind(s_chConfig);
    vcgra.start.bind(s_start);
    testbench.start.bind(s_start);
    vcgra.rst.bind(s_rst);
    testbench.rst.bind(s_rst);
    testbench.ready.bind(s_ready);
    vcgra.ready.bind(s_ready);

#ifdef GSYSC
    RENAME_SIGNAL(&s_clk,
        (cgra::create_name<std::string,uint32_t>("s_clk_", 0)));
    RENAME_SIGNAL(&s_peConfig,
        (cgra::create_name<std::string,uint32_t>("s_peConfig_", 0)));
    RENAME_SIGNAL(&s_chConfig,
        (cgra::create_name<std::string,uint32_t>("s_chConfig_", 0)));
    RENAME_SIGNAL(&s_start,
        (cgra::create_name<std::string,uint32_t>("s_start_", 0)));
    RENAME_SIGNAL(&s_rst,
        (cgra::create_name<std::string,uint32_t>("s_rst_", 0)));
    RENAME_SIGNAL(&s_ready,
        (cgra::create_name<std::string,uint32_t>("s_ready_", 0)));

    REG_PORT(&vcgra.clk,           &vcgra,     &s_clk);
    REG_PORT(&testbench.clk,       &testbench, &s_clk);
    REG_PORT(&vcgra.pe_config,     &vcgra,     &s_peConfig);
    REG_PORT(&testbench.pe_config, &testbench, &s_peConfig);
    REG_PORT(&vcgra.ch_config,     &vcgra,     &s_chConfig);
    REG_PORT(&testbench.ch_config, &testbench, &s_chConfig);
    REG_PORT(&vcgra.start,         &vcgra,     &s_start);
    REG_PORT(&testbench.start,     &testbench, &s_start);
    REG_PORT(&vcgra.rst,           &vcgra,     &s_rst);
    REG_PORT(&testbench.rst,       &testbench, &s_rst);
    REG_PORT(&testbench.ready,     &testbench, &s_ready);
    REG_PORT(&vcgra.ready,         &vcgra,     &s_ready);
#endif

    for (std::size_t iter = 0; iter < vcgra.data_inputs.size(); ++iter) {
        vcgra.data_inputs.at(iter).bind(s_inputs.at(iter));
        testbench.data_inputs.at(iter).bind(s_inputs.at(iter));
#ifdef GSYSC
        REG_PORT(&vcgra.data_inputs.at(iter), &vcgra, &s_inputs.at(iter));
        REG_PORT(&testbench.data_inputs.at(iter), &testbench, &s_inputs.at(iter));
#endif
    }

    for (std::size_t iter = 0; iter < vcgra.data_outputs.size(); ++iter) {
        vcgra.data_outputs.at(iter).bind(s_outputs.at(iter));
        testbench.data_outputs.at(iter).bind(s_outputs.at(iter));
#ifdef GSYSC
        REG_PORT(&vcgra.data_outputs.at(iter), &vcgra, &s_outputs.at(iter));
        REG_PORT(&testbench.data_outputs.at(iter), &testbench, &s_outputs.at(iter));
#endif
    }

    // create and setup trace file;
    auto * file_ptr = sc_core::sc_create_vcd_trace_file("vcgra_only");

    sc_core::sc_trace(file_ptr, s_clk, s_clk.basename());
    sc_core::sc_trace(file_ptr, s_peConfig, s_peConfig.basename());
    sc_core::sc_trace(file_ptr, s_chConfig, s_chConfig.basename());
    sc_core::sc_trace(file_ptr, s_rst, s_rst.basename());
    sc_core::sc_trace(file_ptr, s_ready, s_ready.basename());
    sc_core::sc_trace(file_ptr, s_start, s_start.basename());
    for (const auto &in : s_inputs) {
        sc_core::sc_trace(file_ptr, in, in.basename());
    }
    for (const auto &out : s_outputs) {
        sc_core::sc_trace(file_ptr, out, out.basename());
    }

    // Start simulation
#ifndef GSYSC
    sc_core::sc_start();
#else
    sc_start();
#endif

    // Write result image
    testbench.writeResultImagetoFile("./vcgra_only_result_image.pgm");

#ifdef MCPAT
    std::ofstream fp_mcpatStats{"mcpat_stats_vcgra_only.log", std::ios_base::out};
    vcgra.dumpMcpatStatistics(fp_mcpatStats);
    fp_mcpatStats.close();
#endif
    // Close trace file
    sc_core::sc_close_vcd_trace_file(file_ptr);

    return EXIT_SUCCESS;
}
