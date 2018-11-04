/*
 * MMU.h
 *
 *  Created on: 03.07.2018
 *      Author: andrewerner
 */
#ifndef HEADER_MMU_H_
#define HEADER_MMU_H_

#include <cstdint>
#include <systemc>
#include <iostream>
#include <array>
#include <initializer_list>
#include <cstring>
#include "Typedef.h"
#include "CommandInterpreter.h"

namespace cgra
{

/*!
 * \class MMU
 *
 * \brief Memory Management Unit for shared memory
 *
 * \details
 *
 */
class MMU : public sc_core::sc_module
{
public:
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief MMU clock type
	typedef bool ready_type_t;
	//!< \brief Signal type to show current processing; Ready for new command if false
	typedef bool start_type_t;
	//!< \brief Start processing using data at address, line, cache select and place port
	typedef sc_dt::sc_lv<3> cache_select_type_t;
	//!< \brief MMU selection type for available caches
	typedef CommandInterpreter::address_type_t address_type_t;
	//!< \brief MMU address type for shared memory data location
	typedef CommandInterpreter::place_type_t place_type_t;
	//!< \brief MMU place type for data position in cache line of target/source cache
	typedef cgra::cache_ack_type_t ack_type_t;
	//!< \brief Cache transaction acknowledge type
	typedef cgra::cache_write_type_t write_type_t;
	//!< \brief Cache transmission write enable type
	typedef sc_dt::sc_lv<cgra::cDataStreamBitWidthConfCaches> conf_stream_type_t;
	//!< \brief Configuration data stream type
	typedef sc_dt::sc_lv<cgra::cDataValueBitwidth> data_stream_type_t;
	//!< \brief Configuration data stream type
	typedef sc_dt::sc_uint<cgra::calc_bitwidth(cNumberOfValuesPerCacheLine)> cache_place_type_t;
	//!< \brief Data type for place signal lines to data caches

	sc_core::sc_in<address_type_t> address{"Address"};
	//!< \brief Shared memory address to load from/store to data
	sc_core::sc_in<place_type_t> place{"Place"};
	//!< \brief Cache line place in target cache
	sc_core::sc_in<cache_select_type_t> cache_select{"Cache_Select"};
	//!< \brief Cache selection for latest operation
	sc_core::sc_in<start_type_t> start{"Start"};
	//!< \brief A positive edge at this port starts a new operation sequence
	sc_core::sc_out<ready_type_t> ready{"Ready"};
	//!< \brief Signal to show current processing; Ready for new command if false
	sc_core::sc_out<write_type_t> write_enable{"Write_Enable"};
	//!< \brief Signal to load streamed data into a cache location
	sc_core::sc_in<ack_type_t> ack{"acknowledge"};
	//!< \brief Signal acknowledge a valid cache transmission
	sc_core::sc_out<conf_stream_type_t> conf_cache_stream{"Conf_Cache_Stream"};
	//!< \brief Configuration data stream to configuration caches
	sc_core::sc_out<data_stream_type_t> data_value_out_stream{"Data_Value_Out_Stream"};
	//!< \brief Data value out stream to data input caches of VCGRA
	sc_core::sc_in<data_stream_type_t> data_value_in_stream{"Data_Value_In_Stream"};
	//!< \brief Data value in stream from data output caches of VCGRA
	sc_core::sc_out<cache_place_type_t> cache_place{"Slct_Cache_Place"};
	//!< \brief Selected Cache place in data input/output caches.
	sc_core::sc_in<clock_type_t> clk{"clock"};
	//!< \brief System clock input


	/*! Definition of operating cache types */
	enum CACHE_TYPE : uint8_t
	{
		DATA_INPUT, 		//!< \brief Data input cache for inputs of VCGRA
		DATA_OUTPUT,		//!< \brief Data output cache for results of VCGRA
		CONF_PE,			//!< \brief Configuration cache for Processing_Elements
		CONF_CC,			//!< \brief Configuration cache for VirtualChannels
		NONE				//!< \brief No cache selected
	};


	//Ctor
	SC_HAS_PROCESS(MMU);
	/*!
	 * \brief General Constructor
	 *
	 * \details
	 * The second constructor parameter is for the internal storage of the caches features.
	 * For the four cache types CONF_PE, CONF_CH, DATA_INPUT and DATA_OUTPUT, three properties
	 * are stored: LINESIZE (LS) in #places, CACHESIZE (CS) in #lines, DATAWIDTH (DW) in #bits.
	 * The parameter expects a value in form {{LS,CS,DW},{LS,CS,DW},{LS,DS,DW},{LS,CS,DW}}.
	 * For the configuration caches the DW needs to be set to the LINESIZE in bits of a cache line.
	 *
	 * \param[in] nameA SystemC module name for MMU instance
	 * \param[in] cacheFeaturesA Initializer list of exact 12 values including features of all available cache types
	 */
	MMU(const sc_core::sc_module_name& nameA, std::initializer_list<uint16_t> cacheFeaturesA );

