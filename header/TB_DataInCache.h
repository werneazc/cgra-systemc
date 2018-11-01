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
 * The TB will load 8 values into both cache lines and
 * each cache line is loaded to current value output.
 * Every value in a cache place is a double from the
 * previous cache line.
 *
 */
class TB_DataInCache : public sc_core::sc_module
{
public:
	typedef sc_dt::sc_int<16> value_type_t;
	//!< \brief Type for stored value datum per place in a cache line
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Clock type
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(2)> select_lines_type_t;
	//!< \brief Type for line selector
	typedef cgra::cache_write_type_t write_enable_type_t;
	//!< \brief Type for write enable signal
	typedef sc_dt::sc_lv<16> stream_type_t;
	//!< \brief Type for streaming input data to fill cache line
	typedef cgra::cache_ack_type_t ack_type_t;
	//!< \brief Acknowledge data type
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(4)> select_value_type_t;
	//!< \brief Select cache place in cache line to store data

	//Entity Ports
	sc_core::sc_out<stream_type_t> sDataInStream{"data_in_stream"};
	//!< \brief New data value to be stored in a cache line
	sc_core::sc_in<clock_type_t> sClk{"clk"};
	//!< \brief Clock of data input cache
	sc_core::sc_out<write_enable_type_t> sWrite{"write_new_value"};
	//!< \brief If a positive edge occurs, datum from data-in-stream is copied into cache line
	sc_core::sc_out<select_lines_type_t> sSlt_in{"data_in_cache_line"};
	//!< \brief Select cache line to store datum from data-in-stream
	sc_core::sc_out<select_value_type_t> sSlt_place{"data_place"};
	//!< \brief Select current cache place for to store value at data in stream
	sc_core::sc_out<select_lines_type_t> sSlt_out{"data_out_cache_line"};
	//!< \brief Select current cache line for current-data set
	std::array<sc_core::sc_in<value_type_t>, 4> sCurrentValues;
	//!< \brief Currently set values to process
	sc_core::sc_in<ack_type_t> sAck{"acknowledge"};
	//!< \brief Acknowledges the income of a new data-stream

	//Ctor
	SC_HAS_PROCESS(TB_DataInCache);
	TB_DataInCache(sc_core::sc_module_name nameA);

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
		return "Testbench for a VCGRA's data input cache.";
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
	~TB_DataInCache() = default;

}; /* end class TB_DataInCache */

} /* end namespace cgra */

#endif  /*  HEADER_TB_DATAINCACHE_H_ */
