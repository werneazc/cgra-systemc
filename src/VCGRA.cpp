/*
 * VCGRA.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: andrewerner
 */

#include "VCGRA.h"
#include <numeric>

namespace cgra {

static constexpr uint32_t pe_unique_id_start_value{0};
//!< \brief Start value for unique IDs of ProcessingElements.

VCGRA::VCGRA(const sc_core::sc_module_name& nameA) :
    sc_core::sc_module(nameA)
{
    
    //Create Processing Elements of VCGRA
	m_pe_instances.init(cgra::cNumOfPe, pe_creator(cgra::pe_unique_id_start_value));
    //Create general VirtualChannel m_channel_instances
    /*
     * Every level in a VCGRA consists of a VirtualChannel. The first level
     * has a separate channel. Thus, the number of VirtualChannel instances 
     * is reduced by one.
     */
    m_channel_instances.init(cgra::cNumOfLevels - 1);
    
    //Connect configuration Selector and Demultiplexer
    m_input_channel_selector.config_input.bind(ch_config);
    m_input_channel_selector.config_parts.at(0).bind(s_input_ch_configuration);
    m_channel_selector.config_input.bind(ch_config);
    for(uint32_t i = 0; m_channel_selector.config_parts.size() > i; ++i) {
        m_channel_selector.config_parts.at(i).bind(s_ch_configurations.at(i));}
    m_sync_selector.config_input.bind(ch_config);
    m_sync_selector.config_parts.at(0).bind(s_sync_configuration);
    m_pe_config_demux.config_input.bind(pe_config);
    for(uint32_t i = 0; m_pe_config_demux.config_parts.size() > i; ++i) {
        m_pe_config_demux.config_parts.at(i).bind(s_pe_configurations.at(i));}
    
    //Connect Input Channel ports
    m_input_channel.clk.bind(clk);
    m_input_channel.rst.bind(rst);
    m_input_channel.conf.bind(s_input_ch_configuration);
    for(uint32_t i = 0; cgra::cInputChannel_NumOfInputs > i; ++i)
    {
        m_input_channel.valids.at(i).bind(start);
        m_input_channel.channel_inputs.at(i).bind(data_inputs.at(i));
    }
    for(uint32_t i = 0; cgra::cInputChannel_NumOfOutputs > i; ++i)
    {
        m_input_channel.enables.at(i).bind(s_enables.at(i));
        m_input_channel.channel_outputs.at(i).bind(s_pe_data_input_signals.at(i));
    }
    
    {
        //Connect PE ports
        uint32_t t_pe_idx{0};
        uint32_t t_en_idx{0};
        uint32_t t_in_idx{0};
        uint32_t t_out_idx{0};
        for(auto& pe : m_pe_instances)
        {
            pe.clk.bind(clk);
            pe.conf.bind(s_pe_configurations.at(t_pe_idx));
            pe.enable.at(0).bind(s_enables.at(t_en_idx++));
            pe.enable.at(1).bind(s_enables.at(t_en_idx++));
            pe.in1.bind(s_pe_data_input_signals.at(t_in_idx++));
            pe.in2.bind(s_pe_data_input_signals.at(t_in_idx++));
            pe.valid.bind(s_pe_valid_signals.at(t_pe_idx));
            if((cgra::cNumOfPe - cgra::cPeLevels.back()) > t_pe_idx) {
                pe.res.bind(s_pe_data_output_signals.at(t_pe_idx));
            }
            else {
                pe.res.bind(data_outputs.at(t_out_idx++));
            }
            
            ++t_pe_idx;
        }
    }
    
    {
        //Connect general VirtualChannels:
        uint32_t t_ch_num{0};
        uint32_t t_pe_level{0};
        for(auto& vch : m_channel_instances)
        {
            vch.clk.bind(clk);
            vch.rst.bind(rst);
            vch.conf.bind(s_ch_configurations.at(t_ch_num));
            
            
            for(uint16_t j = 0 , k = 0; cgra::cPeLevels.at(t_pe_level) > j; ++j, k += 2)
            {
                
                auto t_offset_in = std::accumulate(cgra::cPeLevels.begin(), 
                                                cgra::cPeLevels.begin() + t_pe_level, 
                                                0);
                auto t_offset_out = 2 * (std::accumulate(cgra::cPeLevels.begin(), 
                                                cgra::cPeLevels.begin() + t_pe_level + 1, 
                                                0));
                
                vch.channel_inputs.at(j).bind(s_pe_data_output_signals.at(
                    t_offset_in + j));
                vch.valids.at(j).bind(s_pe_valid_signals.at(
                    t_offset_in + j));
                vch.channel_outputs.at(k).bind(s_pe_data_input_signals.at(
                    t_offset_out + k));
                vch.channel_outputs.at(k + 1).bind(s_pe_data_input_signals.at(
                    t_offset_out + k + 1));
                vch.enables.at(k).bind(s_enables.at(
                    t_offset_out + k));
                vch.enables.at(k + 1).bind(s_enables.at(
                    t_offset_out + k + 1));
            }
            
            ++t_ch_num;
            ++t_pe_level;
        }
    }
    
    //Connect Synchronizer
    m_sync.clk.bind(clk);
    m_sync.conf.bind(s_sync_configuration);
    m_sync.ready.bind(ready);
    for(uint32_t i = 0; cgra::cPeLevels.back() > i; ++i)
    {
        /*
         * Array index starts with zero. Thus, number of PEs in
         * last level needs to be reduced by 1 to calculate index
         * of first valid signal for last PE level.
         */ 
        m_sync.valid_inputs.at(i).bind(s_pe_valid_signals.at((
            cgra::cNumOfPe - cgra::cPeLevels.back()) + i));
    }
    
}

void VCGRA::end_of_elaboration()
{
    ready.write(true);
    for(auto& out : data_outputs) {
        out.write(0);
    }
    
}


#ifdef MCPAT
	/**
	 * \brief Dump runtime statistics for McPAT simulation
	 * 
	 * \param os Define used outstream [default: std::cout]
	 */
void VCGRA::dumpMcpatStatistics(std::ostream& os) const 
{
    for(auto& pe : m_pe_instances)
    {
        pe.dumpMcpatStatistics(os);
    }

    m_input_channel.dumpMcpatStatistics(os);

    for(auto& ch : m_channel_instances)
    {
        ch.dumpMcpatStatistics(os);
    }

    m_sync.dumpMcpatStatistics(os);
}
#endif

void VCGRA::dump(::std::ostream & os) const
{
    os << this->name() << "\t\t\t" << this->kind() << std::endl;
    os << "Reset signal status:\t\t\t" << std::setw(3) << std::boolalpha << rst.read() << std::endl;
    os << "Start signal status:\t\t\t" << std::setw(3) << std::boolalpha << start.read() << std::endl;
    os << "Ready signal status:\t\t\t" << std::setw(3) << std::boolalpha << start.read() << std::endl;
    os << "PE configuration status:\t\t" << pe_config.read().to_string(sc_dt::SC_HEX) << std::endl;
    os << "CH configuration status:\t\t" << ch_config.read().to_string(sc_dt::SC_HEX) << std::endl;
   
    os << "Inputs:" << std::endl;
    for(auto& in : data_inputs) {
        os << in.name() << ":\t\t" << in.read().to_string(sc_dt::SC_DEC, false) << std::endl;
    }
    os << "Outputs:" << std::endl;
    for(auto& out : data_outputs) {
        os << out.name() << ":\t\t" << out.read().to_string(sc_dt::SC_DEC, false) << std::endl; 
    }
    
    os << "VCGRA component status:\n";
    os << "-----------------------" << std::endl;
    for(auto& pe : m_pe_instances) {
        pe.dump(os);
    }
    os << std::endl;
    m_input_channel.dump(os);
    os << std::endl;
    for(auto& ch : m_channel_instances) {
        ch.dump(os);
    }
    os << std::endl;
    m_sync.dump(os);
    os << std::endl;
    m_input_channel_selector.dump(os);
    os << std::endl;
    m_pe_config_demux.dump(os);
    os << std::endl;
    m_channel_selector.dump(os);
    os << std::endl;
    m_sync_selector.dump(os);
    os << std::endl;
    
}

} /* end namespace cgra */
