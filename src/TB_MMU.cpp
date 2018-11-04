/*
 * TB_MMU.cpp
 *
 *  Created on: Nov 3, 2018
 *      Author: andrewerner
 */

#include "TB_MMU.h"
#include "MMU.h"

/*!
 * \brief: Alias for MMU cache type declaration.
 */
using CACHE_TYPE = cgra::MMU::CACHE_TYPE;

cgra::TB_MMU::TB_MMU(const sc_core::sc_module_name& nameA)
{
	SC_THREAD(stimuli);
}

void cgra::TB_MMU::end_of_elaboration()
{
	//Initialize output ports of TB
	sAddress.write(0);
	sPlace.write(0);
	sCache_select.write(CACHE_TYPE::NONE);
	sStart.write(0);
	sPe_sltIn.write(0);
	sPe_sltOut.write(1);
	sVCh_sltIn.write(0);
	sVCh_sltOut.write(1);
	sDic_sltIn.write(0);
	sDic_sltOut.write(1);
	sDoc_sltIn.write(0);
	sDoc_sltOut.write(1);
}

void cgra::TB_MMU::dump(std::ostream& os) const
{
	os << name() << ": " << kind() << std::endl;
	os << "Current cache:\t\t\t";
	switch(static_cast<CACHE_TYPE>(sCache_select.read().to_uint()))
	{
		case CACHE_TYPE::DATA_INPUT:
			os << "Data Input Cache" << std::endl;
			break;
		case CACHE_TYPE::DATA_OUTPUT:
			os << "Data Output Cache" << std::endl;
			break;
		case CACHE_TYPE::CONF_PE:
			os << "PE Configuration Cache" << std::endl;
			break;
		case CACHE_TYPE::CONF_CC:
			os << "vCH Configuration Cache" << std::endl;
			break;
		default:
			os << "Unknown cache type." << std::endl;
			break;
	}
	os << "Current Address:\t\t" << std::setw(3) << sAddress.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Current Place:\t\t\t" << std::setw(3) << sPlace.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Status Start:\t\t\t" << std::setw(3) << std::boolalpha << sStart.read() << std::endl;
	os << "Status Ready:\t\t\t" << std::setw(3) << std::boolalpha << sReady.read() << std::endl;
	os << "PE cache select input:\t\t" << std::setw(3) << sPe_sltIn.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "PE cache select output:\t\t" << std::setw(3) << sPe_sltOut.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "vCh cache select input:\t\t" << std::setw(3) << sVCh_sltIn.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "vCh cache select output:\t" << std::setw(3) << sVCh_sltOut.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Data Input cache select input:\t" << std::setw(3) << sDic_sltIn.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Data Input cache select output:\t" << std::setw(3) << sDic_sltOut.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Data Output cache select input:\t" << std::setw(3) << sDoc_sltIn.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Data Output cache select output:" << std::setw(3) << sDoc_sltOut.read().to_string(sc_dt::SC_DEC, false) << std::endl;

	return;
}

void cgra::TB_MMU::stimuli(void)
{
	//Initial wait
	wait(10, sc_core::SC_NS);

	//Test 1: Test write to DataInCache
	sDic_sltIn.write('1');
	sDic_sltOut.write('0');
	sCache_select.write(MMU::CACHE_TYPE::DATA_INPUT);
	for (int var = 0; var < 8; ++var) {
		sAddress.write(2*var);
		sPlace.write(var);
		sStart.write(true);
		wait(sReady.posedge_event());
		wait(sReady.negedge_event());
		sStart.write(false);
		wait(4, sc_core::SC_NS);
	}

	sDic_sltIn.write('0');
	sDic_sltOut.write('1');

	{
		sc_core::sc_event_and_list and_list;
		for(auto& val : sValues)
			and_list &= val.value_changed_event();
		wait(and_list);
	}

	//Test 2: Block data transfer to DatainCache

	sAddress.write(16);
	sPlace.write(127);
	sStart.write(true);
	wait(sReady.posedge_event());
	wait(sReady.negedge_event());
	sStart.write(false);

	sDic_sltIn.write('1');
	sDic_sltOut.write('0');

	{
		sc_core::sc_event_and_list and_list;
		for(auto& val : sValues)
			and_list &= val.value_changed_event();
		wait(and_list);
	}

	//Test 3: Read values from DataOutCache as block data

	for(uint16_t i = 10, j = 0; 8 > j; ++j, i *= 2)
		sResults.at(j) = i;

	wait(10, sc_core::SC_NS);

	sUpdate.write(true);
	wait(4, sc_core::SC_NS);
	sUpdate.write(false);

	wait(4, sc_core::SC_NS);
	sCache_select.write(MMU::CACHE_TYPE::DATA_OUTPUT);

	sDoc_sltIn.write('1');
	sDoc_sltOut.write('0');
	sAddress.write(32);
	sPlace.write(127);
	wait(4, sc_core::SC_NS);
	sStart.write(true);

	//This is an additional block to test parallel update from VCGRA
	wait(15, sc_core::SC_NS);
	for(uint16_t i = 10, j = 0; 8 > j; ++j, i *= 4)
		sResults.at(j) = i;
	sUpdate.write(1);
	wait(4, sc_core::SC_NS);
	sUpdate.write(false);

	//Update DatInPutCache line in parallel to data load
	wait(4, sc_core::SC_NS);
	sDic_sltIn.write('0');
	sDic_sltOut.write('1');

	wait(sReady.posedge_event());
	wait(sReady.negedge_event());
	sStart.write(false);

	//Test 4: Read value by values from DataOutCache
	sDoc_sltIn.write('0');
	sDoc_sltOut.write('1');
	wait(10, sc_core::SC_NS);
	for (int var = 0, add = 60; var < 8; ++var, add += 2) {
		sAddress.write(add);
		sPlace.write(var);
		sStart.write(true);
		wait(sReady.posedge_event());
		wait(sReady.negedge_event());
		sStart.write(false);
		wait(2, sc_core::SC_NS);
	}

	//Test 5: Write configuration to PE configuration cache

	sCache_select.write(MMU::CACHE_TYPE::CONF_PE);
	sAddress.write(80);
	sPlace.write(127);
	wait(4, sc_core::SC_NS);
	sStart.write(true);
	wait(sReady.posedge_event());
	wait(sReady.negedge_event());
	sStart.write(false);

	sPe_sltIn.write('1');
	sPe_sltOut.write('0');

	//Test 6: Read value by values from DataOutCache

	sCache_select.write(MMU::CACHE_TYPE::CONF_CC);
	sAddress.write(86);
	sPlace.write(127);
	wait(4, sc_core::SC_NS);
	sStart.write(true);
	wait(sReady.posedge_event());
	wait(sReady.negedge_event());
	sStart.write(false);

	sVCh_sltIn.write('1');
	sVCh_sltOut.write('0');

}
