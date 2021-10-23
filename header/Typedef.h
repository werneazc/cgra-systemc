/*
 * Typedef.h
 *
 *  Created on: 06.07.2018
 *      Author: andrewerner
 */

#ifndef HEADER_TYPEDEF_H_
#define HEADER_TYPEDEF_H_

#include <systemc>
#include <cstdint>
#include <array>

/*!
 * \namespace cgra
 * \brief Namespace of VCGRA elements and functions
 */
namespace cgra {

//Function prototypes
//===================
/*!
 * \brief Calculate number of bits to select available cache lines.
 *
 * \param[in] numOfCacheLinesA Number of available cache lines to address
 * 
 * \details
 * Currently its implemented over a simple lookup table, because the 
 * binary logarithm function is not defined for const expression. 
 * Valid inputs for numOfCacheLinesA is ]0, 17].
 */
constexpr uint16_t calc_bitwidth(const uint16_t numOfCacheLinesA)
{
    //Simple look-up table for bitwitdh of select lines
    constexpr std::array<uint16_t, 17> cBitwidth{
        0, 1, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5};
    
    return cBitwidth.at(numOfCacheLinesA);
}

/*!
 * \brief Calculate number of necessary bytes to store a given number of bits
 *
 * \param[in] numOfBitsA Number of bits to store
 */
constexpr uint32_t calc_numOfBytes(const uint32_t numOfBitsA)
{
    return (numOfBitsA % 8) ? (numOfBitsA / 8 + 1) : (numOfBitsA / 8);
}


//Global type definitions
//=======================

//Control Signals
typedef bool clock_type_t;
//!< \brief Architecture clock type
typedef bool reset_type_t;
//!< \brief Architecture reset type
typedef bool ready_type_t;
//!< \brief Architecture ready signal type from VCGRA
typedef bool start_type_t;
//!< \brief Architecture start signal type for VCGRA

//VCGRA properties
//-------------------
static constexpr uint32_t cNumOfLevels{4};
//!< \brief Number of PEs in the VCGRA instance
static constexpr uint32_t cNumOfPe{16};
//!< \brief Number of PEs in the VCGRA instance
static constexpr std::array<uint16_t, cNumOfLevels> cPeLevels{4, 4, 4, 4}; 
//!< \brief Number of PEs for each VCGRA level

//Properties PEs
//-------------------
static constexpr uint32_t cPeDataBitwidth{16};
//!< \brief Bitwidth definition for data values of PE instances of a VCGRA
static constexpr uint32_t cPeConfigLvSize{4};
//!< \brief Bitwidth for logic vector size to decode all available PE operations

//Properties Virtual Channels
//---------------------------
static constexpr uint32_t cInputChannel_NumOfInputs{8};
//!< \brief Number of inputs for the first layer of a VirtualChannel
static constexpr uint32_t cInputChannel_InputBitwidth{cPeDataBitwidth};
//!< \brief Bitwidth of inputs for the first layer of a VirtualChannel
static constexpr uint32_t cInputChannel_NumOfOutputs{8};
//!< \brief Number of outputs for the first layer of a VirtualChannel
static constexpr uint32_t cInputChannel_OutputBitwidth{cPeDataBitwidth};
//!< \brief Bitwidth of outputs for the first layer of a VirtualChannel
static constexpr uint32_t cInputChannel_MuxScltBitwidth{3};
//!< \brief Bitwidth the internal Multiplexers selection port
static constexpr uint32_t cInputChannel_InternalBitwidth{cPeDataBitwidth};
//!< \brief Bitwidth the internal connections within the VirtualChannel instance
static constexpr uint32_t cChannel_NumOfInputs{4};
//!< \brief Number of inputs of a VirtualChannel
static constexpr uint32_t cChannel_InputBitwidth{cPeDataBitwidth};
//!< \brief Bitwidth of inputs of a VirtualChannel
static constexpr uint32_t cChannel_NumOfOutputs{8};
//!< \brief Number of outputs of a VirtualChannel
static constexpr uint32_t cChannel_OutputBitwidth{cPeDataBitwidth};
//!< \brief Bitwidth of outputs of a VirtualChannel
static constexpr uint32_t cChannel_MuxScltBitwidth{2};
//!< \brief Bitwidth the internal Multiplexers selection port
static constexpr uint32_t cChannel_InternalBitwidth{cPeDataBitwidth};
//!< \brief Bitwidth the internal connections within the VirtualChannel instance


//Properties for MMU usability
//--------------------------------------
/* These definitions are caused by the shared resources of MMU ports to the configuration caches.
 * Ack, Write_Enable and DataInstream of a PE, channel and mask configuration are
 * shared ports at the MMU. Multiplexers are controlled by ManagementUnit to select
 * the necessary connection during runtime.
 */
static constexpr uint16_t cDataStreamBitWidthConfCaches{8};
//!< \brief Bitwidth for data streams of configuration parts to configuration caches
typedef bool cache_write_type_t;
//!< \brief Cache type to store streamed input data to a cache location
typedef bool cache_ack_type_t;
//!< \brief Cache type to show MMU data processing
typedef sc_dt::sc_lv<3> cache_slct_type_t;
//!< \brief Select type for available cache types (none, data-input, data-output, pe-config, vch-config)
using cache_load_type_t = cache_write_type_t;
//!< \brief Alias for cache_write_type_t for data output cache load port of a VCGRA
static constexpr uint16_t cMemorySize{UINT16_MAX};
//!< \brief Shared memory size in byte


//Properties for Management Unit
//--------------------------------------
constexpr uint32_t cProgramMemorySize{200000};
//!< \brief Set program memory size for assembler commands

//Properties for PE configuration cache
//--------------------------------------
static constexpr uint16_t cPeConfigBitWidth{64};
//!< \brief Number of bits for whole PE configuration of VCGRA
static constexpr uint16_t cNumberOfPeCacheLines{2};
//!< \brief Number of cache lines for PE configuration cache
static constexpr uint16_t cSelectLineBitwidthPeConfCache{calc_bitwidth(cNumberOfPeCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
static constexpr uint16_t cBitWidthOfSerialInterfacePeConfCache{cDataStreamBitWidthConfCaches};
//!< \brief Bitwidth for serial configuration input stream to configuration cache
typedef sc_dt::sc_lv<cPeConfigBitWidth> pe_config_type_t;
//!< \brief Signal type for the PE configuration connection between PE configuration cache and VCGRA.


//Properties for virtual channel configuration cache
//--------------------------------------------------
static constexpr uint16_t cVChConfigBitWidth{80};
//!< \brief Number of bits for whole vCh configuration of VCGRA
static constexpr uint16_t cNumberOfVChCacheLines{2};
//!< \brief Number of cache lines for vCh configuration cache
static constexpr uint16_t cSelectLineBitwidthVChConfCache{calc_bitwidth(cNumberOfVChCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
static constexpr uint16_t cBitWidthOfSerialInterfaceVChConfCache{cDataStreamBitWidthConfCaches};
//!< \brief Bitwidth for serial configuration input stream to configuration cache
typedef sc_dt::sc_lv<cVChConfigBitWidth> ch_config_type_t;
//!< \brief Signal type for the VirtualChannel configuration connection between VCH configuration cache and VCGRA.



//Properties for data caches
//--------------------------------------------------
static constexpr uint16_t cDataValueBitwidth{cPeDataBitwidth};
//!< \brief Number of bits for one data value
static constexpr uint16_t cMaxNumberOfValuesPerCacheLine{static_cast<uint16_t>(
    (2*cgra::cPeLevels.front()) >= cgra::cPeLevels.back() ? 2 * cgra::cPeLevels.front() : cPeLevels.back())};
//!< \brief Number of accessible data values in a cache line
static constexpr uint16_t cNumberDataInCacheLines{2};
//!< \brief Number of cache lines for data input cache
static constexpr uint16_t cNumberDataOutCacheLines{2};
//!< \brief Number of cache lines for data output cache

//MMU initializer list
//--------------------------------------------------
/*!
 * \brief Initializer list for MMU constructor to store cache parameters.
 *
 * \details
 * The initializer list needs to consists twelve values for four caches:
 * DATA_IN, DATA_OUT, CONF_PE and CONF_VCH. Three following values contain
 * to one cache type. The meaning of the three values is:
 * LINESIZE (LS) in #bytes, CACHESIZE (CS) in #lines, DATAWIDTH (DW) in #bits.
 *
 * Initializer_list<uint16_t>{LS,CS,DW,LS,CS,DW,LS,CS,DW,LS,CS,DW};
 *
 */
static constexpr std::initializer_list<uint16_t> cCacheFeatures{
    static_cast<uint16_t>(cgra::calc_numOfBytes(cgra::cDataValueBitwidth * 2 * cgra::cPeLevels.front())), cgra::cNumberDataInCacheLines, cgra::cDataValueBitwidth,
    static_cast<uint16_t>(cgra::calc_numOfBytes(cgra::cDataValueBitwidth * cgra::cPeLevels.back())), cgra::cNumberDataOutCacheLines, cgra::cDataValueBitwidth,
    static_cast<uint16_t>(cgra::calc_numOfBytes(cgra::cPeConfigBitWidth)), cgra::cNumberOfPeCacheLines, cgra::cPeConfigBitWidth,
    static_cast<uint16_t>(cgra::calc_numOfBytes(cgra::cVChConfigBitWidth)), cgra::cNumberOfVChCacheLines, cgra::cVChConfigBitWidth
};

}

#endif /* HEADER_TYPEDEF_H_ */
