/*
 * DataInCache.h
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

#ifdef MCPAT
#include "McPatCacheAccessCounter.hpp"
#endif

namespace cgra {

//Forward declaration of DataCache class
template <uint32_t B, uint32_t N, uint32_t L>
class DataInCache;

//Definition of input data cache types
typedef DataInCache<cgra::cDataValueBitwidth,
        2 * cgra::cPeLevels.front(),
        cgra::cNumberDataInCacheLines
        > data_input_cache_type_t;
//!< \brief Type definition for the input data cache of a VCGRA

/*!
 * \class DataInCache
 *
 * \brief Templated cache class for data caches at the beginning of a VCGRA
 *
 * \details
 * This is a template of a data input cache for a VCGRA.
 * One can parameterize its properties by the value bitwidth,
 * the number of values per cache line and the number of
 * cache lines itself.
 *
 * The bitwidth of the serial interface to load new data into
 * a cache line corresponds to the bitwidth of one integer value.
 * The selected cache line is linked in parallel to the first virtual
 * channel.
 *
 * \tparam B Bitwidth for a single integer value (with respect to VirtualChannel data bitwidth)
 * \tparam N Number of data values in a data cache line
 * \tparam L Number of cache lines
 *
 */
template <uint32_t B, uint32_t N, uint32_t L = 2>
class DataInCache : public sc_core::sc_module
#ifdef MCPAT
    ,                protected cgra::McPatCacheAccessCounter
