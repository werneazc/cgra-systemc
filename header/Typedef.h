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
#include <cmath>

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
 */
constexpr uint16_t calc_bitwidth(const uint16_t numOfCacheLinesA)
{
	return static_cast<uint16_t>(ceil(log2(numOfCacheLinesA)));
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
using cache_load_type_t = cache_write_type_t;
//!< \brief Alias for cache_write_type_t
static constexpr uint16_t cMemorySize{1024};
//!< \brief Shared memory size in byte


//Properties for Management Unit
//--------------------------------------
constexpr uint32_t cProgramMemorySize{10};
//!< \brief Set program memory size for assembler commands

//Properties for PE configuration cache
//--------------------------------------
static constexpr uint16_t cPeConfigBitWidth{48};
//!< \brief Number of bits for whole PE configuration of VCGRA
static constexpr uint16_t cNumberOfPeCacheLines{2};
//!< \brief Number of cache lines for PE configuration cache
static constexpr uint16_t cSelectLineBitwidthPeConfCache{calc_bitwidth(cNumberOfPeCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
static constexpr uint16_t cBitWidthOfSerialInterfacePeConfCache{cDataStreamBitWidthConfCaches};
//!< \brief Bitwidth for serial configuration input stream to configuration cache


//Properties for virtual channel configuration cache
//--------------------------------------------------
static constexpr uint16_t cVChConfigBitWidth{64};
//!< \brief Number of bits for whole vCh configuration of VCGRA
static constexpr uint16_t cNumberOfVChCacheLines{2};
//!< \brief Number of cache lines for vCh configuration cache
static constexpr uint16_t cSelectLineBitwidthVChConfCache{calc_bitwidth(cNumberOfVChCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
static constexpr uint16_t cBitWidthOfSerialInterfaceVChConfCache{cDataStreamBitWidthConfCaches};
//!< \brief Bitwidth for serial configuration input stream to configuration cache


//Properties for data caches
//--------------------------------------------------
static constexpr uint16_t cDataValueBitwidth{16};
//!< \brief Number of bits for one
static constexpr uint16_t cNumberOfValuesPerCacheLine{8};
//!< \brief Number of accessible data values in a cache line
static constexpr uint16_t cNumberDataInCacheLines{2};
//!< \brief Number of cache lines for PE configuration cache
static constexpr uint16_t cSelectLineBitwidthDataInCache{calc_bitwidth(cNumberDataInCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}

}

#endif /* HEADER_TYPEDEF_H_ */
