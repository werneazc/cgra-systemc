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
};

std::ostream& operator<<(std::ostream& os, const cgra::VCGRA& vcgra)
{
	os << vcgra.name() << "\t" << vcgra.kind() << std::endl;

	return os;
}


cgra::VCGRA::VCGRA(const sc_core::sc_module_name& name) : sc_core::sc_module(name)
{
	//Create Processing elements of VCGRA
	m_pe_array.init(12,pe_creator(cgra::pe_unique_id_start_value));

	//Connect input channel inputs with VCGRA inputs
	for (auto& v : m_input_ch.valids)
		v.bind(start);
	for (uint8_t i = 0; 8 > i; ++i)
		m_input_ch.channel_inputs.at(i).bind(inputs.at(i));

	//Connect VirtualChannel rst-ports and clk-ports
	m_input_ch.rst.bind(rst);
	m_input_ch.clk.bind(clk);
	m_ch_array[cgra::vCh_second_level].rst.bind(rst);
	m_ch_array[cgra::vCh_second_level].clk.bind(clk);
	m_ch_array[cgra::vCh_third_level].rst.bind(rst);
	m_ch_array[cgra::vCh_third_level].clk.bind(clk);

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

	//Connect input_channel outputs to its succeeding PEs
	//and connect PEs outputs to first general channel (first level)
	for (uint8_t i, j = 0; 8 > j; j += 2, ++i)
	{
		auto& t_pe = m_pe_array.at(i);
		t_pe.in1.bind(m_ch_outputs_signals.at(j));
		m_input_ch.channel_outputs.at(j).bind(m_ch_outputs_signals.at(j));
		t_pe.in2.bind(m_ch_outputs_signals.at(j+1));
		m_input_ch.channel_outputs.at(j+1).bind(m_ch_outputs_signals.at(j+1));
		t_pe.enable.at(0).bind(m_ch_enable_signals.at(j));
		m_input_ch.enables.at(j).bind(m_ch_enable_signals.at(j));
		t_pe.enable.at(1).bind(m_ch_enable_signals.at(j+1));
		m_input_ch.enables.at(j+1).bind(m_ch_enable_signals.at(j+1));
		t_pe.clk.bind(clk);
	}


	//Connect second and third level
	//Connect channel outputs to succeeding PEs
	{
		uint32_t i = 4;
		for (auto& c : m_ch_array)
		{
			for (uint8_t j = 0; 8 > j; j += 2)
			{
				auto& t_pe = m_pe_array.at(i++);
				t_pe.in1.bind(m_ch_outputs_signals.at(8+j));
				c.channel_outputs.at(j).bind(m_ch_outputs_signals.at(8+j));
				t_pe.in2.bind(m_ch_outputs_signals.at(8+j+1));
				c.channel_outputs.at(j+1).bind(m_ch_outputs_signals.at(8+j+1));
				t_pe.enable.at(0).bind(m_ch_enable_signals.at(8+j));
				c.enables.at(j).bind(m_ch_enable_signals.at(8+j));
				t_pe.enable.at(1).bind(m_ch_enable_signals.at(8+j+1));
				c.enables.at(j+1).bind(m_ch_enable_signals.at(8+j+1));
				t_pe.clk.bind(clk);
			}
		}
	}

	//Connect channel inputs from preceding PEs
	for (uint8_t j = 0, c = 0; 2 > j; ++j)
		for (uint8_t i = 0; 4 > i; ++i)
		{
			m_ch_array.at(j).channel_inputs.at(i).bind(m_res_signals.at(j * 4 + i));
			m_pe_array.at(c).res.bind(m_res_signals.at(j * 4 + i));
			m_ch_array.at(j).valids.at(i).bind(m_valid_signals.at(j * 4 + i));
			m_pe_array.at(c++).valid.bind(m_valid_signals.at(j * 4 + i));
		}

	//Connect outputs of last PE level to VCGRA outputs
	for (uint8_t i = 0; 4 > i; ++i)
	{
		auto& t_pe = m_pe_array.at(8 + i);
		t_pe.res.bind(m_res_signals.at(8 + i));
		results.at(i).bind(m_res_signals.at(8 + i));
		t_pe.valid.bind(m_valid_signals.at(8 + i));
		m_sync.valid_inputs.at(i).bind(m_valid_signals.at(8 + i));
	}

	//Connect CC of PE's to PE's
	//(Configuration bitstream is separated to parts for each PE)
	m_pe_cc.currentConfig.bind(m_pe_cc_current_config_signal);
	m_pe_config_demux.config_input.bind(m_pe_cc_current_config_signal);

	for (uint32_t i = 0; 12 > i; ++i)
	{
		m_pe_array.at(i).conf.bind(m_pe_conf_part_signals.at(i));
		m_pe_config_demux.config_parts.at(i).bind(m_pe_conf_part_signals.at(i));
	}

	//Connect VirtualChannel configuration cache's current configuration
	m_ch_cc.currentConfig.bind(m_ch_cc_current_config_signal);

	//Connect CC of channels to virtual channels
	//(From CC of channel a range of bits is selected and separated into parts)
	m_in_ch_config_selector.config_input.bind(m_ch_cc_current_config_signal);
	m_input_ch.conf.bind(m_in_ch_config_selector.config_parts.at(0));
	m_ch_config_selector.config_input.bind(m_ch_cc_current_config_signal);
	m_ch_array.at(cgra::vCh_second_level).conf.bind(m_ch_config_selector.config_parts.at(0));
	m_ch_array.at(cgra::vCh_third_level).conf.bind(m_ch_config_selector.config_parts.at(1));

	//Connect CC of channels to Synchronizer
	m_sync_selector.config_input.bind(m_ch_cc_current_config_signal);
	m_sync.conf.bind(m_sync_config_signal);
	m_sync_selector.config_parts.at(0).bind(m_sync_config_signal);

	//Connect ports for clocking and sync' status
	m_sync.clk.bind(clk);
	m_sync.ready.bind(ready);

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

	//PE configuration cache information information
	os << "channel configuration cache information\n";
	os << "---------------------------------------\n";
	m_ch_cc.dump(os);
	os << std::endl;

	return;
}
