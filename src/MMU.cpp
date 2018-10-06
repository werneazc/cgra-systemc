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

				pState = STATES::DECODE;

			}
			break;

		case STATES::DECODE:
			if(127 == pPlace.read().to_uint())
			{
				pBlockTransmission = true;
				//Get cache line size in bits to calculate number of transmissions for whole data block.
				uint16_t tCacheLineSize = (pCacheFeatures.at(pCurrentCache).at(FEATURE_SELECT::LINESIZE)*
										pCacheFeatures.at(pCurrentCache).at(FEATURE_SELECT::DATAWIDTH));

				//Temporary variable to store stream bitwidth depending on cache targets.
				uint16_t tStreamDataWidth{1};

				switch (pCurrentCache)
				{
					case CACHE_TYPE::DATA_INPUT:
					case CACHE_TYPE::DATA_OUTPUT:
						tStreamDataWidth = cgra::cDataValueBitwidth;
						break;
					case CACHE_TYPE::CONF_PE:
					case CACHE_TYPE::CONF_CC:
						tStreamDataWidth = cgra::cDataStreamBitWidthConfCaches;
						break;
					default:
						tCacheLineSize = 0;
						break;
				}

				if(tCacheLineSize % tStreamDataWidth)
					pNumOfTransmission = tCacheLineSize / tStreamDataWidth;
				else
					pNumOfTransmission = (tCacheLineSize / tStreamDataWidth) + 1;

				//Calculate address step with for block data transfers
				pAddressStepSize = tStreamDataWidth / (8 * sizeof(memory_size_type_t));

				pState = STATES::PROCESS;
				cache_place.write(0);
				pPlace.write(0);
			}
			else
			{
				pBlockTransmission = false;
				pNumOfTransmission = 1;

				pState = STATES::VALIDATE;
			}

			break;

		case STATES::VALIDATE:

			if (pCurrentCache == CACHE_TYPE::DATA_INPUT || pCurrentCache == CACHE_TYPE::DATA_OUTPUT)
			{
				uint16_t tmaxPlaces = pCacheFeatures.at(pCurrentCache).at(FEATURE_SELECT::LINESIZE);
				if(tmaxPlaces > pPlace.read().to_uint())
				{
					pState = STATES::PROCESS;
					cache_place.write(pPlace.read());
				}
				else
				{
					ready.write(true);
					SC_REPORT_WARNING("MMU Transmission Error", "Selected Place out of range at selected cache type.");
					pState = STATES::AWAIT;
				}
			}
			else
				pState = STATES::PROCESS;

			break;

		case STATES::PROCESS:
			switch (pCurrentCache)
			{
				case CACHE_TYPE::DATA_OUTPUT:
					pState = STATES::WRITE_EN;
					break;
				case CACHE_TYPE::CONF_CC:
				case CACHE_TYPE::CONF_PE:
				case CACHE_TYPE::DATA_INPUT:
					pState = STATES::WRITE_DATA;
					break;
				default:
					pState=STATES::AWAIT;
					SC_REPORT_WARNING("MMU State Machine", "Unknown cache type. Await for new start signal.");
					break;
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
					if(pBlockTransmission)
						pState = STATES::BLOCK;
					else
					{
						ready.write(true);
						pState = STATES::AWAIT;
						pNumOfTransmission = 0;
					}
				}
			}
			break;

		case STATES::READ_DATA:
			process_data_output();
			write_enable.write(false);
			if(pBlockTransmission)
				pState = STATES::BLOCK;
			else
			{
				ready.write(true);
				pState = STATES::AWAIT;
				pNumOfTransmission = 0;
			}
			break;

		case STATES::BLOCK:
		{
			uint16_t tAddress = pAddress.read().to_uint() + pAddressStepSize;
			pAddress.write(tAddress);
			if(!(--pNumOfTransmission))
				pBlockTransmission = false;

			uint16_t tPlace = static_cast<uint16_t>(pPlace.read().to_uint());
			cache_place.write(++tPlace);
			pPlace.write(tPlace);
			pState = STATES::PROCESS;
		}
			break;

		default:
			pState=STATES::AWAIT;
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
		/*
		 * Hint to magic number 8: Sizeof returns the size of a data type in number of bytes.
		 * The bitwidth of a data connections is set in number of bits. Thus, a previous devision by
		 * 8 calculates a data path size in the number of bytes.
		 */
		memcpy(&tvalue, pCurrentMemPtr, (cgra::cDataValueBitwidth / (8 * sizeof(memory_size_type_t))));
		data_value_out_stream.write(tvalue);
	}

	return;
}

void MMU::process_configuration()
{

	/*
	 * Hint to magic number 8: Sizeof returns the size of a data type in number of bytes.
	 * The bitwidth of a data connections is set in number of bits. Thus, a previous devision by
	 * 8 calculates a data path size in the number of bytes.
	 */
	if(pAddress.read().to_uint() + (cgra::cDataStreamBitWidthConfCaches / (8 * sizeof(memory_size_type_t))) >= cgra::cMemorySize)
		SC_REPORT_WARNING("MMU Transmission Error", "Addressed value out of memory.");
	else
	{
		//Temporary variable for data to write to data stream
		conf_stream_type_t tvalue{0};

		pCurrentMemPtr = pMemStartPtr + pAddress.read().to_uint();
		/*
		 * Hint to magic number 8: Sizeof returns the size of a data type in number of bytes.
		 * The bitwidth of a data connections is set in number of bits. Thus, a previous devision by
		 * 8 calculates a data path size in the number of bytes.
		 */
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
