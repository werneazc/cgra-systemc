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
#include "CommandInterpreter.h"
#include "Typedef.h"

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
	typedef CommandInterpreter::line_type_t line_type_t;
	//!< \brief MMU line type to select cache line at source/target cache
	typedef CommandInterpreter::place_type_t place_type_t;
	//!< \brief MMU place type for data position in cache line of target/source cache

	sc_core::sc_in<address_type_t> address{"Address"};
	//!< \brief Shared memory address to load from/store to data
	sc_core::sc_in<line_type_t> line{"Line"};
	//!< \brief Cache line in target cache
	sc_core::sc_in<place_type_t> place{"Place"};
	//!< \brief Cache line place in target cache
	sc_core::sc_in<cache_select_type_t> cache_select{"Cache_Select"};
	//!< \brief Cache selection for latest operation
	sc_core::sc_in<start_type_t> start{"Start"};
	//!< \brief A positive edge at this port starts a new operation sequence
	sc_core::sc_out<ready_type_t> ready{"Ready"};
	//!< \brief Signal to show current processing; Ready for new command if false


	/*! Definition of operating cache types */
	enum CACHE_TYPE : uint8_t
	{
		NONE,				//!< \brief No cache selected
		DATA_INPUT, 		//!< \brief Data input cache for inputs of VCGRA
		DATA_OUTPUT,		//!< \brief Data output cache for results of VCGRA
		CONF_PE,			//!< \brief Configuration cache for processing elements
		CONF_CC				//!< \brief Configuration cache for virtual channels
	};

};

} // end namespace cgra

#endif /* HEADER_MMU_H_ */
