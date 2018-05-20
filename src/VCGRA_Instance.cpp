/*
 * VCGRA_Instance.cpp
 *
 *  Created on: Apr 5, 2018
 *      Author: andrewerner
 */

#include "VCGRA_Instance.h"
#include <iostream>

namespace cgra {
	static constexpr uint32_t pe_unique_id_start_value{0};
	//!< \brief Start value for unique IDs of ProcessingElements.
	static constexpr uint8_t vCh_second_level{0};
	//!< \brief Index for VirtualChannel of second level
	static constexpr uint8_t vCh_third_level{1};
	//!< \brief Index for VirtualChannel of third level
	static constexpr uint8_t pe_enable_In1{0};
	//!< \brief Index of enable signal for first input of a Processing_Element
	static constexpr uint8_t pe_enable_In2{1};
	//!< \brief Index of enable signal for second input of a Processing_Element
	static constexpr uint8_t num_of_PE{12};
	//!< \brief Number of PEs in VCGRA instance
	static constexpr uint8_t PEs_per_level{4};
	//!< \brief Number of PEs in each level of the VCGRA instance

};

std::ostream& operator<<(std::ostream& os, const cgra::VCGRA& vcgra)
{
	os << vcgra.name() << "\t" << vcgra.kind() << std::endl;

	return os;
}


