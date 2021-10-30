/*
 * TopLevel.h
 *
 *  Created on: Jan 20, 2018
 *      Author: andrewerner
 */
#ifndef TOPLEVEL_H_
#define TOPLEVEL_H_

#include <array>
#include <utility>
#include <iostream>
#include "Typedef.h"
#include "VCGRA.h"
#include "ConfigurationCache.h"
#include "DataInCache.h"
#include "DataOutCache.h"
#include "MMU.h"
#include "Management_Unit.h"
#include "General_Mux.h"
#include "General_DeMux.h"

#ifdef GSYSC
#include <gsysc.h>
#endif

namespace cgra 
{
/*!
 * \class TopLevel
 * 
 * \brief Architecture including VCGRA, ConfigurationCache, MMU and ManagementUnit
 * 
 * \details
 * This class includes the architecture with all it components and necessary
 * connections. Ports are public available to enable tracing. 
 * 
 */
class TopLevel : public sc_core::sc_module
{
public:
    //Signal type definitions
    typedef cgra::reset_type_t reset_type_t;
    //!< \brief Type for architecture reset signal.
    typedef cgra::ready_type_t ready_type_t;
    //!< \brief Type for VCGRA ready signal.
    typedef cgra::start_type_t start_type_t;
    //!< \brief Type for VCGRA start signal.
    typedef cgra::clock_type_t clock_type_t;
    //!< \brief Type for architecture clock signal.
    typedef cgra::VCGRA::data_input_type_t data_input_type_t;
    //!< \brief VCGRA data input signal type.
    typedef cgra::VCGRA::data_output_type_t data_output_type_t;
    //!< \brief VCGRA data output signal type.
    typedef cgra::pe_config_type_t pe_config_type_t;
    //!< \brief VCGRA ProcessingElement configuration type.
    typedef cgra::ch_config_type_t ch_config_type_t;
    //!< \brief VCGRA VirtualChannel and Synchronizer configuration type.
    typedef MMU::data_stream_type_t data_cache_stream_type_t;
    //!< \brief Streaming type for data transmissions between MMU and data caches.
    typedef MMU::conf_stream_type_t config_cache_stream_type_t;
    //!< \brief Streaming type for data transmissions between MMU and configuration caches.
    typedef MMU::cache_place_type_t data_cache_place_type_t;
    //!< \brief Selection type for available caches.
    typedef MMU::start_type_t mmu_start_type_t;
    //!< \brief Signal type for MMU start signal.
    typedef MMU::ready_type_t mmu_ready_type_t;
    //!< \brief Signal type for MMU ready signal.
    typedef cgra::cache_write_type_t write_enable_type_t;
    //!< \brief Signal type for data and configuration cache write enable signals.
    typedef cgra::cache_ack_type_t ack_type_t;
    //!< \brief Signal type for data and configuration cache acknowledge signals.
    typedef ManagementUnit::run_type_t run_type_t;
    //!< \brief Signal type for ManagementUnit run signal.
    typedef ManagementUnit::pause_type_t pause_type_t;
    //!< \brief Signal type for ManagementUnit pause signal.
    typedef ManagementUnit::finish_type_t finish_type_t;
    //!< \brief Signal type for ManagementUnit finish signal.
    typedef ManagementUnit::assembler_type_t assembler_type_t;
    //!< \brief Assembler program type for MangementUnit commands.
    typedef ManagementUnit::address_type_t address_type_t;
    //!< \brief Address signal type for MMU and ManagementUnit connection.
    typedef ManagementUnit::place_type_t place_type_t;
    //!< \brief Signal type for data and configuration place number.
    typedef ManagementUnit::dic_slct_type_t dic_slt_type_t;
    //!< \brief Signal type for DataInCache select cache line signal.
    typedef ManagementUnit::doc_slct_type_t doc_slt_type_t;
    //!< \brief Signal type for DataOutCache select cache line signal.
    typedef ManagementUnit::pe_cc_slct_type_t pe_cc_slt_type_t;
    //!< \brief Signal type for PE ConfigurationCache select cache line signal.
    typedef ManagementUnit::ch_cc_slct_type_t ch_cc_slt_type_t;
    //!< \brief Signal type for VCH ConfigurationCache select cache line signal.
    
#ifndef GSYSC
    //Signal Ports to TestBench
    sc_core::sc_in<clock_type_t> clk{"clock"};
    //!< \brief Clock port of architecture.
    sc_core::sc_in<run_type_t> run{"run"};
    //!< \brief Run signal to start execution of architecture if true.
    sc_core::sc_in<pause_type_t> pause{"pause"};
    //!< \brief Pause execution of architecture if true, continue when false.
    sc_core::sc_in<reset_type_t> rst{"reset"};
    //!< \brief Reset architecture when false. Execution starts at beginning.
    sc_core::sc_out<finish_type_t> finish{"finish"};
    //!< \brief Architecture shows finished processing of all assembler commands.
#else
    //Signal Ports to TestBench
    sc_in<clock_type_t> clk{"clock"};
    //!< \brief Clock port of architecture.
    sc_in<run_type_t> run{"run"};
    //!< \brief Run signal to start execution of architecture if true.
    sc_in<pause_type_t> pause{"pause"};
    //!< \brief Pause execution of architecture if true, continue when false.
    sc_in<reset_type_t> rst{"reset"};
    //!< \brief Reset architecture when false. Execution starts at beginning.
    sc_out<finish_type_t> finish{"finish"};
    //!< \brief Architecture shows finished processing of all assembler commands.
#endif

