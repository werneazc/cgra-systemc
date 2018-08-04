/*
 * vcgra_instance.h
 *
 *  Created on: Apr 5, 2018
 *      Author: andrewerner
 */

#ifndef HEADER_VCGRA_INSTANCE_H_
#define HEADER_VCGRA_INSTANCE_H_


#include <cstdint>
#include <systemc>
#include <array>
#include "Typedef.h"
#include "VirtualChannel.h"
#include "ProcessingElement.h"
#include "ConfigurationCache.h"
#include "Demultiplexer.h"
#include "Selector.h"
#include "Synchronizer.h"

namespace cgra {

/************************************************************************/
// VCGRA instance
//!
//! \class VCGRA
//! \brief A VCGRA instance as an example
//!
//! \details
//! This is a small example of a VCGRA. IT consists of 12 Processing_Elements and 3 virtual
//! channels, where a row of a VCGRA consists of 4 PEs and one channel, each.
//! The VCGRA additionally includes 2 configuration caches: One for all PEs
//! and one for all channels.
//!
/************************************************************************/
class VCGRA : public sc_core::sc_module
{
public:
	typedef Processing_Element<16,16> pe_type_t;
	//!< \brief Type of all Processing_Elements within the VCGRA
	typedef VirtualChannel<8,16,8,16,3> input_channel_type_t;
	//!< \brief Type of the first VirtualChannel for external communication
	typedef VirtualChannel<4,16,8,16,2> general_channel_type_t;
	//!< \brief General VirtualChannel type for internal channels
	typedef ConfigurationCache<sc_dt::sc_lv<48>,1,2,8> pe_config_cache_type_t;
	//!< \brief Type for Processing_Element configuration cache
	typedef ConfigurationCache<sc_dt::sc_lv<64>,1,2,8> ch_config_cache_type_t;
	//!< \brief Type for VirtualChannel configuration cache
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Clock type definition
	typedef cgra::reset_type_t reset_type_t;
	//!< \brief Reset type definition
	typedef cgra::ready_type_t ready_type_t;
	//!< \brief Ready type definition
	typedef typename pe_type_t::enable_type_t start_type_t;
	//!< \brief Start type definition
	typedef typename pe_type_t::output_type_t output_type_t;
	//!< \brief VCGRA output type definition
	typedef typename pe_type_t::input_type_t input_type_t;
	//!< \brief VCGRA input type definition

	//VCGRA interfaces
	//================

	//Control:
	//--------
	sc_core::sc_in<start_type_t> start{"start"};
	//!< \brief Start input
	sc_core::sc_in<clock_type_t> clk{"clock"};
	//!< \brief Clock input
	sc_core::sc_in<reset_type_t> rst{"reset"};
	//!< \brief Reset input
	sc_core::sc_out<ready_type_t> ready{"ready"};
	//!< \brief Ready signal of last PE level
	sc_core::sc_in<pe_config_cache_type_t::write_enable_type_t> write_pe_cc{"write_pe_cc"};
	//!< \brief Write enable for ProcessingElements ConfigurationCache
	sc_core::sc_in<pe_config_cache_type_t::select_type_t> slct_in_pe_cc{"slct_in_pe_cc"};
	//!< \brief Select port for input cache line for ProcessingElements ConfigurationCache
	sc_core::sc_in<pe_config_cache_type_t::select_type_t> slct_out_pe_cc{"slct_out_pe_cc"};
	//!< \brief Select port for output cache line for ProcessingElements ConfigurationCache
	sc_core::sc_out<pe_config_cache_type_t::ack_type_t> ack_pe_cc{"ack_pe_cc"};
	//!< \brief Acknowledge output for ProcessingElements ConfigurationCache
	sc_core::sc_in<ch_config_cache_type_t::write_enable_type_t> write_ch_cc{"write_ch_cc"};
	//!< \brief Write enable for VirtualChannel ConfigurationCache
	sc_core::sc_in<ch_config_cache_type_t::select_type_t> slct_in_ch_cc{"slct_in_ch_cc"};
	//!< \brief Select port for input cache line for VirtualChannel ConfigurationCache
	sc_core::sc_in<ch_config_cache_type_t::select_type_t> slct_out_ch_cc{"slct_out_ch_cc"};
	//!< \brief Select port for output cache line for VirtualChannel ConfigurationCache
	sc_core::sc_out<ch_config_cache_type_t::ack_type_t> ack_ch_cc{"ack_ch_cc"};
	//!< \brief Acknowledge output for VirtualChannel ConfigurationCache