	virtual ~MMU();
	//!< \brief Destructor

	/*!
	 * \brief Initialize output signals of module
	 */
	virtual void end_of_elaboration() override;

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override
	{ return "MMU";	}

	/*!
	 * \brief Print name of MMU
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void print(std::ostream& os = std::cout) const override
	{ os << this->name(); return; }

	/*!
	 * \brief Dump MMU information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override;

	/*!
	 * \brief Internal state machine of MMU
	 */
	void state_machine();

	/*!
	 * \brief Dump shared memory content
	 *
	 * \details
	 * Prints content of shared memory cell by cell.
	 * The format can be selected by formatA input.
	 * \param[in] startAddrA Start address to dump memory content
	 * \param[in] endAddrA End address to stop dumping of memory content
	 * \param[in] formatA Format data representation (decimal, octal, hexadecimal) [default: hex]
	 * \param[in] showBaseA Add prefix to show data representation [default: false]
	 * \param[out] os Define used outstream [default: std::cout]
	 *
	 * \tparam T Interpretation data type for stored and displayed data
	 *
	 */
	template<typename T>
	void dump_memory(const uint32_t startAddrA, const uint32_t endAddrA,
			sc_dt::sc_numrep formatA = sc_dt::SC_NOBASE, bool showBaseA = false,
			std::ostream& os = std::cout) const;

	/*!
	 * \brief Write content to shared memory
	 *
	 * \param[in] startAddA Start address to write data to
	 * \param[in] startDataA Pointer to value or value array
	 * \param[in] numOfValuesA Number of values which should be copied (default = 1)
	 *
	 * \tparam T Data type of value(s) which should be stored in shared memory
	 * \return True for successful storing in shared memory
	 */
	template<typename T>
	bool write_shared_memory(const uint32_t startAddrA, T* startDataA, uint32_t numOfValuesA = 1);

	/*!
	 * \brief Read data from shared memory
	 *
	 * \param[in] startAddA Start address to read data from
	 * \param[in] startDataA Pointer to value or value array
	 * \param[in] numOfValuesA Number of values which should be copied (default = 1)
	 *
	 * \tparam T Data type of value(s) which should be read from shared memory
	 * \return True for successful storing in shared memory
	 */
	template<typename T>
	bool read_shared_memory(const uint32_t startAddrA, T* startDataA, uint32_t numOfValuesA = 1) const;

private:
	//Private type definitions
	typedef std::array<std::array<uint16_t, 3>, 4> cache_features_type_t;
    //!< \brief Storage type for cache features to store
	/*!
	 * \brief Type defines the size of an addressable memory line in shared memory.
	 *
	 * \details The size of the memory type in bits needs to be lower or equal to the
	 * bitwidth of the streaming interfaces.
	 */
	typedef uint8_t memory_size_type_t;

	//Private Members
	enum CACHE_TYPE pCurrentCache{CACHE_TYPE::NONE};
	//!< \brief Case selector for currently selected cache
	/*!
	 * \details
	 * Cache Order: DATA_INPUT, DATA_OUTPUT, CONF_PE, CONF_CC
	 * Value Order: LINESIZE, CACHESIZE, DATAWIDTH
	 */
	cache_features_type_t pCacheFeatures;
	//!< \brief Stores cache features of the architecture
	sc_core::sc_buffer<address_type_t> pAddress{"CurrentAddress"};
	//!< \brief Shared memory address to load from/store to data
	sc_core::sc_buffer<place_type_t> pPlaceIn{"CurrentPlace_InputBuffer"};
	//!< \brief Cache line place in target cache (input buffer)
	sc_core::sc_buffer<cache_place_type_t> pPlaceOut{"CurrentPlace_OutputBuffer"};
	//!< \brief Cache line place in target cache (output buffer)
	memory_size_type_t* const pMemStartPtr{static_cast<memory_size_type_t*>(calloc(cgra::cMemorySize, sizeof(memory_size_type_t)))};
	//!< \brief Start address of shared memory block.
	memory_size_type_t* const pMemEndPtr{pMemStartPtr+cgra::cMemorySize - 1};
	//!< \brief End address of shared memory block.
	memory_size_type_t* pCurrentMemPtr;
	//!< \brief Actual position in shared memory block.

	/*!
	 * \brief Flag for block transmissions of whole cache lines.
	 *
	 * \details
	 * If "place" value equals 127, a complete data block in the size of
	 * addressed cache line is processed. The block starts at the given
	 * address and is processed sequentially in shared memory.
	 */
	bool pBlockTransmission{false};
	uint16_t pNumOfTransmission{0};
	//!< \brief This is the number of open required transmissions to complete transmission process.
	uint16_t pAddressStepSize{0};
	//!< \brief This is the step size for address adaption in block transfers.