cgra::VCGRA::VCGRA(const sc_core::sc_module_name& name) : sc_core::sc_module(name)
{

	//Processin Elements:
	//-------------------

	//Create Processing elements of VCGRA
	m_pe_array.init(cgra::num_of_PE,pe_creator(cgra::pe_unique_id_start_value));

	for (uint8_t i, j = 0; 2 * cgra::num_of_PE > j; j += 2, ++i)
	{
		auto& t_pe = m_pe_array.at(i);
		t_pe.in1.bind(m_ch_outputs_signals.at(j));
		t_pe.in2.bind(m_ch_outputs_signals.at(j+1));
		t_pe.enable.at(cgra::pe_enable_In1).bind(m_ch_enable_signals.at(j));
		t_pe.enable.at(cgra::pe_enable_In2).bind(m_ch_enable_signals.at(j+1));
		t_pe.clk.bind(clk);
		t_pe.conf.bind(m_pe_conf_part_signals.at(i));
		t_pe.valid.bind(m_valid_signals.at(i));
		if(2 * cgra::PEs_per_level > i)
			t_pe.res.bind(m_res_signals.at(i));
	}


	//Input Channel:
	//--------------

	//Connect input channel inputs with VCGRA inputs and start signal
	for (auto& v : m_input_ch.valids)
		v.bind(start);
	for (uint8_t i = 0; 8 > i; ++i)
		m_input_ch.channel_inputs.at(i).bind(inputs.at(i));

	//Connect Input VirtualChannel rst-ports and clk-ports
	m_input_ch.rst.bind(rst);
	m_input_ch.clk.bind(clk);

	//Connect input channel enable and output signals
	for (uint8_t i, j = 0; 2 * cgra::PEs_per_level > j; j += 2, ++i)
	{
		m_input_ch.channel_outputs.at(j).bind(m_ch_outputs_signals.at(j));
		m_input_ch.enables.at(j).bind(m_ch_enable_signals.at(j));
		m_input_ch.channel_outputs.at(j+1).bind(m_ch_outputs_signals.at(j+1));
		m_input_ch.enables.at(j+1).bind(m_ch_enable_signals.at(j+1));
	}

	//Connect Configuration of Input VirtualChannel
	m_input_ch.conf.bind(m_input_ch_config_signal);


	//General VirtualChannels:
	//------------------------

	uint8_t t_level_count{1}; // temporary variable to count the number of PE levels for indexing

	//For all VCGRA channels connect...
	for(auto& ch : m_ch_array)
	{
		//Connect VirtualChannel rst-ports and clk-ports
		ch.clk.bind(clk);
		ch.rst.bind(rst);
		ch.conf.bind(m_ch_config_selector.config_parts.at(t_level_count - 1));

		//Connect VirtualChannel input and output signals
		for (uint8_t j = 0, i = 0; 2 * cgra::PEs_per_level > j; j += 2, ++i)
		{
			ch.channel_inputs.at(i).bind(m_res_signals.at((t_level_count - 1) * cgra::PEs_per_level + i));
			ch.valids.at(i).bind(m_valid_signals.at((t_level_count - 1) * cgra::PEs_per_level + i));

			ch.channel_outputs.at(j).bind(m_ch_outputs_signals.at(t_level_count * 2 * cgra::PEs_per_level + j));
			ch.channel_outputs.at(j+1).bind(m_ch_outputs_signals.at(t_level_count * 2 * cgra::PEs_per_level + j + 1));
			ch.enables.at(j).bind(m_ch_enable_signals.at(t_level_count * 2 * cgra::PEs_per_level + j));
			ch.enables.at(j+1).bind(m_ch_enable_signals.at(t_level_count * 2 * cgra::PEs_per_level + j + 1));
		}

		++t_level_count;
	}

	//Configuration Caches:
	//---------------------

	//Connect Configuration Cache control signals
	m_ch_cc.ack.bind(ack_ch_cc);
	m_ch_cc.clk.bind(clk);
	m_ch_cc.dataInStream.bind(ch_config_stream);
	m_ch_cc.slt_in.bind(slct_in_ch_cc);
	m_ch_cc.slt_out.bind(slct_out_ch_cc);
	m_ch_cc.write.bind(write_ch_cc);
	m_pe_cc.ack.bind(ack_pe_cc);
	m_pe_cc.clk.bind(clk);
	m_pe_cc.dataInStream.bind(pe_config_stream);
	m_pe_cc.slt_in.bind(slct_in_pe_cc);
	m_pe_cc.slt_out.bind(slct_out_pe_cc);
	m_pe_cc.write.bind(write_pe_cc);

	//Connect CC of PE's to PE's
	//(Configuration bitstream is separated to parts for each PE)
	m_pe_cc.currentConfig.bind(m_pe_cc_current_config_signal);
	m_pe_config_demux.config_input.bind(m_pe_cc_current_config_signal);

	for (uint32_t i = 0; 12 > i; ++i)
		m_pe_config_demux.config_parts.at(i).bind(m_pe_conf_part_signals.at(i));

	//Connect VirtualChannel configuration cache's current configuration
	m_ch_cc.currentConfig.bind(m_ch_cc_current_config_signal);

	//Connect CC of channels to virtual channels
	//(From CC of channel a range of bits is selected and separated into parts)
	m_in_ch_config_selector.config_input.bind(m_ch_cc_current_config_signal);
	m_in_ch_config_selector.config_parts.at(0).bind(m_input_ch_config_signal);
	m_ch_config_selector.config_input.bind(m_ch_cc_current_config_signal);
	m_ch_config_selector.config_parts.at(cgra::vCh_second_level).bind(m_vCh_config_signals.at(cgra::vCh_second_level));
	m_ch_config_selector.config_parts.at(cgra::vCh_third_level).bind(m_vCh_config_signals.at(cgra::vCh_third_level));

	//Connect CC of channels to Synchronizer
	m_sync_selector.config_input.bind(m_ch_cc_current_config_signal);
	m_sync_selector.config_parts.at(0).bind(m_sync_config_signal);


	//Synchronizer for VCGRA:
	//-----------------------

	//Connect valid signals of last PE level to Synchronizer
	for (uint8_t i = 0; cgra::PEs_per_level > i; ++i)
		m_sync.valid_inputs.at(i).bind(m_valid_signals.at(2 * cgra::PEs_per_level  + i));

	//Connect ports for clocking, configuration and sync' status
	m_sync.conf.bind(m_sync_config_signal);
	m_sync.clk.bind(clk);
	m_sync.ready.bind(ready);

	//Connect outputs of last PE level to VCGRA outputs
	//-------------------------------------------------
	for (uint8_t i = 0; cgra::PEs_per_level > i; ++i)
		m_pe_array.at(2 * PEs_per_level + i).res.bind(results.at(i));

#ifdef DEBUG
	for(auto& pe : m_pe_array)
	{
		std::cout << pe.name() << ".In1: " << pe.in1.bind_count() << std::endl;
		std::cout << pe.name() << ".In2: " << pe.in2.bind_count() << std::endl;
		std::cout << pe.name() << ".res: " << pe.res.bind_count() << std::endl;
		std::cout << pe.name() << ".conf: " << pe.conf.bind_count() << std::endl;
		std::cout << pe.name() << ".clk: " << pe.clk.bind_count() << std::endl;
		std::cout << pe.name() << ".enable1: " << pe.enable[0].bind_count() << std::endl;
		std::cout << pe.name() << ".enable2: " << pe.enable[1].bind_count() << std::endl;
		std::cout << pe.name() << ".valid: " << pe.valid.bind_count() << "\n" <<std::endl;
	}

	{
		uint16_t i = 0;

		for(auto& in : m_input_ch.channel_inputs)
		{
			std::cout << m_input_ch.name() << ".input_";
			std::cout << i++ << ": " << in.bind_count() << std::endl;
		}

		i = 0;
		for(auto& v : m_input_ch.valids)
		{
			std::cout << m_input_ch.name() << ".valid_";
			std::cout << i++ << ": " << v.bind_count() << std::endl;
		}

		i = 0;
		for(auto& en : m_input_ch.enables)
		{
			std::cout << m_input_ch.name() << ".enable_";
			std::cout << i++ << ": " << en.bind_count() << std::endl;
		}

		i = 0;
		for(auto& out : m_input_ch.channel_outputs)
		{
			std::cout << m_input_ch.name() << ".output_";
			std::cout << i++ << ": " << out.bind_count() << std::endl;
		}

		std::cout << m_input_ch.name() << ".conf: " << m_input_ch.conf.bind_count() << std::endl;
		std::cout << m_input_ch.name() << ".clk: " << m_input_ch.conf.bind_count() << "\n" << std::endl;

	}


	for(auto& vCh : m_ch_array)
	{
		uint16_t i = 0;

		for(auto& in : vCh.channel_inputs)
		{
			std::cout << vCh.name() << ".input_";
			std::cout << i++ << ": " << in.bind_count() << std::endl;
		}

		i = 0;
		for(auto& v : vCh.valids)
		{
			std::cout << vCh.name() << ".valid_";
			std::cout << i++ << ": " << v.bind_count() << std::endl;
		}

		i = 0;
		for(auto& en : vCh.enables)
		{
			std::cout << vCh.name() << ".enable_";
			std::cout << i++ << ": " << en.bind_count() << std::endl;
		}

		i = 0;
		for(auto& out : vCh.channel_outputs)
		{
			std::cout << vCh.name() << ".output_";
			std::cout << i++ << ": " << out.bind_count() << std::endl;
		}

		std::cout << vCh.name() << ".conf: " << vCh.conf.bind_count() << std::endl;
		std::cout << vCh.name() << ".clk: " << vCh.conf.bind_count() << "\n" << std::endl;
	}


	std::cout << m_pe_config_demux.config_input.name() << ": " << m_pe_config_demux.config_input.bind_count() << std::endl;
	for(auto& i : m_pe_config_demux.config_parts)
		std::cout << i.name() << ": " << i.bind_count() << std::endl;

#endif

	return;
}