    /*
     * Internal Signals and Component Instances
     * These signals and Components are public to ease their tracing.
    */ 
    //Components
    cgra::VCGRA vcgra{"vcgra"};
    //!< \brief VCGRA instance within the architecture
    cgra::data_input_cache_type_t data_in_cache{"dic"};
    //!< \brief VCGRA data value input cache.
    cgra::data_output_cache_type_t data_out_cache{"doc"};
    //!< \brief VCGRA data value output cache.
    cgra::pe_config_cache_type_t pe_confCache{"pe_cc", cgra::cPeConfigBitWidth};
    //!< \brief ConfigurationCache instance for ProcessingElement configuration.
    cgra::ch_config_cache_type_t ch_confCache{"ch_cc", cgra::cVChConfigBitWidth};
    //!< \brief ConfigurationCache instance for VirtualChannel and Synchronizer configuration.
    cgra::MMU mmu{"mmu", cgra::cCacheFeatures};
    //!< \brief MMU instance within the architecture.
    cgra::ManagementUnit mu;
    //!< \brief ManagementUnit instance within the architecture.
    cgra::General_DeMux<cgra::cache_write_type_t, 4, 3> we_dmux{"we_dmux"};
    //!< \brief Demultiplexer to contribute MMU write enable signal to all available caches.
    cgra::General_Mux<cgra::cache_ack_type_t, 4, 3> ack_mux{"ack_mux"};
    //!< \brief Multiplexer to collect acknowledge signals from all available caches.
    
#ifndef GSYSC
    //Signals
    sc_core::sc_signal<start_type_t> s_vcgra_start{"vcgra_start"};
    //!< \brief VCGRA start signal from ManagementUnit to VCGRA
    sc_core::sc_signal<ready_type_t> s_vcgra_ready{"vcgra_ready"};
    //!< \brief VCGRA ready signal from VCGRA to ManagementUnit
    sc_core::sc_signal<pe_config_type_t> s_pe_config{"pe_config"};
    //!< \brief ProcessingElement configuration from PE ConfigurationCache to VCGRA.
    sc_core::sc_signal<ch_config_type_t> s_ch_config{"ch_config"};
    //!< \brief VirtualChannel configuration from VCH ConfigurationCache to VCGRA.
    std::array<sc_core::sc_signal<data_input_type_t>, 2 * cgra::cPeLevels.front()> s_vcgra_data_inputs;
    //!< \brief VCGRA data inputs from DataInCache to VCGRA.
    std::array<sc_core::sc_signal<data_output_type_t>, cgra::cPeLevels.back()> s_vcgra_data_outputs;
    //!< \brief VCGRA data outputs from VCGRA to DataOutCache.
    sc_core::sc_signal<cache_slct_type_t> s_cache_select{"cache_select"};
    //!< \brief Signal to select a target cache type for transmissions between MMU and an available cache.
    sc_core::sc_signal<config_cache_stream_type_t> s_config_cache_stream{"config_cache_stream"};
    //!< \brief Signal to connect MMU with configuration caches to load new configurations.
    sc_core::sc_signal<data_cache_stream_type_t> s_data_out_stream{"data_out_stream"};
    //!< \brief Signal to connect MMU with DataOutCache instance to store results in shared memory.
    sc_core::sc_signal<data_cache_stream_type_t> s_data_in_stream{"data_in_stream"};
    //!< \brief Signal to connect MMU with DataInCache instance to load values from shared memory.
    std::array<sc_core::sc_signal<ack_type_t>, 5> s_acknowledges;
    //!< \brief Ackknowledge signals of available caches. The last entry is for MUX to MMU connection.
    std::array<sc_core::sc_signal<write_enable_type_t>, 5> s_write_enables;
    //!< \brief Write enables signals for available caches. The last entry is for DMUX to MMU connection.
    std::pair<sc_core::sc_signal<pe_cc_slt_type_t>, sc_core::sc_signal<pe_cc_slt_type_t>> s_pe_select_signals;
    //!< \brief Selection signals for ProcessingElement ConfigurationCache (first=slt_in, second=slt_out).
    std::pair<sc_core::sc_signal<ch_cc_slt_type_t>, sc_core::sc_signal<ch_cc_slt_type_t>> s_ch_select_signals;
    //!< \brief Selection signals for VirtualChannel ConfigurationCache (first=slt_in, second=slt_out).
    std::pair<sc_core::sc_signal<dic_slt_type_t>, sc_core::sc_signal<dic_slt_type_t>> s_dic_select_signals;
    //!< \brief Selection signals for DataInCache (first=slt_in, second=slt_out).
    std::pair<sc_core::sc_signal<doc_slt_type_t>, sc_core::sc_signal<doc_slt_type_t>> s_doc_select_signals;
    //!< \brief Selection signals for DataOutCache (first=slt_in, second=slt_out).
    sc_core::sc_signal<data_cache_place_type_t> s_cache_place{"data_cache_place"};
    //!< \brief Select a place for a value or result in one of the data caches.
    sc_core::sc_signal<mmu_start_type_t> s_mmu_start{"mmu_start"};
    //!< \brief Start signal from ManagementUnit for MMU.
    sc_core::sc_signal<mmu_ready_type_t> s_mmu_ready{"mmu_ready"};
    //!< \brief Ready signal of MMU for ManagementUnit.
    sc_core::sc_signal<address_type_t> s_address{"address"};
    //!< \brief Shared memory address for MMU to access data.
    sc_core::sc_signal<place_type_t> s_place{"place"};
    //!< \brief Value place in target cache of current selected line.
#else
    //Signals
    sc_signal<start_type_t> s_vcgra_start{"vcgra_start"};
    //!< \brief VCGRA start signal from ManagementUnit to VCGRA
    sc_signal<ready_type_t> s_vcgra_ready{"vcgra_ready"};
    //!< \brief VCGRA ready signal from VCGRA to ManagementUnit
    sc_signal<pe_config_type_t> s_pe_config{"pe_config"};
    //!< \brief ProcessingElement configuration from PE ConfigurationCache to VCGRA.
    sc_signal<ch_config_type_t> s_ch_config{"ch_config"};
    //!< \brief VirtualChannel configuration from VCH ConfigurationCache to VCGRA.
    std::array<sc_signal<data_input_type_t>, 2 * cgra::cPeLevels.front()> s_vcgra_data_inputs;
    //!< \brief VCGRA data inputs from DataInCache to VCGRA.
    std::array<sc_signal<data_output_type_t>, cgra::cPeLevels.back()> s_vcgra_data_outputs;
    //!< \brief VCGRA data outputs from VCGRA to DataOutCache.
    sc_signal<cache_slct_type_t> s_cache_select{"cache_select"};
    //!< \brief Signal to select a target cache type for transmissions between MMU and an available cache.
    sc_signal<config_cache_stream_type_t> s_config_cache_stream{"config_cache_stream"};
    //!< \brief Signal to connect MMU with configuration caches to load new configurations.
    sc_signal<data_cache_stream_type_t> s_data_out_stream{"data_out_stream"};
    //!< \brief Signal to connect MMU with DataOutCache instance to store results in shared memory.
    sc_signal<data_cache_stream_type_t> s_data_in_stream{"data_in_stream"};
    //!< \brief Signal to connect MMU with DataInCache instance to load values from shared memory.
    std::array<sc_signal<ack_type_t>, 5> s_acknowledges;
    //!< \brief Ackknowledge signals of available caches. The last entry is for MUX to MMU connection.
    std::array<sc_signal<write_enable_type_t>, 5> s_write_enables;
    //!< \brief Write enables signals for available caches. The last entry is for DMUX to MMU connection.
    std::pair<sc_signal<pe_cc_slt_type_t>, sc_signal<pe_cc_slt_type_t>> s_pe_select_signals;
    //!< \brief Selection signals for ProcessingElement ConfigurationCache (first=slt_in, second=slt_out).
    std::pair<sc_signal<ch_cc_slt_type_t>, sc_signal<ch_cc_slt_type_t>> s_ch_select_signals;
    //!< \brief Selection signals for VirtualChannel ConfigurationCache (first=slt_in, second=slt_out).
    std::pair<sc_signal<dic_slt_type_t>, sc_signal<dic_slt_type_t>> s_dic_select_signals;
    //!< \brief Selection signals for DataInCache (first=slt_in, second=slt_out).
    std::pair<sc_signal<doc_slt_type_t>, sc_signal<doc_slt_type_t>> s_doc_select_signals;
    //!< \brief Selection signals for DataOutCache (first=slt_in, second=slt_out).
    sc_signal<data_cache_place_type_t> s_cache_place{"data_cache_place"};
    //!< \brief Select a place for a value or result in one of the data caches.
    sc_signal<mmu_start_type_t> s_mmu_start{"mmu_start"};
    //!< \brief Start signal from ManagementUnit for MMU.
    sc_signal<mmu_ready_type_t> s_mmu_ready{"mmu_ready"};
    //!< \brief Ready signal of MMU for ManagementUnit.
    sc_signal<address_type_t> s_address{"address"};
    //!< \brief Shared memory address for MMU to access data.
    sc_signal<place_type_t> s_place{"place"};
    //!< \brief Value place in target cache of current selected line.
#endif
    
