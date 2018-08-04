/*
 * TestBenchCC.h
 *
 * Created on: Aug 17, 2017
 * Author: andrewerner
 */

#ifndef HEADER_TESTBENCHCC_H_
#define HEADER_TESTBENCHCC_H_

#include <systemc.h>
#include <iostream>
#include "ConfigurationCache.h"
#include "VirtualChannel.h"

namespace cgra {

/*!
 * \class TestBench_CC
 *
 * \brief Test bench for ConfigurationCache module
 *
 * \details
 * A configuration line is pre-loaded. Afterwards the configuration is switched during one clock cycle.
 */
class TestBench_CC: public sc_core::sc_module {
private:
	typedef cgra::ConfigurationCache<cgra::VirtualChannel<4, 8, 8, 8, 2>::conf_type_t, 2, 4> cache_type_t;
public:

	//Entity Ports
	sc_core::sc_in<cache_type_t::config_type_t> r_current_configuration{"current_configuration"};
	//!< \brief Input for current configuration
	sc_core::sc_out<cache_type_t::select_type_t> s_slt_in{"select_cache_input"};
	//!< \brief Select Input Cache Line data stream saving
	sc_core::sc_out<cache_type_t::select_type_t> s_slt_out{"select_cache_out"};
	//!< \brief Select Output Cache Line for configuration data
	sc_core::sc_out<cache_type_t::stream_type_t> s_stream_data_out{"stream_data_out"};
	//!< \brief Data stream for configuration data
	sc_core::sc_out<cache_type_t::write_enable_type_t> s_write_enable{"write_enable"};
	//!< \brief Write enable to store new configurations
	sc_core::sc_in<cache_type_t::clock_type_t> clk{"clk"};
	//!< \brief Clock
	sc_core::sc_in<cache_type_t::ack_type_t> r_ack{"acknowledge"};
	//!< \brief Synchronization for data stream exchange

	SC_HAS_PROCESS(TestBench_CC);
	/*!
	 * \brief Named constructor
	 *
	 * \param[in] 	nameA 	Name of the test bench module
	 */
	TestBench_CC(const sc_core::sc_module_name& nameA);

	/*!
	 * \brief Dtor
	 */
	virtual ~TestBench_CC() = default;

	/*!
	 * \brief Return kind of configuration module
	 */
	virtual const char* kind() const override
	{ return "TestBench for Configuration Cache"; }

	/*!
	 * \brief Return information dump about SystemC module
	 *
	 * \param[out] os Reference to output stream (default std::cout)
	 */
	virtual void dump(std::ostream& os = std::cout) const override;

	/*!
	 * \brief Create stimulus signals for ConfigurationCache module
	 */
	void stimuli();

	/*!
	 * \brief initialize outputs
	 */
	virtual void end_of_elaboration() override;

private:
	//Forbidden Constructors
	TestBench_CC() = delete;
	TestBench_CC(const TestBench_CC& src) = delete;				//!< \brief because sc_module could not be copied
	TestBench_CC& operator=(const TestBench_CC& src) = delete;	//!< \brief because move not implemented for sc_module
	TestBench_CC(TestBench_CC&& src) = delete;					//!< \brief because sc_module could not be copied
	TestBench_CC& operator=(TestBench_CC&& src) = delete;		//!< \brief because move not implemented for sc_module
};

} /* namespace cgra */

#endif /* HEADER_TESTBENCHCC_H_ */
