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
 * /brief Calculate number of bits to select available cache lines.
 */
constexpr uint16_t calc_bitwidth(const uint16_t numOfCacheLinesA)
{
	return static_cast<uint16_t>(ceil(log2(numOfCacheLinesA)));
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

//Properties for PE configuration cache
//--------------------------------------
constexpr uint16_t cPeConfigBitWidth{48};
//!< \brief Number of bits for whole PE configuration of VCGRA
constexpr uint16_t cNumberOfPeCacheLines{2};
//!< \brief Number of cache lines for PE configuration cache
constexpr uint16_t cSelectLineBitwidthPeConfCache{calc_bitwidth(cNumberOfPeCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
constexpr uint16_t cBitWidthOfSerialInterfacePeConfCache{8};
//!< \brief Bitwidth for serial configuration input stream to configuration cache


//Properties for virtual channel configuration cache
//--------------------------------------------------
constexpr uint16_t cVChConfigBitWidth{64};
//!< \brief Number of bits for whole vCh configuration of VCGRA
constexpr uint16_t cNumberOfVChCacheLines{2};
//!< \brief Number of cache lines for vCh configuration cache
constexpr uint16_t cSelectLineBitwidthVChConfCache{calc_bitwidth(cNumberOfVChCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
constexpr uint16_t cBitWidthOfSerialInterfaceVChConfCache{8};
//!< \brief Bitwidth for serial configuration input stream to configuration cache


//Properties for input data cache
//--------------------------------------------------
constexpr uint16_t cDataValueBitwidth{16};
//!< \brief Number of bits for one
constexpr uint16_t cNumberOfValuesPerCacheLine{2};
//!< \brief Number of accessible data values in a cache line
constexpr uint16_t cNumberDataInCacheLines{2};
//!< \brief Number of cache lines for PE configuration cache
constexpr uint16_t cSelectLineBitwidthVChConfCache{calc_bitwidth(cNumberDataInCacheLines)};
//!< \brief Bitwidth to select available cache lines round-up{log2(cNumberOfCacheLines)}
constexpr uint16_t cBitWidthOfSerialInterfaceDateInCache{8};
//!< \brief Bitwidth for serial configuration input stream to configuration cache

}



#endif /* HEADER_TYPEDEF_H_ */