	//Data:
	//-----
	std::array<sc_core::sc_in<input_type_t>,8> inputs;
	//!< \brief input signal vector of a VCGRA
	std::array<sc_core::sc_out<output_type_t>,4> results;
	//!< \brief result signal vector of last PE level
	sc_core::sc_in<pe_config_cache_type_t::stream_type_t> pe_config_stream{"pe_config_stream"};
	//!< \brief result signal vector of last PE level
	sc_core::sc_in<ch_config_cache_type_t::stream_type_t> ch_config_stream{"ch_config_stream"};
	//!< \brief result signal vector of last PE level


	//Constructor
	/*!
	 * \brief Constructor of a VCGRA
	 * \param[in] name VCGRA module name
	 */
	VCGRA(const sc_core::sc_module_name& name);


	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "VCGRA";
	}

	/*!
	 * \brief Write current signals to output
	 *
	 * \param[out] 	os 	Output stream to write information to
	 * \param[in] 	tb 	Constant reference to a VCGRA instance
	 */
	friend std::ostream& operator<<(std::ostream& os, const VCGRA& vcgra);

	/*!
	 * \brief Dump the VCGRA structure
	 *
	 * \param[out] os out-stream where to write to (default: std::cout)
	 */
	virtual void dump(std::ostream& os = ::std::cout) const override;

	/*!
	 * \brief Dump the VCGRA structure
	 *
	 * \param[out] os out-stream where to write to (default: std::cout)
	 */
	virtual void end_of_elaboration() override
	{
		//#TODO: Include checks for design
#ifdef DEBUG
#endif
	}

	//Destructor
	/*!
	 * \brief Destructor of a VCGRA
	 */
	virtual ~VCGRA() = default;

private:

	//forbidden constructor
	VCGRA() = delete;
	VCGRA(const VCGRA &source) = delete;			  	//!< \brief because sc_module could not be copied
	VCGRA(VCGRA &&source) = delete;				  		//!< \brief because move not implemented for sc_module
	VCGRA &operator=(const VCGRA &source) = delete; 	//!< \brief because sc_module could not be copied
	VCGRA &operator=(VCGRA &&source) = delete;	  		//!< \brief because move not implemented for sc_module

	//Private type definitions
	typedef Demultiplexer<pe_config_cache_type_t::config_type_t,12,48> pe_config_demux_type_t;
	//!< \brief Demultiplexer type for PE configuration
	typedef Selector<ch_config_cache_type_t::config_type_t,1,24,24> in_ch_config_selector_type_t;
	//!< \brief Selector type for input channel configuration
	typedef Selector<ch_config_cache_type_t::config_type_t,2,16,32> ch_config_selector_type_t;
	//!< \brief Selector type for general channel configurations
	typedef Selector<ch_config_cache_type_t::config_type_t,1,4,4> sync_selector_type_t;
	//!< \brief Selector type for synchronization configuration
	typedef Synchronizer<pe_type_t::valid_type_t,4> synchronizer_type_t;
	//!< \brief Selector type for synchronization configuration

