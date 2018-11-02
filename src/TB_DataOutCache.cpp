/*
 * TB_DataInCache.cpp
 *
 *  Created on: Oct 31, 2018
 *      Author: andrewerner
 */

#include <iostream>
#include <iomanip>
#include "TB_DataOutCache.h"

cgra::TB_DataOutCache::TB_DataOutCache(sc_core::sc_module_name nameA) :
sc_core::sc_module(nameA)
{
	SC_THREAD(stimuli);
}

void cgra::TB_DataOutCache::stimuli(void)
{
	//Store sDataInStream value locally and modify
	value_type_t tvalue{10};

	wait(10, sc_core::SC_NS);

	//Test1: update cache line
	for(auto& val : sCurrentResults)
	{
		val.write(tvalue);
		tvalue *= 2;
	}
	sLoad.write(true);
	wait(1, sc_core::SC_NS);
	sUpdate.write(true);
	wait(sAck.posedge_event());
	sLoad.write(false);
	wait(sAck.negedge_event());


	//Test2: change slt_in while update true
	sSlt_in.write(1);
	wait(7, sc_core::SC_NS);

	//Test3: change slt_in while update false
	sUpdate.write(false);
	wait(4, sc_core::SC_NS);

	//Test4: update && load
	for(auto& val : sCurrentResults)
	{
		val.write(tvalue);
		tvalue *= 2;
	}
	sUpdate.write(true);
	wait(3, sc_core::SC_NS);
	sLoad.write(true);

	//Test5: stream data to shared memory
	sUpdate.write(false);
	wait(sAck.posedge_event());

	for(uint8_t i = 1; 4 > i; ++i)
	{
		sLoad.write(false);
		wait(sAck.negedge_event());

		sSlt_place.write(i);
		sLoad.write(true);
		wait(sAck.posedge_event());
	}
	sLoad.write(false);
	wait(sAck.negedge_event());

	//Test6: change slt_in while load true
	sLoad.write(true);
	sSlt_in.write(0);
	wait(4, sc_core::SC_NS);
	sLoad.write(false);

	//Test7: stream data to shared memory
	sSlt_out.write(1);
	sSlt_place.write(0);
	sLoad.write(true);
	wait(sAck.posedge_event());

	for(uint8_t i = 1; 4 > i; ++i)
	{
		sLoad.write(false);
		wait(sAck.negedge_event());

		sSlt_place.write(i);
		sLoad.write(true);
		if(i == 1)
		{
			for(auto& val : sCurrentResults)
			{
				val.write(tvalue);
				tvalue *= 2;
			}
			sUpdate.write(true);
		}
		wait(sAck.posedge_event());
		if(i == 2)
			sUpdate.write(false);
	}
	sLoad.write(false);
	wait(sAck.negedge_event());

}

void cgra::TB_DataOutCache::end_of_elaboration()
{
	//Initialize start values for simulation.
	sSlt_in.write(0);
	sSlt_out.write(0);
	sSlt_place.write(0);
	sUpdate.write(false);
	sLoad.write(false);

	for(auto& val : sCurrentResults)
		val.write(0);

}

void cgra::TB_DataOutCache::dump(std::ostream& os) const
{

		os << name() << "\t\t" << kind() << std::endl;
		os << "Bitwidth serial input:\t\t\t" << std::setw(3) << static_cast<uint32_t>(sDataOutStream.read().length()) << std::endl;
		os << "Content serial output:\t\t\t" << sDataOutStream.read().to_string(sc_dt::SC_HEX, true) << std::endl;
		os << "Selected input cache line:\t\t" << std::setw(3) << sSlt_in.read().to_string(sc_dt::SC_DEC, false) << std::endl;
		os << "Selected cache line place:\t\t" << std::setw(3) << sSlt_place.read().to_string(sc_dt::SC_DEC, false) << std::endl;
		os << "Selected output cache line:\t\t" << std::setw(3) << sSlt_out.read().to_string(sc_dt::SC_DEC, false) << std::endl;

		return;
}
