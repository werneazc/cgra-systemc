/*
 * TestBenchCC.cpp
 *
 *  Created on: Aug 17, 2017
 *      Author: andrewerner
 */

#include "TestBenchCC.h"

namespace cgra {

TestBench_CC::TestBench_CC(const sc_core::sc_module_name& nameA) :
    sc_core::sc_module(nameA){

    SC_THREAD(stimuli);
    sensitive << clk.pos();
}

void TestBench_CC::dump(std::ostream& os) const {

    os << name() << ":\t" << kind() << std::endl;
    os << "================================================================================\n";
    os << clk.basename() << ":\t\t\t" << (clk.read() ? "True" : "False") << "\n";
    os << s_write_enable.basename() << ":\t\t" << (s_write_enable.read() ? "True" : "False") << "\n";
    os << r_ack.basename() << ":\t\t" << (r_ack.read() ? "True" : "False") << "\n";
    os << s_slt_in.basename() << ":\t" << s_slt_in.read().to_string() << "\n";
    os << s_slt_out.basename() << ":\t" << s_slt_out.read().to_string() << "\n";
    os << r_current_configuration.basename() << ":\t" << std::hex << r_current_configuration.read().to_uint() << "\n";
    os << s_stream_data_out.basename() << ":\t" << std::hex << s_stream_data_out.read().to_uint() << std::endl;
}

void TestBench_CC::stimuli() {

    wait(2);
    s_write_enable.write(true);

    wait(r_ack.posedge_event());
    s_write_enable.write(false);

    wait(r_ack.negedge_event());
    s_stream_data_out.write(0xfe);

    wait();
    s_write_enable.write(true);

    wait(r_ack.posedge_event());
    s_write_enable.write(false);

    wait(r_ack.negedge_event());
    s_slt_in.write("01");
    s_slt_out.write("00");

    wait(r_current_configuration.value_changed_event());
    dump();
    s_slt_in.write("00");
    s_write_enable.write(true);
    wait();
    s_write_enable.write(false);
}

void TestBench_CC::end_of_elaboration() {

    s_slt_in.write("00");
    s_slt_out.write("01");

    s_stream_data_out.write(0xaf);
    s_write_enable.write(false);
}

} /* namespace cgra */

