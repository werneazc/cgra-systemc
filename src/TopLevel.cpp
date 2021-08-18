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
    #ifdef GSYSC
        RENAME_SIGNAL(&clk,
            (cgra::create_name<std::string,uint32_t>("clk_", 0)));
        RENAME_SIGNAL(&rst,
            (cgra::create_name<std::string,uint32_t>("rst_", 0)));
        RENAME_SIGNAL(&run,
            (cgra::create_name<std::string,uint32_t>("run_", 0)));
        RENAME_SIGNAL(&pause,
            (cgra::create_name<std::string,uint32_t>("pause_", 0)));
        RENAME_SIGNAL(&finish,
            (cgra::create_name<std::string,uint32_t>("finish_", 0)));

        REG_PORT(&vcgra.clk,          &vcgra,          &clk);
        REG_PORT(&data_in_cache.clk,  &data_in_cache,  &clk);
        REG_PORT(&data_out_cache.clk, &data_out_cache, &clk);
        REG_PORT(&pe_confCache.clk,   &pe_confCache,   &clk);
        REG_PORT(&ch_confCache.clk,   &ch_confCache,   &clk);
        REG_PORT(&mmu.clk,            &mmu,            &clk);
        REG_PORT(&mu.clk,             &mu,             &clk);
        REG_PORT(&vcgra.rst,          &vcgra,          &rst);
        REG_PORT(&mu.rst,             &mu,             &rst);
        REG_PORT(&mu.run,             &mu,             &rin);
        REG_PORT(&mu.pause,           &mu,             &pause);
        REG_PORT(&mu.finish,          &mu,             &finish);
    #endif
    
    //bind remaining component ports to internal signals
    //==================================================
    //VCGRA
    vcgra.start.bind(s_vcgra_start);
    vcgra.ready.bind(s_vcgra_ready);
    vcgra.ch_config.bind(s_ch_config);
    vcgra.pe_config.bind(s_pe_config);
    #ifdef GSYSC
        RENAME_SIGNAL(&s_vcgra_start,
            (cgra::create_name<std::string,uint32_t>("s_vcgra_start_", 0)));
        RENAME_SIGNAL(&s_vcgra_ready,
            (cgra::create_name<std::string,uint32_t>("s_vcgra_ready_", 0)));
        RENAME_SIGNAL(&s_ch_config,
            (cgra::create_name<std::string,uint32_t>("s_ch_config_", 0)));
        RENAME_SIGNAL(&s_pe_config,
            (cgra::create_name<std::string,uint32_t>("s_pe_config_", 0)));

        REG_PORT(&vcgra.start,     &vcgra, &s_vcgra_start);
        REG_PORT(&vcgra.ready,     &vcgra, &s_vcgra_ready);
        REG_PORT(&vcgra.ch_config, &vcgra, &s_ch_config);
        REG_PORT(&vcgra.pe_config, &vcgra, &s_pe_config);
    #endif

    for(uint16_t idx = 0; 2 * cgra::cPeLevels.front() > idx; ++idx)
        vcgra.data_inputs.at(idx).bind(s_vcgra_data_inputs.at(idx));
        #ifdef GSYSC
            RENAME_SIGNAL(&s_vcgra_data_inputs.at(idx),
                (cgra::create_name<std::string,uint32_t>("s_vcgra_data_input_", 0)));

            REG_PORT(&vcgra.data_inputs, &vcgra, &s_vcgra_data_inputs.at(idx));
        #endif
    for(uint16_t idx = 0; cgra::cPeLevels.back() > idx; ++idx)
        vcgra.data_outputs.at(idx).bind(s_vcgra_data_outputs.at(idx));
        #ifdef GSYSC
            RENAME_SIGNAL(&s_vcgra_data_outputs.at(idx),
                (cgra::create_name<std::string,uint32_t>("s_vcgra_data_output_", 0)));
    
            REG_PORT(&vcgra.data_outputs, &vcgra, &s_vcgra_data_outputs.at(idx));
        #endif
    
    //PE configuration Cache
    pe_confCache.currentConfig.bind(s_pe_config);
    pe_confCache.dataInStream.bind(s_config_cache_stream);
    pe_confCache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE));
    pe_confCache.write.bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_PE));
    pe_confCache.slt_in.bind(s_pe_select_signals.first);
    pe_confCache.slt_out.bind(s_pe_select_signals.second);
    #ifdef GSYSC
        RENAME_SIGNAL(&s_config_cache_stream,
            (cgra::create_name<std::string,uint32_t>("s_config_cache_stream_", 0)));
        RENAME_SIGNAL(&s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE),
            (cgra::create_name<std::string,uint32_t>("s_acknowledges_CONF_PE_", 0)));
        RENAME_SIGNAL(&s_write_enables.at(MMU::CACHE_TYPE::CONF_PE),
            (cgra::create_name<std::string,uint32_t>("s_write_enables_CONF_PE_", 0)));
        RENAME_SIGNAL(&s_pe_select_signals.first,
            (cgra::create_name<std::string,uint32_t>("s_pe_select_signals_first_", 0)));
        RENAME_SIGNAL(&s_pe_select_signals.second,
            (cgra::create_name<std::string,uint32_t>("s_pe_select_signals_second_", 0)));

        REG_PORT(&pe_confCache.currentConfig, &pe_confCache, &s_pe_config);
        REG_PORT(&pe_confCache.dataInStream,  &pe_confCache, &s_config_cache_stream);
        REG_PORT(&pe_confCache.ack,           &pe_confCache, &s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE));
        REG_PORT(&pe_confCache.write,         &pe_confCache, &s_write_enables.at(MMU::CACHE_TYPE::CONF_PE));
        REG_PORT(&pe_confCache.slt_in,        &pe_confCache, &s_pe_select_signals.first);
        REG_PORT(&pe_confCache.slt_out,       &pe_confCache, &s_pe_select_signals.second);
    #endif
    
    
    //VCH Configuration Cache
    ch_confCache.currentConfig.bind(s_ch_config);
    ch_confCache.dataInStream.bind(s_config_cache_stream);
    ch_confCache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC));
    ch_confCache.write.bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_CC));
    ch_confCache.slt_in.bind(s_ch_select_signals.first);
    ch_confCache.slt_out.bind(s_ch_select_signals.second);
    #ifdef GSYSC
        RENAME_SIGNAL(&s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC),
            (cgra::create_name<std::string,uint32_t>("s_acknowledges_CONF_CC_", 0)));
        RENAME_SIGNAL(&s_write_enables.at(MMU::CACHE_TYPE::CONF_CC),
            (cgra::create_name<std::string,uint32_t>("s_write_enables_CONF_CC_", 0)));
        RENAME_SIGNAL(&s_ch_select_signals.first,
            (cgra::create_name<std::string,uint32_t>("s_ch_select_signals_first_", 0)));
        RENAME_SIGNAL(&s_ch_select_signals.second,
            (cgra::create_name<std::string,uint32_t>("s_ch_select_signals_second_", 0)));

        REG_PORT(&ch_confCache.currentConfig, &ch_confCache, &s_pe_config);
        REG_PORT(&ch_confCache.dataInStream,  &ch_confCache, &s_config_cache_stream);
        REG_PORT(&ch_confCache.ack,           &ch_confCache, &s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC));
        REG_PORT(&ch_confCache.write,         &ch_confCache, &s_write_enables.at(MMU::CACHE_TYPE::CONF_CC));
        REG_PORT(&ch_confCache.slt_in,        &ch_confCache, &s_ch_select_signals.first);
        REG_PORT(&ch_confCache.slt_out,       &ch_confCache, &s_ch_select_signals.second);
    #endif
    
    //Data Output Cache
    data_out_cache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    data_out_cache.load.bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    data_out_cache.dataOutStream.bind(s_data_out_stream);
    data_out_cache.update.bind(s_vcgra_ready);
    data_out_cache.slt_in.bind(s_doc_select_signals.first);
    data_out_cache.slt_out.bind(s_doc_select_signals.second);
    data_out_cache.slt_place.bind(s_cache_place);
    #ifdef GSYSC
        RENAME_SIGNAL(&s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT),
            (cgra::create_name<std::string,uint32_t>("s_acknowledges_DATA_OUTPUT_", 0)));
        RENAME_SIGNAL(&s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT),
            (cgra::create_name<std::string,uint32_t>("s_write_enables_DATA_OUTPUT_", 0)));
        RENAME_SIGNAL(&s_data_out_stream,
            (cgra::create_name<std::string,uint32_t>("s_data_out_stream_", 0)));
        RENAME_SIGNAL(&s_doc_select_signals.first,
            (cgra::create_name<std::string,uint32_t>("s_doc_select_signals_first_", 0)));
        RENAME_SIGNAL(&s_doc_select_signals.second,
            (cgra::create_name<std::string,uint32_t>("s_doc_select_signals_second_", 0)));
        RENAME_SIGNAL(&s_cache_place,
            (cgra::create_name<std::string,uint32_t>("s_cache_place_", 0)));

        REG_PORT(&data_out_cache.ack,           &data_out_cache, &s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
        REG_PORT(&data_out_cache.load,          &data_out_cache, &s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
        REG_PORT(&data_out_cache.dataOutStream, &data_out_cache, &s_data_out_stream);
        REG_PORT(&data_out_cache.update,        &data_out_cache, &s_vcgra_ready);
        REG_PORT(&data_out_cache.slt_in,        &data_out_cache, &s_doc_select_signals.first);
        REG_PORT(&data_out_cache.slt_out,       &data_out_cache, &s_doc_select_signals.second);
        REG_PORT(&data_out_cache.slt_place,     &data_out_cache, &s_cache_place);
    #endif

    for(uint16_t idx = 0; cgra::cPeLevels.back() > idx; ++idx)
        data_out_cache.currentResults.at(idx).bind(s_vcgra_data_outputs.at(idx));
        #ifdef GSYSC
            REG_PORT(&data_out_cache.currentResults, &data_out_cache, &s_vcgra_data_outputs.at(idx));
        #endif
    
    //Data Input Cache
    data_in_cache.ack.bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT));
    data_in_cache.write.bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_INPUT));
    data_in_cache.dataInStream.bind(s_data_in_stream);
    data_in_cache.slt_in.bind(s_dic_select_signals.first);
    data_in_cache.slt_out.bind(s_dic_select_signals.second);
    data_in_cache.slt_place.bind(s_cache_place);
    #ifdef GSYSC
        RENAME_SIGNAL(&s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT),
            (cgra::create_name<std::string,uint32_t>("s_acknowledges_DATA_INPUT_", 0)));
        RENAME_SIGNAL(&s_write_enables.at(MMU::CACHE_TYPE::DATA_INPUT),
            (cgra::create_name<std::string,uint32_t>("s_write_enables_DATA_INPUT_", 0)));
        RENAME_SIGNAL(&s_data_in_stream,
            (cgra::create_name<std::string,uint32_t>("s_data_in_stream_", 0)));
        RENAME_SIGNAL(&s_dic_select_signals.first,
            (cgra::create_name<std::string,uint32_t>("s_dic_select_signals_first_", 0)));
        RENAME_SIGNAL(&s_dic_select_signals.second,
            (cgra::create_name<std::string,uint32_t>("s_dic_select_signals_second_", 0)));

        REG_PORT(&data_in_cache.ack,          &data_in_cache, &s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
        REG_PORT(&data_in_cache.write,        &data_in_cache, &s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
        REG_PORT(&data_in_cache.dataInStream, &data_in_cache, &s_data_in_stream);
        REG_PORT(&data_in_cache.slt_in,       &data_in_cache, &s_dic_select_signals.first);
        REG_PORT(&data_in_cache.slt_out,      &data_in_cache, &s_dic_select_signals.second);
        REG_PORT(&data_in_cache.slt_place,    &data_in_cache, &s_cache_place);
    #endif

    for(uint16_t idx = 0; 2 * cgra::cPeLevels.front() > idx; ++idx)
        data_in_cache.currentValues.at(idx).bind(s_vcgra_data_inputs.at(idx));
        #ifdef GSYSC
            RENAME_SIGNAL(&s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT),
                (cgra::create_name<std::string,uint32_t>("s_acknowledges_DATA_INPUT_", 0)));
            REG_PORT(&data_in_cache.slt_place,    &data_in_cache, &s_cache_place);
        #endif
    
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
    #ifdef GSYSC
        RENAME_SIGNAL(&s_mmu_start,
            (cgra::create_name<std::string,uint32_t>("s_mmu_start_", 0)));
        RENAME_SIGNAL(&s_mmu_ready,
            (cgra::create_name<std::string,uint32_t>("s_mmu_ready_", 0)));
        RENAME_SIGNAL(&s_address,
            (cgra::create_name<std::string,uint32_t>("s_address_", 0)));
        RENAME_SIGNAL(&s_place,
            (cgra::create_name<std::string,uint32_t>("s_place_", 0)));
        RENAME_SIGNAL(&s_cache_select,
            (cgra::create_name<std::string,uint32_t>("s_cache_select_", 0)));

        REG_PORT(&mu.start,                     &mu, &s_vcgra_start);
        REG_PORT(&mu.ready,                     &mu, &s_vcgra_ready);
        REG_PORT(&mu.mmu_start,                 &mu, &s_mmu_start);
        REG_PORT(&mu.mmu_ready,                 &mu, &s_mmu_ready);
        REG_PORT(&mu.address,                   &mu, &s_address);
        REG_PORT(&mu.place,                     &mu, &s_place);
        REG_PORT(&mu.cache_select,              &mu, &s_cache_select);
        REG_PORT(&mu.pe_cc_select_lines.first,  &mu, &s_pe_select_signals.first);
        REG_PORT(&mu.pe_cc_select_lines.second, &mu, &s_pe_select_signals.second);
        REG_PORT(&mu.ch_cc_select_lines.first,  &mu, &s_ch_select_signals.first);
        REG_PORT(&mu.ch_cc_select_lines.second, &mu, &s_ch_select_signals.second);
        REG_PORT(&mu.dic_select_lines.first,    &mu, &s_dic_select_signals.first);
        REG_PORT(&mu.dic_select_lines.second,   &mu, &s_dic_select_signals.second);
        REG_PORT(&mu.doc_select_lines.first,    &mu, &s_doc_select_signals.first);
        REG_PORT(&mu.doc_select_lines.second,   &mu, &s_doc_select_signals.second);
    #endif
    
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
    #ifdef GSYSC
        RENAME_SIGNAL(&s_acknowledges.at(MMU::CACHE_TYPE::NONE),
            (cgra::create_name<std::string,uint32_t>("s_acknowledges_NONE_", 0)));
        RENAME_SIGNAL(&s_write_enables.at(MMU::CACHE_TYPE::NONE),
            (cgra::create_name<std::string,uint32_t>("s_write_enables_NONE_", 0)));

        REG_PORT(&mmu.start,                 &mmu, &s_mmu_start);
        REG_PORT(&mmu.ready,                 &mmu, &s_mmu_ready);
        REG_PORT(&mmu.address,               &mmu, &s_address);
        REG_PORT(&mmu.place,                 &mmu, &s_place);
        REG_PORT(&mmu.conf_cache_stream,     &mmu, &s_config_cache_stream);
        REG_PORT(&mmu.cache_select,          &mmu, &s_cache_select);
        REG_PORT(&mmu.ack,                   &mmu, &s_acknowledges.at(MMU::CACHE_TYPE::NONE));
        REG_PORT(&mmu.write_enable,          &mmu, &s_write_enables.at(MMU::CACHE_TYPE::NONE));
        REG_PORT(&mmu.data_value_in_stream,  &mmu, &s_data_out_stream);
        REG_PORT(&mmu.data_value_out_stream, &mmu, &s_data_in_stream);
        REG_PORT(&mmu.cache_place,           &mmu, &s_cache_place);
    #endif
    
    //WE DeMUX
    we_dmux.input.bind(s_write_enables.at(MMU::CACHE_TYPE::NONE));
    we_dmux.outputs.at(MMU::CACHE_TYPE::CONF_PE).bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_PE));
    we_dmux.outputs.at(MMU::CACHE_TYPE::CONF_CC).bind(s_write_enables.at(MMU::CACHE_TYPE::CONF_CC));
    we_dmux.outputs.at(MMU::CACHE_TYPE::DATA_INPUT).bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_INPUT));
    we_dmux.outputs.at(MMU::CACHE_TYPE::DATA_OUTPUT).bind(s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    we_dmux.select.bind(s_cache_select);
    #ifdef GSYSC
        REG_PORT(&we_dmux.input,                                    &we_dmux, &s_write_enables.at(MMU::CACHE_TYPE::NONE));
        REG_PORT(&we_dmux.outputs.at(MMU::CACHE_TYPE::CONF_PE),     &we_dmux, &s_write_enables.at(MMU::CACHE_TYPE::CONF_PE));
        REG_PORT(&we_dmux.outputs.at(MMU::CACHE_TYPE::CONF_CC),     &we_dmux, &s_write_enables.at(MMU::CACHE_TYPE::CONF_CC));
        REG_PORT(&we_dmux.outputs.at(MMU::CACHE_TYPE::DATA_INPUT),  &we_dmux, &s_write_enables.at(MMU::CACHE_TYPE::DATA_INPUT));
        REG_PORT(&we_dmux.outputs.at(MMU::CACHE_TYPE::DATA_OUTPUT), &we_dmux, &s_write_enables.at(MMU::CACHE_TYPE::DATA_OUTPUT));
        REG_PORT(&we_dmux.select,                                   &we_dmux, &s_cache_select);
    #endif
    
    //Ack MUX
    ack_mux.output.bind(s_acknowledges.at(MMU::CACHE_TYPE::NONE));
    ack_mux.inputs.at(MMU::CACHE_TYPE::CONF_PE).bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE));
    ack_mux.inputs.at(MMU::CACHE_TYPE::CONF_CC).bind(s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC));
    ack_mux.inputs.at(MMU::CACHE_TYPE::DATA_INPUT).bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT));
    ack_mux.inputs.at(MMU::CACHE_TYPE::DATA_OUTPUT).bind(s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
    ack_mux.select.bind(s_cache_select);
    #ifdef GSYSC
        REG_PORT(&ack_mux.output,                                  &ack_mux, &s_acknowledges.at(MMU::CACHE_TYPE::NONE));
        REG_PORT(&ack_mux.inputs.at(MMU::CACHE_TYPE::CONF_PE),     &ack_mux, &s_acknowledges.at(MMU::CACHE_TYPE::CONF_PE));
        REG_PORT(&ack_mux.inputs.at(MMU::CACHE_TYPE::CONF_CC),     &ack_mux, &s_acknowledges.at(MMU::CACHE_TYPE::CONF_CC));
        REG_PORT(&ack_mux.inputs.at(MMU::CACHE_TYPE::DATA_INPUT),  &ack_mux, &s_acknowledges.at(MMU::CACHE_TYPE::DATA_INPUT));
        REG_PORT(&ack_mux.inputs.at(MMU::CACHE_TYPE::DATA_OUTPUT), &ack_mux, &s_acknowledges.at(MMU::CACHE_TYPE::DATA_OUTPUT));
        REG_PORT(&ack_mux.select,                                  &ack_mux, &s_cache_select);
    #endif

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
