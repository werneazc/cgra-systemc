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
	 * are stored: LINESIZE (LS), CACHESIZE (CS), DATAWIDTH (DW). The parameter expects a value in form
	 * {{LS,CS,DW},{LS,CS,DW},{LS,DS,DW},{LS,CS,DW}}. For the configuration caches the DW is ignored and
	 * might be set to zero or to the LINESIZE of a cache line.
	 *
	 * \param[in] nameA SystemC module name for DataInCache instance
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
	 * \brief Internal state machine of MMU
	 */
	void state_machine();

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
    //!<

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
	sc_core::sc_buffer<place_type_t> pPlace{"CurrentPlace"};
	//!< \brief Cache line place in target cache
	memory_size_type_t* const pMemStartPtr{static_cast<memory_size_type_t*>(calloc(cgra::cMemorySize, sizeof(memory_size_type_t)))};
	//!< \brief Start address of shared memory block.
	memory_size_type_t* const pMemEndPtr{pMemStartPtr+cgra::cMemorySize};
	//!< \brief End address of shared memory block.
	memory_size_type_t* pCurrentMemPtr;
	//!< \brief Actual position in shared memory block.

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
		WRITE_DATA,		//!< \brief Write new data to data stream output
		WRITE_EN,		//!< \brief Set current data at data stream output
		WAIT_ACK,		//!< \brief Wait for cache to acknowledge data transmission
		READ_DATA		//!< \brief Read data from data input stream
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

#endif /* HEADER_MMU_H_ */