#endif
{
public:
    typedef sc_dt::sc_int<B> value_type_t;
    //!< \brief Type for stored value datum per place in a cache line
    typedef cgra::clock_type_t clock_type_t;
    //!< \brief Clock type
    typedef sc_dt::sc_lv<cgra::calc_bitwidth(L)> select_lines_type_t;
    //!< \brief Type for line selector
    typedef cgra::cache_write_type_t write_enable_type_t;
    //!< \brief Type for write enable signal
    typedef sc_dt::sc_lv<B> stream_type_t;
    //!< \brief Type for streaming input data to fill cache line
    typedef cgra::cache_ack_type_t ack_type_t;
    //!< \brief Acknowledge data type
    typedef sc_dt::sc_uint<cgra::calc_bitwidth(cgra::cMaxNumberOfValuesPerCacheLine)> select_value_type_t;
    //!< \brief Select cache place in cache line to store data

#ifndef GSYSC
    //Entity Ports
    sc_core::sc_in<stream_type_t> dataInStream{"data_in_stream"};
    //!< \brief New data value to be stored in a cache line
    sc_core::sc_in<clock_type_t> clk{"clk"};
    //!< \brief Clock of data input cache
    sc_core::sc_in<write_enable_type_t> write{"write_new_value"};
    //!< \brief If a positive edge occurs, datum from data-in-stream is copied into cache line
    sc_core::sc_in<select_lines_type_t> slt_in{"data_in_cache_line"};
    //!< \brief Select cache line to store datum from data-in-stream
    sc_core::sc_in<select_value_type_t> slt_place{"data_place"};
    //!< \brief Select current cache place for to store value at data in stream
    sc_core::sc_in<select_lines_type_t> slt_out{"data_out_cache_line"};
    //!< \brief Select current cache line for current-data set
    std::array<sc_core::sc_out<value_type_t>, N> currentValues;
    //!< \brief Currently set values to process
    sc_core::sc_out<ack_type_t> ack{"acknowledge"};
    //!< \brief Acknowledges the income of a new data-stream
#else
    //Entity Ports
    sc_in<stream_type_t> dataInStream{"data_in_stream"};
    //!< \brief New data value to be stored in a cache line
    sc_in<clock_type_t> clk{"clk"};
    //!< \brief Clock of data input cache
    sc_in<write_enable_type_t> write{"write_new_value"};
    //!< \brief If a positive edge occurs, datum from data-in-stream is copied into cache line
    sc_in<select_lines_type_t> slt_in{"data_in_cache_line"};
    //!< \brief Select cache line to store datum from data-in-stream
    sc_in<select_value_type_t> slt_place{"data_place"};
    //!< \brief Select current cache place for to store value at data in stream
    sc_in<select_lines_type_t> slt_out{"data_out_cache_line"};
    //!< \brief Select current cache line for current-data set
    std::array<sc_out<value_type_t>, N> currentValues;
    //!< \brief Currently set values to process
    sc_out<ack_type_t> ack{"acknowledge"};
    //!< \brief Acknowledges the income of a new data-stream
#endif

    //Ctor
    SC_HAS_PROCESS(DataInCache);
    /*!
     * \brief General Constructor
     *
     * \param[in] nameA SystemC module name for DataInCache instance
     */
    DataInCache(const sc_core::sc_module_name &nameA) : sc_core::sc_module(nameA)
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
    void end_of_elaboration() override
    {
        // Initialize current value output with zero
        for (auto &value : currentValues) {
            value.write(0);
        }

        // Initialize all buffers with zero
        for (auto &line : m_cacheLines) {
            for (auto &value : line) {
                value.write(0);
            }
        }

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
#ifdef MCPAT
        ++this->m_writeAccessCounter;
#endif
        // Check if selected place in cache line is valid
        if (N <= slt_place.read().to_uint()) {
            SC_REPORT_WARNING("Cache Warning", "Selected place not in range of cache size");
            ack.write(true);
        }
        else if (L <= slt_in.read().to_uint()) {
            SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");
            ack.write(true);
        }
        else if (write.read() && !ack.read()) {
            // Check if selected cache line is currently in use
            if (slt_in.read().to_uint() != slt_out.read().to_uint()) {
                auto &value = m_cacheLines[slt_in.read().to_uint()][slt_place.read().to_uint()];
                value.write(dataInStream.read());
                ack.write(true);
            }
            else {
                SC_REPORT_WARNING("Cache Warning", "Selected cache-line currently in use. Value is unchanged");
                ack.write(true);
            }
        }
        else if (ack.read() && !write.read()){
            ack.write(false);
        }
    }

    /*!
     * \brief Switch current value cache line
     *
     * \details
     * Function raises a warning, if a cache line should be
     * written which is not reachable (address error).
     * The cache line won't be selected, if it is also currently in use for input values.
     */
    void switchCacheLine()
    {
#ifdef MCPAT
        ++this->m_readAccessCounter;
#endif
        // Check if selected place in cache line is valid
        if (L <= slt_out.read().to_uint()) {
            SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");
        }

        auto tmp_cacheline = slt_out.read().to_uint();

        // Check if selected cache line is currently used for data input
        if (slt_in.read().to_uint() == tmp_cacheline && write.read()) {
            SC_REPORT_WARNING("Cache Warning", "Selected cache line is currently in use. Value is not changed.");
        }
        else {
            for (uint32_t idx = 0; N > idx; ++idx) {
                currentValues.at(idx).write(m_cacheLines.at(tmp_cacheline).at(idx).read());
            }
        }
    }

    /*!
     * \brief Print kind of SystemC module
     */
    const char *kind() const override
    {
        return "Data Input Cache";
    }

    /*!
     * \brief Print data input cache name
     *
     * \param[out] os Define used outstream [default: std::cout]
     */
    void print(std::ostream &os = std::cout) const override
    {
        os << name();
    }

    /*!
     * \brief Dump data cache information
     *
     * \param[out] os Define used outstream [default: std::cout]
     */
    void dump(std::ostream &os = std::cout) const override
    {
        os << name() << ": " << kind() << std::endl;
        os << "Number of cache lines:\t\t\t" << std::setw(3) << static_cast<uint32_t>(L) << std::endl;
        os << "Cache line length[#bytes]:\t\t" << std::setw(3) << static_cast<uint32_t>(size()) << std::endl;
        os << "Bitwidth serial input:\t\t\t" << std::setw(3) << static_cast<uint32_t>(B) << std::endl;
        os << "Number of places per line:\t\t" << std::setw(3) << static_cast<uint32_t>(N) << std::endl;
        os << "Selected input cache line:\t\t" << std::setw(3) << slt_in.read().to_string(sc_dt::SC_DEC, false) << std::endl;
        os << "Selected cache line place:\t\t" << std::setw(3) << slt_place.read().to_string(sc_dt::SC_DEC, false) << std::endl;
        os << "Selected output cache line:\t\t" << std::setw(3) << slt_out.read().to_string(sc_dt::SC_DEC, false) << std::endl;

        os << "Cache content\n";
        os << "=============\n";

        uint32_t line_iter = 1; // Running index for cache line
        for (auto &line : m_cacheLines) {

            os << std::setw(3) << line_iter++ << ":\t";
            uint32_t value_iter = 1;
            for (auto &value : line) {
                os << value_iter++ << ": ";
                value.print(os);
                os << ", ";
            }
            os << "\n";
        }
    }

    /*!
     * \brief Print content of a cache line
     *
     * \param[in]     line     Select cache line to print
     * \param[out]     os         Select outstream to write [default: std::cout]
     */
    void print_cache_line(uint32_t line, std::ostream &os = std::cout) const
    {
        // Check if selected place in cache line is valid
        if (L <= line) {
            SC_REPORT_WARNING("Cache Warning", "Selected cache line not in range of cache size");
            return;
        }

        uint32_t value_iter = 1;
        for (auto &value : m_cacheLines.at(line)) {
            os << value_iter++ << ": ";
            value.print(os);
            os << ", ";
        }
        os << "\n";
    }

    /*!
     * \brief Return the size of a cache line
     */
    uint16_t size() const
    {
        return m_numOfBytes;
    }

    /*!
     * \brief Return number of cache lines
     */
    uint8_t cache_size() const
    {
        return m_cacheLines.size();
    }

#ifdef MCPAT
    /**
     * @brief Dump statistics for McPAT simulation
     *
     * @param os Out stream to write results to
     */
    void dumpMcpatStatistics(std::ostream &os = ::std::cout) const override
    {
        os << name() << "\t\t" << kind() << "\n";
        os << "read accesses: " << this->m_readAccessCounter << "\n";
        os << "write accesses: " << this->m_writeAccessCounter << "\n";
        os << std::endl;
    }
#endif

private:
    //Forbidden Constructors
    DataInCache() = delete;
    DataInCache(const DataInCache& src) = delete;                //!< \brief because sc_module could not be copied
    DataInCache& operator=(const DataInCache& src) = delete;    //!< \brief because move not implemented for sc_module
    DataInCache(DataInCache&& src) = delete;                    //!< \brief because sc_module could not be copied
    DataInCache& operator=(DataInCache&& src) = delete;          //!< \brief because move not implemented for sc_module

    //Private Signals and Buffers
    std::array<std::array<sc_core::sc_buffer<value_type_t>, N>, L> m_cacheLines;
    //!< \brief Two-dimensional array, where 1st. dimension is cache line and 2nd. dimension is value
    const uint32_t m_numOfBytes{cgra::calc_numOfBytes(B * N)};
    //!< \brief Cache line size in bytes
};

} // namespace cgra

#endif /* HEADER_DATAINCACHE_H_ */
