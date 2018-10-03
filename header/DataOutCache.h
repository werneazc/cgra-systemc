/*
 * DataOutCache.h
 *
 *  Created on: 03.08.2018
 *      Author: andrewerner
 */

#ifndef HEADER_DATAOUTCACHE_H_
#define HEADER_DATAOUTCACHE_H_

#include <systemc>
#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>
#include "Typedef.h"

namespace cgra {

//Forward declaration of DataCache class
template <uint32_t B, uint32_t N, uint32_t L>
class DataOutCache;

//Definition of output data cache types
static typedef DataOutCache<cgra::cDataValueBitwidth,
		cgra::cNumberOfValuesPerCacheLine,
		cgra::cNumberDataInCacheLines
		> data_output_cache_type_t;
//!< \brief Type definition for the input data cache of a VCGRA

/*!
 * \class DataOutCache
 *
 * \brief Templated cache class for data caches at the end of a VCGRA
 *
 * \details
 * This is a template of a data output cache for a VCGRA.
 * One can parameterize its properties by the value bitwidth,
 * the number of values per cache line and the number of
 * cache lines itself.
 *
 * The input is stored in parallel into the current cache line.
 * The number of input ports depends on the number of PEs in the
 * last PE level in the VCGRA instance. A value is transmitted into
 * shared memory via a serial interface; speed: one value per clock cycle.
 *
 * \tparam B Bitwidth for a single integer value (with respect to VirtualChannel data bitwidth)
 * \tparam N Number of data values in a data cache line
 * \tparam L Number of cache lines
 *
 */
template <uint32_t B, uint32_t N, uint32_t L = 2>
class DataOutCache : public sc_core::sc_module
{
public:
	typedef sc_dt::sc_int<B> value_type_t;
	//!< \brief Type for stored value in a cache line place
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Clock type
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(L)> select_lines_type_t;
	//!< \brief Type for line selector
	typedef cgra::cache_load_type_t load_enable_type_t;
	//!< \brief Type for load signal
	typedef sc_dt::sc_lv<B> stream_type_t;
	//!< \brief Type for streaming a value to shared memory
	typedef cgra::cache_ack_type_t ack_type_t;
	//!< \brief Acknowledge data type
	typedef bool update_type_t;
	//!< \brief Data type for update of input values
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(N)> select_value_type_t;
	//!< \brief Select cache place in cache line to process data

	//Entity Ports
	std::array<sc_core::sc_in<value_type_t>, N> currentResults;
	//!< \brief Last recent results from last PE level
	sc_core::sc_in<update_type_t> update{"update"};
	//!< \brief Rising edge stores new data in selected input cache line
	sc_core::sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock of data cache
	sc_core::sc_in<load_enable_type_t> load{"load_new_value"};
	//!< \brief If a positive edge occurs, datum from data-out-stream is copied into shared memory location
	sc_core::sc_in<select_lines_type_t> slt_in{"data_in_cache_line"};
	//!< \brief Select cache line to store datum from data-out-stream
	sc_core::sc_in<select_value_type_t> slt_place{"data_place"};
	//!< \brief Select current cache place for data out stream
	sc_core::sc_in<select_lines_type_t> slt_out{"data_out_cache_line"};
	//!< \brief Select current cache line for data out stream.
	sc_core::sc_out<stream_type_t> dataOutStream{"data_out_stream"};
	//!< \brief Data stream to shared memory
	sc_core::sc_out<ack_type_t> ack{"acknowledge"};
	//!< \brief Acknowledges the request of a new data to stream

	//Ctor
	SC_HAS_PROCESS(DataOutCache);
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA SystemC module name for DataOutCache instance
	 */
	DataOutCache(const sc_core::sc_module_name& nameA) :
		sc_core::sc_module(nameA)
	{
		SC_METHOD(loadValueFromCacheLine);
		sensitive << clk.pos();
		SC_METHOD(switchCacheLine);
		sensitive << clk.pos();
		SC_METHOD(updateCacheLine);
		sensitive << update.pos();
	}

	virtual ~DataOutCache() = default;
	//!< \brief Destructor

	/*!
	 * \brief Initialize output signals of module
	 */
	void end_of_elaboration()
	{
		//Initialize data output stream
		dataOutStream.write(0);

		//Initialize all buffers with zero
		for(auto& line : m_cacheLines)
			for(auto& value : line)
				value.write(0);

		ack.write(false);
	}

