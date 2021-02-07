/*
 * vcgra_only.cpp
 *
 *  Created on: Dec 04, 2020
 *      Author: andrewerner
 */

#include "testbench_vcgra_only.hpp"
#include <systemc>
#include <type_traits>

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
        }
    }
    else {
        throw std::invalid_argument("Bounded arrays need to have the same sizes.");
    }
}

} // namespace

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

    // Close trace file
    sc_core::sc_close_vcd_trace_file(fp);

    return EXIT_SUCCESS;
}