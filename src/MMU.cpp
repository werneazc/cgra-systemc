/*
 * MMU.cpp
 *
 *  Created on: Oct 2, 2018
 *      Author: andrewerner
 */

#include "MMU.h"
#include <cstring>

namespace cgra {

MMU::MMU(const sc_core::sc_module_name& nameA,
		std::initializer_list<uint16_t> cacheFeaturesA) :
		sc_core::sc_module(nameA), pCurrentMemPtr(const_cast<memory_size_type_t*>(pMemStartPtr))
{

	//Initialize cache feature list; Check if the constructor parameter will have the correct size
	if((pCacheFeatures.size() * pCacheFeatures.at(CACHE_TYPE::CONF_CC).size()) != cacheFeaturesA.size())
		SC_REPORT_ERROR("MMU Constructor Error", "Size of initializer list for cache features not equal 12.");
	else
	{
		uint8_t idx = 0;
		for(auto val : cacheFeaturesA)
		{
			pCacheFeatures.at(idx/3).at(idx%3) = val;
			++idx;
		}
	}

	SC_METHOD(state_machine);
	sensitive << clk.pos();

	return;
}

MMU::~MMU()
{
	free(pMemStartPtr);

	return;
}

void MMU::end_of_elaboration()
{
	//Initialize output ports of MMU entity before simulator starts.
	ready.write(false);
	conf_cache_stream.write(0);
	write_enable.write(false);
	data_value_out_stream.write(0);

	return;
}

void MMU::state_machine()
{
	switch (pState)
	{
		case STATES::AWAIT:
			if(ready.read() && start.read())
				ready.write(false);

			else if(start.read() && !(ready.read()))
			{
				//Save current cache type for further processing
				pCurrentCache = static_cast<CACHE_TYPE>(cache_select.read().to_uint());
				pAddress.write(address.read());
				pPlace.write(place.read());

				switch (pCurrentCache) {
					case CACHE_TYPE::DATA_OUTPUT:
						pState = STATES::WRITE_EN;
						break;
					case CACHE_TYPE::CONF_CC:
					case CACHE_TYPE::CONF_PE:
					case CACHE_TYPE::DATA_INPUT:
						pState = STATES::WRITE_DATA;
						break;
					default:
						SC_REPORT_WARNING("MMU State Machine", "Unknown cache type. Await for new start signal.");
						break;
				}
			}
			break;

		case STATES::WRITE_DATA:

			//Set data output regarding current handled cache type
			switch (pCurrentCache)
			{
				case CACHE_TYPE::DATA_INPUT:
					process_data_input();
					break;
				case CACHE_TYPE::CONF_CC:
				case CACHE_TYPE::CONF_PE:
					process_configuration();
					break;
				default:
					SC_REPORT_WARNING("MMU State Machine", "Unknown cache type. Await for new start signal.");
					pState = STATES::AWAIT;
					break;
			}
			pState = STATES::WRITE_EN;
			break;

		case STATES::WRITE_EN:
			write_enable.write(true);
			pState = STATES::WAIT_ACK;
			break;

		case STATES::WAIT_ACK:
			if(ack.read())
			{
				if(pCurrentCache == CACHE_TYPE::DATA_OUTPUT)
					pState = STATES::READ_DATA;
				else
				{
					write_enable.write(false);
					ready.write(true);
					pState = STATES::AWAIT;
				}
			}
			break;

		case STATES::READ_DATA:
			process_data_output();
			write_enable.write(false);
			ready.write(true);
			pState = STATES::AWAIT;
			break;

		default:
			SC_REPORT_WARNING("MMU State Machine.", "Unknown state-machine state. Nothing done. Wait for new valid input");
			break;
	}

	return;
}

void MMU::process_data_input()
{

	if(pAddress.read().to_uint() + (cgra::cDataValueBitwidth/(8 * sizeof(memory_size_type_t))) >= cgra::cMemorySize)
		SC_REPORT_WARNING("MMU Transmission Error", "Addressed value out of memory.");
	else
	{
		//Temporary variable for data to write to data stream
		data_stream_type_t tvalue{0};

		pCurrentMemPtr = pMemStartPtr + pAddress.read().to_uint();
		memcpy(&tvalue, pCurrentMemPtr, (cgra::cDataValueBitwidth / (8 * sizeof(memory_size_type_t))));
		data_value_out_stream.write(tvalue);
	}

	return;
}

void MMU::process_configuration()
{

	if(pAddress.read().to_uint() + (cgra::cDataStreamBitWidthConfCaches / (8 * sizeof(memory_size_type_t))) >= cgra::cMemorySize)
		SC_REPORT_WARNING("MMU Transmission Error", "Addressed value out of memory.");
	else
	{
		//Temporary variable for data to write to data stream
		conf_stream_type_t tvalue{0};

		pCurrentMemPtr = pMemStartPtr + pAddress.read().to_uint();
		memcpy(&tvalue, pCurrentMemPtr, (cgra::cDataStreamBitWidthConfCaches / (8 * sizeof(memory_size_type_t))));
		conf_cache_stream.write(tvalue);
	}

	return;
}

void MMU::process_data_output()
{

	if(pAddress.read().to_uint() + (cgra::cDataValueBitwidth/(8 * sizeof(memory_size_type_t))) >= cgra::cMemorySize)
		SC_REPORT_WARNING("MMU Transmission Error", "Addressed value out of memory.");
	else
	{
		//Temporary variable for data to write to data stream
		data_stream_type_t tvalue{0};

		pCurrentMemPtr = pMemStartPtr + pAddress.read().to_uint();
		tvalue = data_value_in_stream.read();
		memcpy(pCurrentMemPtr, &tvalue, (cgra::cDataValueBitwidth / (8 * sizeof(memory_size_type_t))));
	}

	return;
}

} /* End namespace cgra */
