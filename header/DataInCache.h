/*
 * DataCache.h
 *
 *  Created on: 03.08.2018
 *      Author: andrewerner
 */

#ifndef HEADER_DATAINCACHE_H_
#define HEADER_DATAINCACHE_H_

#include <systemc>
#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>
#include "Typedef.h"

namespace cgra {

//Forward declaration of DataCache class
template <uint32_t B, uint32_t N, uint32_t L, uint32_t S, uint32_t I>
class DataInCache;

//Definition of input data cache types
static typedef DataInCache<cgra::cDataValueBitwidth,
		cgra::cNumberOfValuesPerCacheLine,
		cgra::cNumberDataInCacheLines
		> data_input_cache_type;
//!< \brief Type definition for the input data cache of a VCGRA

/*!
 * \class DataInCache
 *
 * \brief Templated cache class for data caches at the beginning of a VCGRA
 *
 * \details
 * This is a template of a data input cache for a VCGRA.
 * One can parameterize its properties by the value bitwidth.
 *
 * \tparam B Bitwidth for a single integer value (with respect to VirtualChannel data bitwidth)
 * \tparam N Number of data values in a data cache line
 * \tparam L Number of cache lines
 *
 */
template <uint32_t B, uint32_t N, uint32_t L = 2>
class DataInCache : public sc_core::sc_module
{
public:
	typedef sc_dt::sc_int<B> value_type_t;
	//!< \brief Type for stored configuration data per line
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Clock type
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(L)> select_lines_type_t;
	//!< \brief Type for line selector
	typedef bool write_enable_type_t;
	//!< \brief Type for write enable signal
	typedef sc_dt::sc_lv<B> stream_type_t;
	//!< \brief Type for streaming input data to fill cache line
	typedef bool ack_type_t;
	//!< \brief Acknowledge data type
	typedef sc_dt::sc_lv<cgra::calc_bitwidth(N)> select_value_type_t;
	//!< \brief Select cache place in cache line to store data

	//Entity Ports
	sc_core::sc_in<stream_type_t> dataInStream{"data_in_stream"};
	//!< \brief New configuration data to be stored in a cache line
	sc_core::sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock of configuration cache
	sc_core::sc_in<write_enable_type_t> write{"write_new_value"};
	//!< \brief If a positive edge occurs, datum from data-in-stream is copied into cache line
	sc_core::sc_in<select_lines_type_t> slt_in{"data_in_cache_line"};
	//!< \brief Select cache line to store datum from data-in-stream
	sc_core::sc_in<select_value_type_t> slt_place{"data_place"};
	//!< \brief Select current cache line for current-configuration
	sc_core::sc_in<select_lines_type_t> slt_out{"data_out_cache_line"};
	//!< \brief Select current cache line for current-configuration
	std::array<sc_core::sc_out<value_type_t>, N> currentValues;
	//!< \brief Currently set configuration
	sc_core::sc_out<ack_type_t> ack{"acknowledge"};
	//!< \brief Acknowledges the income of a new data-stream

	//Ctor
	SC_HAS_PROCESS(DataInCache);
	/*!
	 * \brief General Constructor
	 *
	 * \details
	 * The number of configuration bits defines the size of the cache lines.
	 *
	 * \param[in] nameA SystemC module name for DataInCache instance
	 */
	DataInCache(const sc_core::sc_module_name& nameA) :
		sc_core::sc_module(nameA)
	{
		SC_METHOD(storeValueInCacheLine);
		sensitive << clk.pos();
		SC_METHOD(switchCacheLine);
		sensitive << clk.pos();
	}

	virtual ~DataInCache() = default;
	//!< \brief Destructor

	/*!
	 * \brief Initialize output signals of module
	 */
	void end_of_elaboration()
	{
		//Initialize current value output with zero
		for(auto& value : currentValues)
			value.write(0);

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
	void storeValueInCacheLine()
	{
		//Check if selected place in cache line is valid
		if(N <= slt_place.read().to_uint())
			SC_REPORT_WARNING("Cache Warning", "Selected place not in range of cache size");
		else if(L <= slt_in.read().to_uint())
			SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");

		if(write.read() && !ack.read())
		{
			//Check if selected cache line is currently in use
			if(slt_in.read().to_uint() != slt_out.read().to_uint())
			{
				auto& value = m_cacheLines[slt_in.read().to_uint()][slt_place.read().to_uint()];
				value.write(dataInStream.read());
				ack.write(true);
			}
			else
				SC_REPORT_WARNING("Cache Warning", "Selected cache-line currently in use. Value is unchanged");
		}

		if (ack.read() && !write.read())
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
		if(L <= slt_out.read().to_uint())
			SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");

		auto tmp_cacheline = slt_out.read().to_uint();

		//Check if selected cache line is currently used for data input
		if(slt_in.read().to_uint() != tmp_cacheline)
			for(uint32_t idx = 0; N > idx; ++idx)
				currentValues.at(idx).write(m_cacheLines.at(tmp_cacheline).at(idx).read());
		else
			SC_REPORT_WARNING("Cache Warning", "Selected cache line is currently in use. Value is not changed.");

		return;
	}

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override {
		return "Data Input Cache";
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
		os << "Bitwidth serial input:\t\t" << std::setw(3) << static_cast<uint32_t>(N) << std::endl;
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
	DataInCache() = delete;
	DataInCache(const DataInCache& src) = delete;				//!< \brief because sc_module could not be copied
	DataInCache& operator=(const DataInCache& src) = delete;	//!< \brief because move not implemented for sc_module
	DataInCache(DataInCache&& src) = delete;					//!< \brief because sc_module could not be copied
	DataInCache& operator=(DataInCache&& src) = delete;  		//!< \brief because move not implemented for sc_module

	//Private Signals and Buffers
	std::array<std::array<sc_core::sc_buffer<value_type_t>, N>, L> m_cacheLines;
	//!< \brief Two-dimensional array, where 1st. dimension is cache line and 2nd. dimension is value

	const uint32_t m_numOfBytes{cgra::calc_numOfBytes(B * N * L)};
	//!< \brief Cache size in bytes
};

} //End namespace CGRA

#endif /* HEADER_DATAINCACHE_H_ */
