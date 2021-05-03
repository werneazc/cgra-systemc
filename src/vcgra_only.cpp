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
#include "utils.hpp"
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

/**
 * @brief Bind signals of vectors
 *
 * @tparam T Type of input ports
 * @tparam G Type of output ports
 * @tparam U Type of signal to bind
 * @param p_inputs Reference to array of input ports
 * @param p_outputs Reference to array of output ports
 * @param p_signals Reference to array of signals
 */
template <typename T, typename G, typename U> void array_bind(T &p_inputs, G &p_outputs, U &p_signals)
{

    if (p_inputs.size() == p_outputs.size() && p_signals.size() == p_inputs.size()) {
        for (std::size_t iter = 0; iter < p_inputs.size(); ++iter) {
            p_inputs.at(iter).bind(p_signals.at(iter));
            p_outputs.at(iter).bind(p_signals.at(iter));

            #ifdef GSYSC
            RENAME_SIGNAL(p_signals.at(iter),
                (cgra::create_name<std::string,uint32_t>("p_signals_", iter)));
            REG_PORT(p_inputs.at(iter), 
                     p_inputs, 
                     p_signals.at(iter));
            REG_PORT(p_outputs.at(iter), 
                     p_outputs, 
                     p_signals.at(iter));
            #endif
        }
    }
    else {
        throw std::invalid_argument("Bounded arrays need to have the same sizes.");
    }
}

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

int sc_main(int argc, char **argv)
{
    cgra::Testbench testbench("vcgra_testbench", "../demo/lena.pgm");
    cgra::VCGRA vcgra("VCGRA_Instance");

    // Load configurations into testbench
    for (auto &peConf : cPeConfigs) {
        testbench.appendPeConfiguration(peConf);
    }

    for (auto &chConf : cChConfigs) {
        testbench.appendChConfiguration(chConf);
    }

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
            RENAME_SIGNAL(s_clk,
                (cgra::create_name<std::string,uint32_t>("p_clk_", 0)));
            RENAME_SIGNAL(s_peConfig,
                (cgra::create_name<std::string,uint32_t>("s_peConfig_", 0)));
            RENAME_SIGNAL(s_chConfig,
                (cgra::create_name<std::string,uint32_t>("s_chConfig_", 0)));
            RENAME_SIGNAL(s_start,
                (cgra::create_name<std::string,uint32_t>("s_start_", 0)));
            RENAME_SIGNAL(s_rst,
                (cgra::create_name<std::string,uint32_t>("s_rst_", 0)));
            RENAME_SIGNAL(s_ready,
                (cgra::create_name<std::string,uint32_t>("s_ready_", 0)));

            REG_PORT(vcgra.clk,           vcgra,     s_clk);
            REG_PORT(testbench.clk,       testbench, s_clk);
            REG_PORT(vcgra.pe_config,     vcgra,     s_peConfig);
            REG_PORT(testbench.pe_config, testbench, s_peConfig);
            REG_PORT(vcgra.ch_config,     vcgra,     s_chConfig);
            REG_PORT(testbench.ch_config, testbench, s_chConfig);
            REG_PORT(vcgra.start,         vcgra,     s_start);
            REG_PORT(testbench.start,     testbench, s_start);
            REG_PORT(vcgra.rst,           vcgra,     s_rst);
            REG_PORT(testbench.rst,       testbench, s_rst);
            REG_PORT(testbench.ready,     testbench, s_ready);
            REG_PORT(vcgra.ready,         vcgra,     s_ready);
    #endif

    try {
        array_bind(vcgra.data_inputs, testbench.data_inputs, s_inputs);
        array_bind(testbench.data_outputs, vcgra.data_outputs, s_outputs);
    }
    catch (const std::invalid_argument &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown exception\n";
        return EXIT_FAILURE;
    }

    // create and setup trace file;
    auto fp = sc_core::sc_create_vcd_trace_file("vcgra_only");

    sc_core::sc_trace(fp, s_clk, s_clk.basename());
    sc_core::sc_trace(fp, s_peConfig, s_peConfig.basename());
    sc_core::sc_trace(fp, s_chConfig, s_chConfig.basename());
    sc_core::sc_trace(fp, s_rst, s_rst.basename());
    sc_core::sc_trace(fp, s_ready, s_ready.basename());
    sc_core::sc_trace(fp, s_start, s_start.basename());
    for (auto &in : s_inputs) {
        sc_core::sc_trace(fp, in, in.basename());
    }
    for (auto &out : s_outputs) {
        sc_core::sc_trace(fp, out, out.basename());
    }

    // Start simulation
    sc_core::sc_start();

    // Write result image
    testbench.writeResultImagetoFile("./vcgra_only_result_image.pgm");

#ifdef MCPAT
    std::ofstream fp_mcpatStats{"mcpat_stats_vcgra_only.log", std::ios_base::out};
    vcgra.dumpMcpatStatistics(fp_mcpatStats);
    fp_mcpatStats.close();
#endif
    // Close trace file
    sc_core::sc_close_vcd_trace_file(fp);

    return EXIT_SUCCESS;
}