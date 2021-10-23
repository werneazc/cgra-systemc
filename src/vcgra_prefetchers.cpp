/**
 * @file vcgra_prefetchers.cpp
 * @author André Werner (andre.werner@b-tu.de)
 * @brief Small architecture simulation of a VCGRA and its pre-fetchers
 * @version 0.1
 * @date 2021-01-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "Testbench_VcgraPreloaders.hpp"
#include <systemc>
#include <type_traits>
#include <fstream>

#ifdef GSYSC
#include <gsysc.h>
#endif

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
                RENAME_SIGNAL(&p_signals.at(iter),
                    (cgra::create_name<std::string,uint32_t>("p_signal_", iter)));
            #endif
        }
    }
    else {
        throw std::invalid_argument("Bounded arrays need to have the same sizes.");
    }
}

} // namespace

int sc_main(int argc, char **argv)
{
    cgra::Testbench testbench("toplevel_testbench", "../demo/lena.pgm");
    cgra::TopLevel toplevel("toplevel_instance");

    // Load configurations into testbench
    for (auto &peConf : cPeConfigs) {
        testbench.appendPeConfiguration(peConf);
    }

    for (auto &chConf : cChConfigs) {
        testbench.appendChConfiguration(chConf);
    }

    // Signals
    sc_core::sc_clock s_clk("clk", 200, sc_core::SC_NS);

#ifndef GSYSC
    sc_core::sc_signal<cgra::start_type_t> s_start("start");
    sc_core::sc_signal<cgra::reset_type_t> s_rst("rst");
    sc_core::sc_signal<cgra::ready_type_t> s_ready("ready");
    sc_core::sc_vector<sc_core::sc_signal<cgra::TopLevel::data_input_type_t>> s_inputs("inputs",
                                                                                       toplevel.data_inputs.size());
    sc_core::sc_vector<sc_core::sc_signal<cgra::TopLevel::data_output_type_t>> s_outputs("outputs",
                                                                                         toplevel.data_outputs.size());
    sc_core::sc_signal<cgra::TopLevel::ch_stream_type_t> s_ch_input_stream("ch_input_stream");
    sc_core::sc_signal<cgra::TopLevel::write_enable_type_t> s_ch_write_enable("ch_wren");
    sc_core::sc_signal<cgra::TopLevel::ack_type_t> s_ch_ack("ch_ack");
    sc_core::sc_signal<cgra::TopLevel::ch_select_type_t> s_ch_slct_in("ch_select_in");
    sc_core::sc_signal<cgra::TopLevel::ch_select_type_t> s_ch_slct_out("ch_select_out");
    sc_core::sc_signal<cgra::TopLevel::pe_stream_type_t> s_pe_input_stream("pe_input_stream");
    sc_core::sc_signal<cgra::TopLevel::write_enable_type_t> s_pe_write_enable("pe_wren");
    sc_core::sc_signal<cgra::TopLevel::ack_type_t> s_pe_ack("pe_ack");
    sc_core::sc_signal<cgra::TopLevel::ch_select_type_t> s_pe_slct_in("pe_select_in");
    sc_core::sc_signal<cgra::TopLevel::ch_select_type_t> s_pe_slct_out("pe_select_out");
#else
    sc_signal<cgra::start_type_t> s_start("start");
    sc_signal<cgra::reset_type_t> s_rst("rst");
    sc_signal<cgra::ready_type_t> s_ready("ready");
    sc_vector<sc_signal<cgra::TopLevel::data_input_type_t>> s_inputs("inputs",
                                                                        toplevel.data_inputs.size());
    sc_vector<sc_signal<cgra::TopLevel::data_output_type_t>> s_outputs("outputs",
                                                                        toplevel.data_outputs.size());
    sc_signal<cgra::TopLevel::ch_stream_type_t> s_ch_input_stream("ch_input_stream");
    sc_signal<cgra::TopLevel::write_enable_type_t> s_ch_write_enable("ch_wren");
    sc_signal<cgra::TopLevel::ack_type_t> s_ch_ack("ch_ack");
    sc_signal<cgra::TopLevel::ch_select_type_t> s_ch_slct_in("ch_select_in");
    sc_signal<cgra::TopLevel::ch_select_type_t> s_ch_slct_out("ch_select_out");
    sc_signal<cgra::TopLevel::pe_stream_type_t> s_pe_input_stream("pe_input_stream");
    sc_signal<cgra::TopLevel::write_enable_type_t> s_pe_write_enable("pe_wren");
    sc_signal<cgra::TopLevel::ack_type_t> s_pe_ack("pe_ack");
    sc_signal<cgra::TopLevel::ch_select_type_t> s_pe_slct_in("pe_select_in");
    sc_signal<cgra::TopLevel::ch_select_type_t> s_pe_slct_out("pe_select_out");
#endif

    // Connect toplevel and testbench
    toplevel.clk.bind(s_clk);
    testbench.clk.bind(s_clk);
    toplevel.start.bind(s_start);
    testbench.start.bind(s_start);
    toplevel.reset.bind(s_rst);
    testbench.reset.bind(s_rst);
    toplevel.ready.bind(s_ready);
    testbench.ready.bind(s_ready);
    toplevel.pe_write_enable.bind(s_pe_write_enable);
    testbench.pe_write_enable.bind(s_pe_write_enable);
    toplevel.pe_ack.bind(s_pe_ack);
    testbench.pe_ack.bind(s_pe_ack);
    toplevel.pe_slct_in.bind(s_pe_slct_in);
    testbench.pe_slct_in.bind(s_pe_slct_in);
    toplevel.pe_slct_out.bind(s_pe_slct_out);
    testbench.pe_slct_out.bind(s_pe_slct_out);
    toplevel.pe_input_stream.bind(s_pe_input_stream);
    testbench.pe_input_stream.bind(s_pe_input_stream);
    toplevel.ch_write_enable.bind(s_ch_write_enable);
    testbench.ch_write_enable.bind(s_ch_write_enable);
    toplevel.ch_ack.bind(s_ch_ack);
    testbench.ch_ack.bind(s_ch_ack);
    toplevel.ch_slct_in.bind(s_ch_slct_in);
    testbench.ch_slct_in.bind(s_ch_slct_in);
    toplevel.ch_slct_out.bind(s_ch_slct_out);
    testbench.ch_slct_out.bind(s_ch_slct_out);
    toplevel.ch_input_stream.bind(s_ch_input_stream);
    testbench.ch_input_stream.bind(s_ch_input_stream);

#ifdef GSYSC
    RENAME_SIGNAL(&s_clk,
        (cgra::create_name<std::string,uint32_t>("s_clk_", 0)));
    RENAME_SIGNAL(&s_start,
        (cgra::create_name<std::string,uint32_t>("s_start_", 0)));
    RENAME_SIGNAL(&s_rst,
        (cgra::create_name<std::string,uint32_t>("s_rst_", 0)));
    RENAME_SIGNAL(&s_ready,
        (cgra::create_name<std::string,uint32_t>("s_ready_", 0)));
    RENAME_SIGNAL(&s_pe_write_enable,
        (cgra::create_name<std::string,uint32_t>("s_pe_write_enable_", 0)));
    RENAME_SIGNAL(&s_pe_ack,
        (cgra::create_name<std::string,uint32_t>("s_pe_ack_", 0)));
    RENAME_SIGNAL(&s_pe_slct_in,
        (cgra::create_name<std::string,uint32_t>("s_pe_slct_in_", 0)));
    RENAME_SIGNAL(&s_pe_slct_out,
        (cgra::create_name<std::string,uint32_t>("s_pe_slct_out_", 0)));
    RENAME_SIGNAL(&s_pe_input_stream,
        (cgra::create_name<std::string,uint32_t>("s_pe_input_stream_", 0)));
    RENAME_SIGNAL(&s_ch_write_enable,
        (cgra::create_name<std::string,uint32_t>("s_ch_write_enable_", 0)));
    RENAME_SIGNAL(&s_ch_ack,
        (cgra::create_name<std::string,uint32_t>("s_ch_ack_", 0)));
    RENAME_SIGNAL(&s_ch_slct_in,
        (cgra::create_name<std::string,uint32_t>("s_ch_slct_in_", 0)));
    RENAME_SIGNAL(&s_ch_slct_out,
        (cgra::create_name<std::string,uint32_t>("s_ch_slct_out_", 0)));
    RENAME_SIGNAL(&s_ch_input_stream,
        (cgra::create_name<std::string,uint32_t>("s_ch_input_stream_", 0)));

    REG_PORT(&toplevel.clk,             &toplevel,     &s_clk);
    REG_PORT(&toplevel.start,           &toplevel,     &s_start);
    REG_PORT(&toplevel.reset,           &toplevel,     &s_rst);
    REG_PORT(&toplevel.ready,           &toplevel,     &s_ready);
    REG_PORT(&toplevel.pe_write_enable, &toplevel,     &s_pe_write_enable);
    REG_PORT(&toplevel.pe_ack,          &toplevel,     &s_pe_ack);
    REG_PORT(&toplevel.pe_slct_in,      &toplevel,     &s_pe_slct_in);
    REG_PORT(&toplevel.pe_slct_out,     &toplevel,     &s_pe_slct_out);
    REG_PORT(&toplevel.pe_input_stream, &toplevel,     &s_pe_input_stream);
    REG_PORT(&toplevel.ch_write_enable, &toplevel,     &s_ch_write_enable);
    REG_PORT(&toplevel.ch_ack,          &toplevel,     &s_ch_ack);
    REG_PORT(&toplevel.ch_slct_in,      &toplevel,     &s_ch_slct_in);
    REG_PORT(&toplevel.ch_slct_out,     &toplevel,     &s_ch_slct_out);
    REG_PORT(&toplevel.ch_input_stream, &toplevel,     &s_ch_input_stream);
    REG_PORT(&testbench.clk,             &testbench,     &s_clk);
    REG_PORT(&testbench.start,           &testbench,     &s_start);
    REG_PORT(&testbench.reset,           &testbench,     &s_rst);
    REG_PORT(&testbench.ready,           &testbench,     &s_ready);
    REG_PORT(&testbench.pe_write_enable, &testbench,     &s_pe_write_enable);
    REG_PORT(&testbench.pe_ack,          &testbench,     &s_pe_ack);
    REG_PORT(&testbench.pe_slct_in,      &testbench,     &s_pe_slct_in);
    REG_PORT(&testbench.pe_slct_out,     &testbench,     &s_pe_slct_out);
    REG_PORT(&testbench.pe_input_stream, &testbench,     &s_pe_input_stream);
    REG_PORT(&testbench.ch_write_enable, &testbench,     &s_ch_write_enable);
    REG_PORT(&testbench.ch_ack,          &testbench,     &s_ch_ack);
    REG_PORT(&testbench.ch_slct_in,      &testbench,     &s_ch_slct_in);
    REG_PORT(&testbench.ch_slct_out,     &testbench,     &s_ch_slct_out);
    REG_PORT(&testbench.ch_input_stream, &testbench,     &s_ch_input_stream);
#endif

    try {
        array_bind(toplevel.data_inputs, testbench.data_inputs, s_inputs);
        array_bind(testbench.data_outputs, toplevel.data_outputs, s_outputs);
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
    auto fp = sc_core::sc_create_vcd_trace_file("vcgra_prefetchers");

    sc_core::sc_trace(fp, s_clk, s_clk.basename());
    sc_core::sc_trace(fp, s_rst, s_rst.basename());
    sc_core::sc_trace(fp, s_ready, s_ready.basename());
    sc_core::sc_trace(fp, s_start, s_start.basename());
    sc_core::sc_trace(fp, s_pe_ack, s_pe_ack.basename());
    sc_core::sc_trace(fp, s_pe_write_enable, s_pe_write_enable.basename());
    sc_core::sc_trace(fp, s_pe_slct_in, s_pe_slct_in.basename());
    sc_core::sc_trace(fp, s_pe_slct_out, s_pe_slct_out.basename());
    sc_core::sc_trace(fp, s_pe_input_stream, s_pe_input_stream.basename());
    sc_core::sc_trace(fp, s_ch_ack, s_ch_ack.basename());
    sc_core::sc_trace(fp, s_ch_write_enable, s_ch_write_enable.basename());
    sc_core::sc_trace(fp, s_ch_slct_in, s_ch_slct_in.basename());
    sc_core::sc_trace(fp, s_ch_slct_out, s_ch_slct_out.basename());
    sc_core::sc_trace(fp, s_ch_input_stream, s_ch_input_stream.basename());
    // sc_core::sc_trace(fp, toplevel.s_pe_config, toplevel.s_pe_config.basename());
    // sc_core::sc_trace(fp, toplevel.s_ch_config, toplevel.s_ch_config.basename());
    for (auto &in : s_inputs) {
        sc_core::sc_trace(fp, in, in.basename());
    }
    for (auto &out : s_outputs) {
        sc_core::sc_trace(fp, out, out.basename());
    }

    // Run simulation
#ifndef GSYSC
    sc_core::sc_start();
#else
    sc_start();
#endif

    // Write result image
    testbench.writeResultImagetoFile("./vcgra_prefetchers_result_image.pgm");

#ifdef MCPAT
    std::ofstream fp_mcpatStats{"mcpat_stats_vcgra_prefetchers.log", std::ios_base::out};
    toplevel.vcgra.dumpMcpatStatistics(fp_mcpatStats);
    toplevel.pe_config_prefetcher.dumpMcpatStatistics(fp_mcpatStats);
    toplevel.ch_config_prefetcher.dumpMcpatStatistics(fp_mcpatStats);
    fp_mcpatStats.close();
#endif

    // Close trace file
    sc_core::sc_close_vcd_trace_file(fp);

    return EXIT_SUCCESS;
}
