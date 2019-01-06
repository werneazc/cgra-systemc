/*
 * MU_TB.cpp
 *
 *  Created on: Jan 5, 2019
 *      Author: andrewerner
 */
#include "MU_TB.h"

cgra::TB_MU::TB_MU(const sc_core::sc_module_name& nameA)
{
	SC_THREAD(stimuli);

	SC_THREAD(simulate_vcgra);
	sensitive << start.pos();

	SC_THREAD(simulate_mmu);
	sensitive << mmu_start.pos();
}

void cgra::TB_MU::end_of_elaboration()
{
	run.write(false);
	pause.write(false);
	rst.write(true);
	ready.write(false);
	mmu_ready.write(true);
}

void cgra::TB_MU::dump(std::ostream& os) const
{

	os << name() << ": " << kind() << std::endl;
	os << "Current start status:\t\t" << std::setw(3) << std::boolalpha << start.read() << std::endl;
	os << "Current ready status:\t\t" << std::setw(3) << std::boolalpha << ready.read() << std::endl;
	os << "Current run status:\t\t" << std::setw(3) << std::boolalpha << run.read() << std::endl;
	os << "Current pause status:\t\t" << std::setw(3) << std::boolalpha << pause.read() << std::endl;
	os << "Current reset status:\t\t" << std::setw(3) << std::boolalpha << rst.read() << std::endl;
	os << "Current MMU start status:\t" << std::setw(3) << std::boolalpha << mmu_start.read() << std::endl;
	os << "Current MMU ready status:\t" << std::setw(3) << std::boolalpha << mmu_ready.read() << std::endl;

	os << "Current Address:\t\t" << std::setw(3) << address.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Current place:\t\t\t" << std::setw(3) << place.read().to_string(sc_dt::SC_DEC, false) << std::endl;
	os << "Current Cache:\t\t" << std::setw(3) << cache_select.read().to_string(sc_dt::SC_HEX) << std::endl;

	os << "Current line selections for caches:/n";
	os << "===================================" << std::endl;
	os << "Data Input Cache: Select in: " << dic_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << dic_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
	os << "Data Output Cache: Select in: " << doc_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << doc_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
	os << "PE Configuration Cache: Select in: " << pe_cc_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << pe_cc_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
	os << "vCh Configuration Cache: Select in: " << ch_cc_select_lines.first.read().to_string(sc_dt::SC_DEC);
	os << "; Select out: " << ch_cc_select_lines.second.read().to_string(sc_dt::SC_DEC) << std::endl;
}

void cgra::TB_MU::stimuli(void)
{
	wait(10, sc_core::SC_NS);
	run.write(true);
	wait(finish.posedge_event());
	wait(finish.negedge_event());
	run.write(false);
	rst.write(false);
	wait(4, sc_core::SC_NS);
	rst.write(true);
	run.write(true);
	wait(100, sc_core::SC_NS);
	pause.write(true);
	wait(100, sc_core::SC_NS);
	pause.write(false);
	wait(finish.posedge_event());
	wait(finish.negedge_event());
	run.write(false);
	rst.write(false);
	wait(4, sc_core::SC_NS);
	rst.write(true);

	sc_stop();
}

void cgra::TB_MU::simulate_vcgra(void)
{
	do
	{
		wait();
		wait(250, sc_core::SC_NS);
		ready.write(true);
		wait(4, sc_core::SC_NS);
		ready.write(false);
	}
	while(true);
}

void cgra::TB_MU::simulate_mmu(void)
{
	do
	{
		wait();
		wait(25, sc_core::SC_NS);
		mmu_ready.write(false);
		wait(4, sc_core::SC_NS);
		mmu_ready.write(true);
	}
	while(true);
}
