/*
 * TB_DataInCache.h
 *
 * Created on: Oct 31, 2018
 * Author: andrewerner
 */

#ifndef HEADER_TB_DATAINCACHE_H_
#define HEADER_TB_DATAINCACHE_H_

#include <systemc>
#include <array>
#include "Typedef.h"


namespace cgra {

/*!
 * \class TB_DataInCache
 *
 * \brief Test bench for a VCGRA's data input cache
 *
 * \details
 *
 */
class TB_DataOutCache : public sc_core::sc_module
{
public:
	typedef sc_dt::sc_int<16> value_type_t;
	//!< \brief Type for stored value datum per place in a cache line
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Clock type
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(2)> select_lines_type_t;
	//!< \brief Type for line selector
	typedef cgra::cache_load_type_t load_enable_type_t;
	//!< \brief Type for load signal
	typedef sc_dt::sc_lv<16> stream_type_t;
	//!< \brief Type for streaming a value to shared memory
	typedef cgra::cache_ack_type_t ack_type_t;
	//!< \brief Acknowledge data type
	typedef bool update_type_t;
	//!< \brief Data type for update of input values
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(4)> select_value_type_t;
	//!< \brief Select cache place in cache line to load data

	//Entity Ports
	std::array<sc_core::sc_out<value_type_t>, 4> sCurrentResults;
	//!< \brief Last recent results from last PE level
	sc_core::sc_out<update_type_t> sUpdate{"update"};
	//!< \brief Rising edge stores new data in selected input cache line
	sc_core::sc_in<clock_type_t> sClk{"clk"};
	//!< \brief Clock of data cache
	sc_core::sc_out<load_enable_type_t> sLoad{"load_new_value"};
	//!< \brief If a positive edge occurs, datum from data-out-stream is copied into shared memory location
	sc_core::sc_out<select_lines_type_t> sSlt_in{"data_in_cache_line"};
	//!< \brief Select cache line to store datum from data-out-stream
	sc_core::sc_out<select_value_type_t> sSlt_place{"data_place"};
	//!< \brief Select current cache place for data out stream
	sc_core::sc_out<select_lines_type_t> sSlt_out{"data_out_cache_line"};
	//!< \brief Select current cache line for data out stream.
	sc_core::sc_in<stream_type_t> sDataOutStream{"data_out_stream"};
	//!< \brief Data stream to shared memory
	sc_core::sc_in<ack_type_t> sAck{"acknowledge"};
	//!< \brief Acknowledges the request of a new data to stream

	//Ctor
	SC_HAS_PROCESS(TB_DataOutCache);
	TB_DataOutCache(sc_core::sc_module_name nameA);

	/*!
	 * \brief Stimulus function to test a data input cache instance
	 *
	 * \details
	 *
	 */
	void stimuli(void);


	/*!
	 * \brief Initialize output signals of module
	 */
	void end_of_elaboration() override;

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override {
		return "Testbench for a VCGRA's data output cache.";
	}

	/*!
	 * \brief Print name of test bench for a VCGRA's data input cache
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void print(std::ostream& os = std::cout) const override
	{
		os << name();
	}

	/*!
	 * \brief Dump configuration cache information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override;

	//Destructor
	~TB_DataOutCache() = default;

}; /* end class TB_DataInCache */

} /* end namespace cgra */

#endif  /*  HEADER_TB_DATAINCACHE_H_ */
