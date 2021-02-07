/**
 * @file TopLevel_VcgraPreloaders.cpp
 * @author André Werner (andre.werner@b-tu.de)
 * @brief Implementation of top level for VCGRA with pre-fetchers
 * @version 0.1
 * @date 2021-01-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "TopLevel_VcgraPreloaders.hpp"

namespace cgra
{

TopLevel::TopLevel(const sc_core::sc_module_name &nameA) : sc_core::sc_module(nameA)
{

    // VCGRA signal port-port bindings
    vcgra.start.bind(start);
    vcgra.rst.bind(reset);
    vcgra.ready.bind(ready);
    vcgra.clk.bind(clk);
    for (size_t idx = 0; data_inputs.size() > idx; ++idx) {
        vcgra.data_inputs.at(idx).bind(data_inputs.at(idx));
    }
    for (size_t idx = 0; data_outputs.size() > idx; ++idx) {
        vcgra.data_outputs.at(idx).bind(data_outputs.at(idx));
    }
    vcgra.pe_config.bind(s_pe_config);
    vcgra.ch_config.bind(s_ch_config);

    // PE pre-fetcher port bindings
    pe_config_prefetcher.clk.bind(clk);
    pe_config_prefetcher.write.bind(pe_write_enable);
    pe_config_prefetcher.ack.bind(pe_ack);
    pe_config_prefetcher.slt_in.bind(pe_slct_in);
    pe_config_prefetcher.slt_out.bind(pe_slct_out);
    pe_config_prefetcher.dataInStream.bind(pe_input_stream);
    pe_config_prefetcher.currentConfig.bind(s_pe_config);

    // virtual channel pre-fetcher port bindings
    ch_config_prefetcher.clk.bind(clk);
    ch_config_prefetcher.write.bind(ch_write_enable);
    ch_config_prefetcher.ack.bind(ch_ack);
    ch_config_prefetcher.slt_in.bind(ch_slct_in);
    ch_config_prefetcher.slt_out.bind(ch_slct_out);
    ch_config_prefetcher.dataInStream.bind(ch_input_stream);
    ch_config_prefetcher.currentConfig.bind(s_ch_config);
}

void TopLevel::end_of_elaboration()
{
    s_pe_config.write(0);
    s_ch_config.write(0);
}

void TopLevel::dump(::std::ostream& os) const
{
    os << name() << ": function not implemented";
}

} // namespace cgra
