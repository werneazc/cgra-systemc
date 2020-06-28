/*
 * TB_DataInCache.cpp
 *
 *  Created on: Oct 31, 2018
 *      Author: andrewerner
 */

#include <iostream>
#include <iomanip>
#include "TB_DataInCache.h"

cgra::TB_DataInCache::TB_DataInCache(sc_core::sc_module_name nameA) :
sc_core::sc_module(nameA)
{
	SC_THREAD(stimuli);
}

void cgra::TB_DataInCache::stimuli(void)
{
	//Store sDataInStream value locally and modify
	value_type_t tvalue{0};

	wait(10, sc_core::SC_NS);

	//Test1: switch slt_out with slt_out == slt_in
	sSlt_out.write(0);
	wait(10, sc_core::SC_NS);

	//Test2: switch slt_out with slt_out > cacheline size
	sSlt_out.write(4);
	wait(10, sc_core::SC_NS);


	//Test3: switch slt_out with slt_out successfully
	sSlt_out.write(1);
	wait(10, sc_core::SC_NS);

	//Test4: write data to data input cache
	sDataInStream.write(10);
	wait(10, sc_core::SC_NS);
	sWrite.write(true);
	wait(sAck.posedge_event());

	for(uint8_t idx = 1; 4 > idx; ++idx)
	{
		sWrite.write(false);
		wait(sAck.negedge_event());

		sSlt_place.write(idx);
		tvalue = 2 * sDataInStream.read().to_uint();
		sDataInStream.write(tvalue);
		sWrite.write(true);
		wait(sAck.posedge_event());
	}

	//Test5: slt_in == slt_out + write
	sWrite.write(false);
	wait(sAck.negedge_event());
	sSlt_in.write(1);
	sWrite.write(true);
	wait(sAck.posedge_event());
	sWrite.write(false);
	sSlt_in.write(0);
	wait(sAck.negedge_event());

//	//Test6: slt_place > # of place in a cache line
//	sSlt_place.write(4);
//	sWrite.write(true);
//	wait(sAck.posedge_event());
//	sWrite.write(false);
//	sSlt_place.write(3);
//	wait(sAck.negedge_event());
//
//	//Test7: slt_in > # of cache lines
//	sSlt_in.write(4);
//	sWrite.write(true);
//	wait(sAck.posedge_event());
//	sWrite.write(false);
//	sSlt_in.write(0);
//	wait(sAck.negedge_event());

	//Test8: change current values
	sSlt_in.write(1);
	sSlt_out.write(0);
	wait(10, sc_core::SC_NS);

	//Test9: write data to data input cache
	sSlt_place.write(0);
	sDataInStream.write(160);
	wait(10, sc_core::SC_NS);
	sWrite.write(true);
	wait(sAck.posedge_event());

	for(uint8_t idx = 1; 4 > idx; ++idx)
	{
		sWrite.write(false);
		wait(sAck.negedge_event());

		sSlt_place.write(idx);
		tvalue = 2 * sDataInStream.read().to_uint();
		sDataInStream.write(tvalue);
		sWrite.write(true);
		wait(sAck.posedge_event());
	}

	//Test10: change current values
	sSlt_in.write(0);
	sSlt_out.write(1);
	wait(10, sc_core::SC_NS);
}

void cgra::TB_DataInCache::end_of_elaboration()
{
	//Initialize start values for simulation.
	sSlt_in.write(0);
	sSlt_out.write(1);
	sSlt_place.write(0);
	sWrite.write(false);
	sDataInStream.write(0);

}

void cgra::TB_DataInCache::dump(std::ostream& os) const
{

		os << name() << "\t\t" << kind() << std::endl;
		os << "Bitwidth serial input:\t\t\t" << std::setw(3) << static_cast<uint32_t>(sDataInStream.read().length()) << std::endl;
		os << "Content serial input:\t\t\t" << sDataInStream.read().to_string(sc_dt::SC_HEX, true) << std::endl;
		os << "Selected input cache line:\t\t" << std::setw(3) << sSlt_in.read().to_string(sc_dt::SC_DEC) << std::endl;
		os << "Selected cache line place:\t\t" << std::setw(3) << sSlt_place.read().to_string(sc_dt::SC_DEC) << std::endl;
		os << "Selected output cache line:\t\t" << std::setw(3) << sSlt_out.read().to_string(sc_dt::SC_DEC) << std::endl;

		return;
}
