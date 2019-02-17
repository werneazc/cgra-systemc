/*
 * TopLevel.cpp
 *
 *  Created on: Jan 20, 2018
 *      Author: andrewerner
 */
#include "TopLevel.h"

cgra::TopLevel::TopLevel(const sc_core::sc_module_name& nameA, 
                   assembler_type_t* assemblerPtrA,
                   uint64_t programSizeA) :
    sc_core::sc_module(nameA),
    mu("mu", assemblerPtrA, programSizeA)
{
    //bind ports of architecture to components
    //========================================
    //clock:
    vcgra.clk.bind(clk);
    data_in_cache.clk.bind(clk);
    data_out_cache.clk.bind(clk);
    pe_confCache.clk.bind(clk);
    ch_confCache.clk.bind(clk);
    mmu.clk.bind(clk);
    mu.clk.bind(clk);
    //reset
    vcgra.rst.bind(rst);
    mu.rst.bind(rst);
    //run
    mu.run.bind(run);
    //pause
    mu.pause.bind(pause);
    //finish
    mu.finish.bind(finish);
    
    //bind remaining component ports to internal signals
    //==================================================
    //VCGRA
    vcgra.start.bind(s_vcgra_start);
    vcgra.ready.bind(s_vcgra_ready);
    vcgra.ch_config.bind(s_ch_config);
    vcgra.pe_config.bind(s_pe_config);
    for(uint16_t idx = 0; 2 * cgra::cPeLevels.front() > idx; ++idx)
        vcgra.data_inputs.at(idx).bind(s_vcgra_data_inputs.at(idx));
    for(uint16_t idx = 0; cgra::cPeLevels.back() > idx; ++idx)
        vcgra.data_outputs.at(idx).bind(s_vcgra_data_outputs.at(idx));
    
    //PE configuration Cache
    pe_confCache.currentConfig.bind(s_pe_config);
    pe_confCache.dataInStream.bind(s_config_cache_stream);
    pe_confCache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE));
    pe_confCache.write.bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_PE));
    pe_confCache.slt_in.bind(s_pe_select_signals.first);
    pe_confCache.slt_out.bind(s_pe_select_signals.second);
    
    
    //VCH Configuration Cache
    ch_confCache.currentConfig.bind(s_ch_config);
    ch_confCache.dataInStream.bind(s_config_cache_stream);
    ch_confCache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC));
    ch_confCache.write.bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_CC));
    ch_confCache.slt_in.bind(s_ch_select_signals.first);
    ch_confCache.slt_out.bind(s_ch_select_signals.second);
    
    //Data Output Cache
    data_out_cache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    data_out_cache.load.bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    data_out_cache.dataOutStream.bind(s_data_out_stream);
    data_out_cache.update.bind(s_vcgra_ready);
    data_out_cache.slt_in.bind(s_doc_select_signals.first);
    data_out_cache.slt_out.bind(s_doc_select_signals.second);
    data_out_cache.slt_place.bind(s_cache_place);
    for(uint16_t idx = 0; cgra::cPeLevels.back() > idx; ++idx)
        data_out_cache.currentResults.at(idx).bind(s_vcgra_data_outputs.at(idx));
    
    //Data Input Cache
    data_in_cache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT));
    data_in_cache.write.bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_INPUT));
    data_in_cache.dataInStream.bind(s_data_in_stream);
    data_in_cache.slt_in.bind(s_dic_select_signals.first);
    data_in_cache.slt_out.bind(s_dic_select_signals.second);
    data_in_cache.slt_place.bind(s_cache_place);
    for(uint16_t idx = 0; 2 * cgra::cPeLevels.front() > idx; ++idx)
        data_in_cache.currentValues.at(idx).bind(s_vcgra_data_inputs.at(idx));
    
    //ManagementUnit
    mu.start.bind(s_vcgra_start);
    mu.ready.bind(s_vcgra_ready);
    mu.mmu_start.bind(s_mmu_start);
    mu.mmu_ready.bind(s_mmu_ready);
    mu.address.bind(s_address);
    mu.place.bind(s_place);
    mu.cache_select.bind(s_cache_select);
    mu.pe_cc_select_lines.first.bind(s_pe_select_signals.first);
    mu.pe_cc_select_lines.second.bind(s_pe_select_signals.second);
    mu.ch_cc_select_lines.first.bind(s_ch_select_signals.first);
    mu.ch_cc_select_lines.second.bind(s_ch_select_signals.second);
    mu.dic_select_lines.first.bind(s_dic_select_signals.first);
    mu.dic_select_lines.second.bind(s_dic_select_signals.second);
    mu.doc_select_lines.first.bind(s_doc_select_signals.first);
    mu.doc_select_lines.second.bind(s_doc_select_signals.second);
    
    //MMU
    mmu.start.bind(s_mmu_start);
    mmu.ready.bind(s_mmu_ready);
    mmu.address.bind(s_address);
    mmu.place.bind(s_place);
    mmu.conf_cache_stream.bind(s_config_cache_stream);
    mmu.cache_select.bind(s_cache_select);
    mmu.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::NONE));
    mmu.write_enable.bind(s_write_enables.at(MMU::CACHE_TYPE::NONE));
    mmu.data_value_in_stream.bind(s_data_out_stream);
    mmu.data_value_out_stream.bind(s_data_in_stream);
    mmu.cache_place.bind(s_cache_place);
    
    //WE DeMUX
    we_dmux.input.bind(s_write_enables.at(MMU::CACHE_TYPE::NONE));
    we_dmux.outputs.at(MMU::CACHE_TYPE::CONF_PE).bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_PE));
    we_dmux.outputs.at(MMU::CACHE_TYPE::CONF_CC).bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_CC));
    we_dmux.outputs.at(MMU::CACHE_TYPE::DATA_INPUT).bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_INPUT));
    we_dmux.outputs.at(MMU::CACHE_TYPE::DATA_OUTPUT).bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    we_dmux.select.bind(s_cache_select);
    
    //Ack MUX
    ack_mux.output.bind(s_acknowledges.at(MMU::CACHE_TYPE::NONE));
    ack_mux.inputs.at(MMU::CACHE_TYPE::CONF_PE).bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE));
    ack_mux.inputs.at(MMU::CACHE_TYPE::CONF_CC).bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC));
    ack_mux.inputs.at(MMU::CACHE_TYPE::DATA_INPUT).bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT));
    ack_mux.inputs.at(MMU::CACHE_TYPE::DATA_OUTPUT).bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    ack_mux.select.bind(s_cache_select);
    
    return;
}


void cgra::TopLevel::end_of_elaboration()
{
    finish.write(false);
    
    return;
}


void cgra::TopLevel::dump(::std::ostream& os) const
{
    os << name() << "\t\t" << kind() << std::endl;
    os << "Port signal values:\n";
    os << "-------------------" << std::endl;
    os << "Signal state \"run\":\t\t" << std::setw(3) << std::boolalpha << run.read() << std::endl;
    os << "Signal state \"pause\":\t\t" << std::setw(3) << std::boolalpha << pause.read() << std::endl;
    os << "Signal state \"rst\":\t\t" << std::setw(3) << std::boolalpha << rst.read() << std::endl;
    os << "Signal state \"finish\":\t\t" << std::setw(3) << std::boolalpha << finish.read() << std::endl;
    
    os << "Components dump:\n";
    os << "----------------" << std::endl;
    vcgra.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    pe_confCache.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    ch_confCache.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    data_in_cache.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    data_out_cache.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    mmu.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    mu.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    we_dmux.dump(os);
    os << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    ack_mux.dump(os);
    
    return;
}
