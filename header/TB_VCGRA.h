/*
 * TestBench_VCGRA.h
 *
 *  Created on: Mai 1, 2018
 *      Author: andrewerner
 */

#ifndef TESTBENCH_VCGRA_H_
#define TESTBENCH_VCGRA_H_

#include <cstdint>
#include <systemc>
#include <array>
#include "VCGRA_Instance.h"

namespace cgra {

/************************************************************************/
// VCGRA TestBench
//!
//! \class TestBench_VCGRA
//! \brief A VCGRA test bench
//!
//! \details
//!	Test bench for the VCGRA example which consists of 12 ProcessingElements
//! and 3 VirtualChannels.
//!
/************************************************************************/
class TestBench_VCGRA : public sc_core::sc_module
{
public:

	//VCGRA interfaces
	//================

	//Control:
	//--------
	sc_core::sc_out<VCGRA::start_type_t> start{"start"};
	//!< \brief Start input
	sc_core::sc_in<VCGRA::clock_type_t> clk{"clock"};
	//!< \brief Clock input
	sc_core::sc_out<VCGRA::reset_type_t> rst{"reset"};
	//!< \brief Reset input
	sc_core::sc_in<VCGRA::ready_type_t> ready{"ready"};
	//!< \brief Ready signal of last PE level
	sc_core::sc_out<VCGRA::pe_config_cache_type_t::write_enable_type_t> write_pe_cc{"write_pe_cc"};
	//!< \brief Write enable for ProcessingElements ConfigurationCache
	sc_core::sc_out<VCGRA::pe_config_cache_type_t::select_type_t> slct_in_pe_cc{"slct_in_pe_cc"};
	//!< \brief Select port for input cache line for ProcessingElements ConfigurationCache
	sc_core::sc_out<VCGRA::pe_config_cache_type_t::select_type_t> slct_out_pe_cc{"slct_out_pe_cc"};
	//!< \brief Select port for output cache line for ProcessingElements ConfigurationCache
	sc_core::sc_in<VCGRA::pe_config_cache_type_t::ack_type_t> ack_pe_cc{"ack_pe_cc"};
	//!< \brief Acknowledge output for ProcessingElements ConfigurationCache
	sc_core::sc_out<VCGRA::ch_config_cache_type_t::write_enable_type_t> write_ch_cc{"write_ch_cc"};
	//!< \brief Write enable for VirtualChannel ConfigurationCache
	sc_core::sc_out<VCGRA::ch_config_cache_type_t::select_type_t> slct_in_ch_cc{"slct_in_ch_cc"};
	//!< \brief Select port for input cache line for VirtualChannel ConfigurationCache
	sc_core::sc_out<VCGRA::ch_config_cache_type_t::select_type_t> slct_out_ch_cc{"slct_out_ch_cc"};
	//!< \brief Select port for output cache line for VirtualChannel ConfigurationCache
	sc_core::sc_in<VCGRA::ch_config_cache_type_t::ack_type_t> ack_ch_cc{"ack_ch_cc"};
	//!< \brief Acknowledge output for VirtualChannel ConfigurationCache

	//Data:
	//-----
	std::array<sc_core::sc_out<VCGRA::input_type_t>,8> inputs;
	//!< \brief input signal vector of a VCGRA
	std::array<sc_core::sc_in<VCGRA::output_type_t>,4> results;
	//!< \brief result signal vector of last PE level
	sc_core::sc_out<VCGRA::pe_config_cache_type_t::stream_type_t> pe_config_stream{"pe_config_stream"};
	//!< \brief result signal vector of last PE level
	sc_core::sc_out<VCGRA::ch_config_cache_type_t::stream_type_t> ch_config_stream{"ch_config_stream"};
	//!< \brief result signal vector of last PE level

	//Contructor:
	SC_HAS_PROCESS(TestBench_VCGRA);
	/*!
	 * \brief Constructor of the test bench
	 *
	 * \param[in] nameA VCGRA test bench module name
	 */
	TestBench_VCGRA(const sc_core::sc_module_name& nameA);

	//Destuctor:
	/*!
	 * \brief Destructor
	 *
	 * \details
	 * Default destructor used.
	 */
	virtual ~TestBench_VCGRA() = default;

	/*!
	 * \brief Process to stimulate the VCGRA
	 *
	 * \details
	 * #TODO: What happens need to be defined later.
	 */
	void stimuli(void);

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "VCGRA TB";
	}

	/*!
	 * \brief Write current signals to output
	 *
	 * \param[out] os Output stream to write information to
	 * \param[in]  vcgra Constant reference to a VCGRA instance
	 */
	friend std::ostream& operator<<(std::ostream& os, const TestBench_VCGRA& vcgra);

	/*!
	 * \brief Dump current status of test bench ports
	 *
	 * \param[out] os out-stream where to write to (default: std::cout)
	 */
	virtual void dump(std::ostream& os = ::std::cout) const override;

	/*!
	 * \brief Initialize signals before first run
	 */
	virtual void end_of_elaboration() override;

private:

	//forbidden constructor
	TestBench_VCGRA() = delete;
	TestBench_VCGRA(const TestBench_VCGRA &source) = delete;			  	//!< \brief because sc_module could not be copied
	TestBench_VCGRA(TestBench_VCGRA &&source) = delete;				  		//!< \brief because move not implemented for sc_module
	TestBench_VCGRA &operator=(const TestBench_VCGRA &source) = delete; 	//!< \brief because sc_module could not be copied
	TestBench_VCGRA &operator=(TestBench_VCGRA &&source) = delete;	  		//!< \brief because move not implemented for sc_module
};

} // end namespace cgra

#endif //TESTBENCH_VCGRA_H_