	//Private member:
	pe_config_demux_type_t m_pe_config_demux{"PE_Config_Demux"};
	//!< \brief Demultiplexer to split PE configuration vector into parts
	in_ch_config_selector_type_t m_in_ch_config_selector{"In_Channel_Config_Selector", 0, 64};
	//!< \brief Selector to select input channel configuration from channel configuration vector
	ch_config_selector_type_t m_ch_config_selector{"Channel_Config_Selector", 24, 64};
	//!< \brief Selector to distribute general channel configurations from channel configuration vector
	sync_selector_type_t m_sync_selector{"Sync_Selector", 56, 64};
	//!< \brief Selector to select Synchronizer configuration from channel configuration vector
	sc_core::sc_vector<pe_type_t> m_pe_array{"VCGRA_PEs"};
	//!< \brief Vector of all Processing_Elements in the VCGRA
	input_channel_type_t m_input_ch{"Input_Channel"};
	//!< \brief Input channel for external communication
	sc_core::sc_vector<general_channel_type_t> m_ch_array{"VCGRA_Channels",2};
	//!< \brief Vector of further VirtualChannels within the array
	pe_config_cache_type_t m_pe_cc{"PE_Config_Cache", 48};
	//!< \brief Instantiation of Processing_Element configuration cache
	ch_config_cache_type_t m_ch_cc{"Channel_Config_Cache", 64};
	//!< \brief Instantiation of VirtualChannel configuration cache
	synchronizer_type_t m_sync{"Sync"};
	//!< \brief Instantiation of VCGRA caller Synchronizer
	std::array<sc_core::sc_signal<pe_type_t::valid_type_t>, 12> m_valid_signals;
	//!< \brief Signal vector to connect valid signals of PEs to its succeeding channels
	std::array<sc_core::sc_signal<pe_type_t::output_type_t>, 12> m_res_signals;
	//!< \brief Signal vector to connect result signals of PEs to its succeeding channels
	std::array<sc_core::sc_signal<input_channel_type_t::output_type_t>, 24> m_ch_outputs_signals;
	//!< \brief Signal vector to connect output signals of VirtualChannels to its succeeding PEs
	std::array<sc_core::sc_signal<input_channel_type_t::enables_type_t>, 24> m_ch_enable_signals;
	//!< \brief Signal vector to connect enable signals of VirtualChannels to its succeeding PEs
	std::array<sc_core::sc_signal<pe_config_demux_type_t::configpart_type_t>, 12> m_pe_conf_part_signals;
	//!< \brief Signal vector to connect configuration signals of PE configuration cache and PEs
	sc_core::sc_signal<sync_selector_type_t::configpart_type_t> m_sync_config_signal{"m_sync_config"};
	//!< \brief Signal for Synchronizer configuration
	sc_core::sc_signal<input_channel_type_t::conf_type_t> m_input_ch_config_signal{"m_input_ch_config"};
	//!< \brief Signal for input VirtualChannel configuration
	std::array<sc_core::sc_signal<general_channel_type_t::conf_type_t>, 2> m_vCh_config_signals;
	//!< \brief Signal for Synchronizer configuration
	sc_core::sc_signal<pe_config_cache_type_t::config_type_t> m_pe_cc_current_config_signal{"m_ch_pe_current_config"};
	//!< \brief Signal to connect configuration cache output of PEs to Demultiplexer
	sc_core::sc_signal<ch_config_cache_type_t::config_type_t> m_ch_cc_current_config_signal{"m_ch_cc_current_config"};
	//!< \brief Signal to connect configuration cache output for VirtualChannels to configuration Selectors


	/*!
	 * \struct pe_creator
	 *
	 * \brief Functor to create PEs for Processing_Element vector.
	 *
	 * \details
	 * It is necessary to create a unique ID for a Processing_Element.
	 * The standard Constructor for a vector of SystemC
	 * modules doesn't consists additional parameters
	 * for further constructor arguments of its elements.
	 * Therefore, we use a Functor to create the Processing_Element which than
	 * is passed into the array.
	 *
	 * \param[in] pe_id Unique ID of a processing element.
	 *
	 */
	struct pe_creator
	{
	public:

		//Contructor
		/*!
		 * \brief Constructor for Processing_Element creator
		 * \param[in] Unique ID of a processing element.
		 */
		pe_creator(uint32_t pe_id) : m_pe_id(pe_id) {};

		//Functor
		/*!
		 * \brief Functor for an pe_type instance
		 * \details
		 * Parameters are defined by sc_core::sc_vector
		 * constructor with creator.
		 */
		pe_type_t* operator()(const char* name, size_t size)
		{
			return new pe_type_t(name, m_pe_id++);
		}

		uint32_t m_pe_id;
		//!< Unique Processing_Element ID
	};

};

} // end namespace cgra

#endif /* HEADER_VCGRA_INSTANCE_H_ */