	/*!
	 * \brief Store streamed data specified within cache line place
	 *
	 * \details
	 * Function raises a warning, if a cache line should be
	 * written which is currently used as the output.
	 * The configuration line won't be changed.
	 */
	void loadValueFromCacheLine()
	{
		//Check if selected place in cache line is valid
		if(N <= slt_place.read().to_uint())
			SC_REPORT_WARNING("Cache Warning", "Selected place not in range of cache size");
		else if(L <= slt_in.read().to_uint())
			SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");

		if(update.read())
		{
			SC_REPORT_WARNING("Cache Warning", "Selected cache line updated. Value not loaded.");
			return;
		}

		if(load.read() && !ack.read())
		{
			//Check if selected cache line is currently in use
			auto& value = m_cacheLines[slt_out.read().to_uint()][slt_place.read().to_uint()];
			dataOutStream.write(value.read());
			ack.write(true);
		}

		if (ack.read() && !load.read())
			ack.write(false);

		return;
	}

	/*!
	 * \brief Switch current value cache line
	 *
	 * \details
	 * Function raises a warning, if a cache line should be
	 * written which is not reachable (address error).
	 * The cache line won't be selected.
	 */
	void switchCacheLine()
	{
		//Check if selected place in cache line is valid
		if(L <= slt_in.read().to_uint())
			SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");

		auto tmp_cacheline = slt_in.read().to_uint();

		//Check if selected cache line is currently used for data import/export
		if(!load.read() && !update.read())
			m_currentCachline = &m_currentCachline[tmp_cacheline];

		else
			SC_REPORT_WARNING("Cache Warning", "Selected cache line is currently in use. Cache line is not changed.");

		return;
	}

	/*!
	 * \brief Store recent results in selected buffer
	 *
	 * \details
	 * All values currently available at the output ports of
	 * the last PE line are stored in the current selected
	 * input cache line.
	 */
	void updateCacheLine()
	{

		//If positive edge update selected buffer with recent results at input port
		if(update.read())
			for(uint32_t idx = 0; N > idx; ++idx)
				*(m_currentCachline + idx).write(currentResults.at(idx).read());

		return;
	}

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override {
		return "Data Output Cache";
	}

	/*!
	 * \brief Print data input cache name
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
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << "\t\t" << kind() << std::endl;
		os << "Number of cache lines:\t\t" << std::setw(3) << static_cast<uint32_t>(L) << std::endl;
		os << "Cache line length:\t\t" << std::setw(3) << static_cast<uint32_t>(size()) << std::endl;
		os << "Bitwidth serial output:\t\t" << std::setw(3) << static_cast<uint32_t>(N) << std::endl;
		os << "Selected input cache line:\t\t" << std::setw(3) << slt_in.read().to_string() << std::endl;
		os << "Selected cache line place:\t\t" << std::setw(3) << slt_place.read().to_string() << std::endl;
		os << "Selected output cache line:\t\t" << std::setw(3) << slt_out.read().to_string() << std::endl;

		os << "Cache content\n";
		os << "=============\n";

		uint32_t line_iter = 1; //Running index for cache line
		for(auto& line : m_cacheLines)
		{

			os << std::setw(3) << line_iter++ <<":\t";
			uint32_t value_iter = 1;
			for(auto& value : line)
			{
				os << value_iter++ << ": ";
				value.print(os);
				os << ', ';
			}
			os << "\n";
		}
	}

	/*!
	 * \brief Print content of a cache line
	 *
	 * \param[in] 	line 	Select cache line to print
	 * \param[out] 	os 		Select outstream to write [default: std::cout]
	 */
	void print_cache_line(uint32_t line, std::ostream& os = std::cout) const
	{
		//Check if selected place in cache line is valid
		if(L <= line)
			SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");

		uint32_t value_iter = 1;
		for(auto& value : m_cacheLines.at(line))
		{
			os << value_iter++ << ": ";
			value.print(os);
			os << ', ';
		}
		os << "\n";
	}

	/*!
	 * \brief Return the size of a cache line
	 */
	uint16_t size() const { return m_numOfBytes; }

	/*!
	 * \brief Return number of cache lines
	 */
	uint8_t cache_size() const { return m_cacheLines.size(); }

private:
	//Forbidden Constructors
	DataOutCache() = delete;
	DataOutCache(const DataOutCache& src) = delete;				//!< \brief because sc_module could not be copied
	DataOutCache& operator=(const DataOutCache& src) = delete;	//!< \brief because move not implemented for sc_module
	DataOutCache(DataOutCache&& src) = delete;					//!< \brief because sc_module could not be copied
	DataOutCache& operator=(DataOutCache&& src) = delete;  		//!< \brief because move not implemented for sc_module

	//Private Signals and Buffers
	std::array<std::array<sc_core::sc_buffer<value_type_t>, N>, L> m_cacheLines;
	//!< \brief Two-dimensional array, where 1st. dimension is cache line and 2nd. dimension is value
	std::array<sc_core::sc_buffer<value_type_t>, N>* m_currentCachline{nullptr};
	//!< \brief Current cache line for new input values (slt_in)
	const uint32_t m_numOfBytes{cgra::calc_numOfBytes(B * N)};
	//!< \brief Cache line size in bytes
};

} //End namespace CGRA

#endif /* HEADER_DATAOUTCACHE_H_ */
