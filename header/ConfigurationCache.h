/*
 * ConfigurationCache.h
 *
 *  Created on: Aug 9, 2017
 *      Author: Andre´ Werner
 */

#ifndef CONFIGURATION_CACHE_H_
#define CONFIGURATION_CACHE_H_

#include <systemc>
#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>
#include "Typedef.h"

namespace cgra {

//Forward declaration of ConfigurationCache template class
template <typename T, uint8_t M, uint8_t L, uint8_t N>
class ConfigurationCache;

//Cache Definitions for VCGRA instance
//------------------------------------
typedef ConfigurationCache<sc_dt::sc_lv<cgra::cPeConfigBitWidth>,
		cgra::cSelectLineBitwidthPeConfCache,
		cgra::cNumberOfPeCacheLines,cgra::cSelectLineBitwidthPeConfCache> pe_config_cache_type_t;
//!< \brief Type for Processing_Element configuration cache
typedef ConfigurationCache<sc_dt::sc_lv<cgra::cVChConfigBitWidth>,
		cgra::cSelectLineBitwidthVChConfCache,
		cgra::cNumberOfVChCacheLines,
		cgra::cBitWidthOfSerialInterfaceVChConfCache> ch_config_cache_type_t;


/*!
 * \class ConfigurationCache
 *
 * \brief Configuration cache for pre-loading
 *
 * \details
 * The number of caches depends on M and is always
 * 2 to the power of M. One can choose the cache line
 * to store a configuration. A configuration is send
 * sequentially if data receiving is enabled and stored
 * in a cache line selected by slt_in. A configuration
 * can be switched by slt_out which chooses one
 * cache line and which linked it to the output.
 * slt_in and slt_out always have to differ. Thus, a current
 * configuration can not be overwritten while it is used.
 * "T" needs to be a data type with a vector which has is an
 * integer multiple of "stream_type_t".
 *
 * \tparam T Type of stored configuration (e.g. bitwidth)
 * \tparam M Bitwidth of select interfaces
 * \tparam L Number of Cache Lines (2 to the power of M)
 * \tparam N Bitwidth of serial configuration input
 */
template <typename T, uint8_t M = 2, uint8_t L = 4, uint8_t N = 8>
class ConfigurationCache : public sc_core::sc_module {
public:
	typedef T config_type_t;
	//!< \brief Type for stored configuration data per line
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Clock type
	typedef sc_dt::sc_lv<M> select_type_t;
	//!< \brief Type for line selector
	typedef bool write_enable_type_t;
	//!< \brief Type for write enable signal
	typedef sc_dt::sc_lv<N> stream_type_t;
	//!< \brief Type for streaming input data to fill cache line
	typedef bool ack_type_t;
	//!< \brief Acknowledge data type

	//Entity Ports
	sc_core::sc_in<stream_type_t> dataInStream{"data_in_stream"};
	//!< \brief New configuration data to be stored in a cache line
	sc_core::sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock of configuration cache
	sc_core::sc_in<write_enable_type_t> write{"write_new_cache_line"};
	//!< \brief If a positive edge occurs, datum from data-in-stream is copied into cache line
	sc_core::sc_in<select_type_t> slt_in{"data_in_cache_line"};
	//!< \brief Select cache line to store datum from data-in-stream
	sc_core::sc_in<select_type_t> slt_out{"data_out_cache_line"};
	//!< \brief Select current cache line for current-configuration
	sc_core::sc_out<config_type_t> currentConfig{"current_configuration"};
	//!< \brief Currently set configuration
	sc_core::sc_out<ack_type_t> ack{"acknowledge"};
	//!< \brief Acknowledges the income of a new data-stream

	//Ctor
	SC_HAS_PROCESS(ConfigurationCache);
	/*!
	 * \brief General Constructor
	 *
	 * \details
	 * The number of configuration bits defines the size of the cache lines.
	 *
	 * \param[in] nameA 				SystemC module name for ConfigurationCache instance
	 * \param[in] configBitstreamLength Number of configuration bits to cache
	 */
	ConfigurationCache(const sc_core::sc_module_name& nameA, uint32_t configBitStreamLength ) :
		sc_core::sc_module(nameA), m_numOfBytes(cgra::calc_numOfBytes(configBitStreamLength))
	{
		SC_METHOD(storeCacheLine);
		sensitive << clk.pos();
		SC_METHOD(switchCacheLine);
		sensitive << clk.pos();
	}


	virtual ~ConfigurationCache() = default;
	//!< \brief Destructor

	/*!
	 * \brief Initialize output signals of module
	 */
	void end_of_elaboration()
	{
		currentConfig.write(0);
		for (uint32_t i = 0; i < L; ++i)
			m_cachelines[i].write(0);
		ack.write(false);
	}

	/*!
	 * \brief Store streamed data within cache line
	 *
	 * \details
	 * Function raises a warning, if a cache line should be
	 * written which is currently used as the output.
	 * The configuration line won't be changed.
	 */
	void storeCacheLine()
	{
		if(write.read() && !ack.read())
		{
			if(slt_in.read().to_uint() != slt_out.read().to_uint())
			{
				auto tmp_cacheline = m_cachelines[slt_in.read().to_uint()].read();
				tmp_cacheline = tmp_cacheline.lrotate(N);
				tmp_cacheline |= dataInStream.read();
				m_cachelines[slt_in.read().to_uint()].write(tmp_cacheline);
				ack.write(true);
			}
			else
				SC_REPORT_WARNING("Configuration Warning", "Selected cache-line currently in use. Configuration is unchanged");
		}

		if (ack.read() && !write.read())
			ack.write(false);
	}

	/*!
	 * \brief Switch current configuration cache line
	 */
	void switchCacheLine()
	{
		auto tmp_cacheline = slt_out.read().to_uint();

		if(slt_in.read().to_uint() != tmp_cacheline)
			currentConfig.write(m_cachelines[tmp_cacheline].read());
	}

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override {
		return "Configuration Cache";
	}

	/*!
	 * \brief Print configuration cache name
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
		os << "Selected output cache line:\t\t" << std::setw(3) << slt_out.read().to_string() << std::endl;

		os << "Cache content\n";
		os << "=============\n";

		uint32_t line_iter = 1; //Running index for cache line
		for(auto& line : m_cachelines)
		{
			os << line_iter++ << std::setw(3) <<":\t";
			line.print(os);
			os << "\n";
		}
	}

	/*!
	 * \brief Print content of a cache line
	 *
	 * \param[in] 	line 	Select cache line to print
	 * \param[out] 	os 		Select out stream to write (default std::cout)
	 */
	void print_cache_line(uint32_t line, std::ostream& os = std::cout) const
	{
		sc_assert(L > line);
		m_cachelines.at(line).print(os);
	}

	/*!
	 * \brief Return the size of a cache line
	 */
	uint16_t size() const { return m_numOfBytes; }

	/*!
	 * \brief Return number of cache lines
	 */
	uint8_t cache_size() const { return m_cachelines.size(); }

private:
	std::array<sc_core::sc_buffer<config_type_t>, L> m_cachelines;
	//!< \brief Cache lines to store a configuration
	uint16_t m_numOfBytes;
	//!< \brief Size of the cache in bytes

	//Forbidden constructors
	ConfigurationCache() = delete;
	ConfigurationCache(const ConfigurationCache& src) = delete;
	ConfigurationCache& operator=(const ConfigurationCache& src) = delete;
	ConfigurationCache(ConfigurationCache&& src) = delete;
	ConfigurationCache& operator=(ConfigurationCache&& src) = delete;
};


} // namespace cgra


#endif /* CONFIGURATION_CACHE_H_ */