void cgra::VCGRA::dump(std::ostream& os) const
{

	//Topic
	os << "VCGRA structure information\n";
	os << "===========================\n" << std::endl;

	//Input Channel information
	os << "Input channel information\n";
	os << "-------------------------\n";
	m_input_ch.dump(os);
	os << std::endl;

	//Processing Element information
	os << "Processing Element information\n";
	os << "------------------------------\n";
	for(auto& pe : m_pe_array)
	{
		pe.dump(os);
		os << std::endl;
	}

	//General virtual channel information
	os << "Virtual Channel information\n";
	os << "---------------------------\n";
	for(auto& ch : m_ch_array)
	{
		ch.dump(os);
		os << std::endl;
	}

	//PE configuration cache information information
	os << "PE configuration cache information\n";
	os << "----------------------------------\n";
	m_pe_cc.dump(os);
	os << std::endl;

	//Channel configuration cache information information
	os << "Information about demultiplexer for PEs configuration\n";
	os << "-----------------------------------------------------\n";
	m_pe_config_demux.dump(os);
	os << std::endl;

	//Channel configuration cache information information
	os << "channel configuration cache information\n";
	os << "---------------------------------------\n";
	m_ch_cc.dump(os);
	os << std::endl;

	//Channel configuration cache information information
	os << "Information about Selector for input channel configuration\n";
	os << "----------------------------------------------------------\n";
	m_in_ch_config_selector.dump(os);
	os << std::endl;

	//Channel configuration cache information information
	os << "Information about Selector for general channel configuration\n";
	os << "------------------------------------------------------------\n";
	m_ch_config_selector.dump(os);
	os << std::endl;

	//Channel configuration cache information information
	os << "Information about Selector for synchronizer configuration\n";
	os << "------------------------------------------------------------\n";
	m_sync_selector.dump(os);
	os << std::endl;

	return;
}