	/*! Definition for cache properties to ease the access. */
	enum FEATURE_SELECT : uint8_t
	{
		LINESIZE, 	//!< \brief Length of a cache line in number of bytes
		CACHESIZE,	//!< \brief Number of cache lines
		DATAWIDTH	//!< \brief Data width of a datum within a cache line
	};
	//!< \brief Select a feature from cache features array.

	/*! State machine states of MMU */
	enum STATES : uint8_t
	{
		AWAIT,			//!< \brief Wait for start signal to perform next data transaction
		DECODE,			//!< \brief Decode data transmission request
		VALIDATE,		//!< \brief Validate a selected cache place for data input/output
		PROCESS,		//!< \brief Start transmission process
		WRITE_DATA,		//!< \brief Write new data to data stream output
		WRITE_EN,		//!< \brief Set current data at data stream output
		WAIT_ACK,		//!< \brief Wait for cache to acknowledge data transmission
		READ_DATA,		//!< \brief Read data from data input stream
		BLOCK			//!< \brief Adaption for block transmissions
	}pState{AWAIT};
	//!< \brief State machine variable

	//Private Methods
	void process_data_input();
	//!< \brief Handle data transmission to data input cache
	void process_configuration();
	//!< \brief Handle data transmission to configuration caches
	void process_data_output();
	//!< \brief Handle data transmission from data output cache

};

} // end namespace cgra

template<typename T>
inline void cgra::MMU::dump_memory(const uint32_t startAddrA,
		const uint32_t endAddrA, sc_dt::sc_numrep formatA, bool showBaseA,
		std::ostream& os) const
{

	//Set temporary pointer to access memory data.
	memory_size_type_t* tMemPtr = pMemStartPtr + startAddrA;

	if(!(formatA == sc_dt::SC_OCT || formatA == sc_dt::SC_DEC || \
			formatA == sc_dt::SC_HEX || formatA == sc_dt::SC_NOBASE))
		SC_REPORT_WARNING("MMU Warning", "Unknown format for memory dump.Show raw data without formating");


	//Count number of printed elements in a row. Insert line break after 8 dumped elements.
	uint16_t tLinecounter{0};
	while(tMemPtr <= pMemEndPtr && tMemPtr <= (pMemStartPtr + endAddrA))
	{
		switch (formatA)
		{
			case sc_dt::SC_OCT:
				if(showBaseA)
					os << std::oct << std::showbase << *(reinterpret_cast<T*>(tMemPtr));
				else
					os << std::oct << std::noshowbase << *(reinterpret_cast<T*>(tMemPtr));
				break;
			case sc_dt::SC_DEC:
				if(showBaseA)
					os << std::dec << std::showbase << *(reinterpret_cast<T*>(tMemPtr));
				else
					os << std::dec << std::noshowbase << *(reinterpret_cast<T*>(tMemPtr));
				break;
			case sc_dt::SC_HEX:
				if(showBaseA)
					os << std::hex << std::showbase << *(reinterpret_cast<T*>(tMemPtr));
				else
					os << std::hex << std::noshowbase << *(reinterpret_cast<T*>(tMemPtr));
				break;
			default:
				os << *(reinterpret_cast<T*>(tMemPtr));
				break;
		}
		//Set line break after 8 values in a row.
		if(tLinecounter > 0 && !(tLinecounter % 7))
			os << std::endl;
		else
			os << '\t';

		tMemPtr += sizeof(T) / sizeof(memory_size_type_t);
		++tLinecounter;
	}

	os << std::endl;
	return;
};

template<typename T>
inline bool cgra::MMU::write_shared_memory(const uint32_t startAddrA,
		T* startDataA, const uint32_t numOfValuesA)
{
	//Set temporary pointer to access memory data.
	memory_size_type_t* tMemPtr = pMemStartPtr + startAddrA;

	uint32_t tStoreCounter{0};

	while(tMemPtr + sizeof(T) <= pMemEndPtr && tStoreCounter < numOfValuesA)
	{
		memcpy(tMemPtr, startDataA, sizeof(T));
		tMemPtr += sizeof(T) / sizeof(memory_size_type_t);
		++startDataA;
		++tStoreCounter;
	}

	if(tStoreCounter == numOfValuesA)
		return true;
	else
		return false;
}

template<typename T>
inline bool cgra::MMU::read_shared_memory(const uint32_t startAddrA,
		T* startDataA, uint32_t numOfValuesA) const
{

	//Set temporary pointer to access memory data.
	memory_size_type_t* tMemPtr = pMemStartPtr + startAddrA;

	uint32_t tStoreCounter{0};

	while(tMemPtr <= pMemEndPtr && tStoreCounter < numOfValuesA)
	{
		memcpy(startDataA,  tMemPtr, sizeof(T));
		tMemPtr += sizeof(T) / sizeof(memory_size_type_t);
		++startDataA;
		++tStoreCounter;
	}

	if(tStoreCounter == numOfValuesA)
		return true;
	else
		return false;
}

#endif /* HEADER_MMU_H_ */