    /*!
     * \brief Constructor
     * 
     * \param[in] nameA Unique module name for TopLevel instance.
     * \param[in] assemblerPtrA Pointer to an array of assembler commands.
     * \param[in] programSizeA Length of the array including the assembler commands.
     */ 
    TopLevel(const sc_core::sc_module_name& nameA, assembler_type_t* assemblerPtrA, uint64_t programSizeA);
    
    /*!
     * \brief Initialize output signals
     */
    virtual void end_of_elaboration() override;
    
    /*!
     * \brief Return kind of architecture component.
     */
    virtual const char * kind() const override
    { return "TopLevel"; }
    
    /*!
     * \brief Write module name to defined outstream.
     * 
     * \param[out] os Target outstream for module name. [default: std::cout]
     */
    virtual void print(::std::ostream & os = std::cout) const override
    { os << name(); return; }
    
    /*!
     * \brief Dump architecture information.
     * 
     * \param[out] os Target outstream for information dump.
     */
    virtual void dump(::std::ostream & os = std::cout) const override;
    
    /*!
     * \brief Destructor
     */
    virtual ~TopLevel() = default;
    
private:
    
    //Forbidden Constructors
    TopLevel() = delete;
    //!< \brief sc_module does always need a unique module name.
    TopLevel(const TopLevel& src) = delete;
    //!< \brief No Copy constructor for base class sc_module available.
    TopLevel& operator=(const TopLevel& src) = delete;
    //!< \brief No Copy constructor for base class sc_module available.
    TopLevel(TopLevel&& src) = delete;
    //!< \brief No Move constructor implemented for base class sc_module.
    TopLevel& operator=(TopLevel&& src) = delete;
    //!< \brief No Move constructor implemented for base class sc_module.
    
}; /* End class TopLevel */
    
} /* End namespace cgra */

#endif /* TOPLEVEL_H_ */
