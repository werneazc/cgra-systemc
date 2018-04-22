
#include <TestBenchPE_PE.h>
#include "ProcessingElement.h"



std::ostream& operator<<(std::ostream& os, const cgra::TestBench_PE& tb)
{

	os << tb.name() << ":\t" << tb.kind() << std::endl;
	os << "================================================================================\n";
	os << "Signal " << tb.s_op.name() << ":\t\t" << std::hex << tb.s_op.read() << "\n";
	os << "Signal " << tb.s_in1.name() << ":\t\t" << std::dec << tb.s_in1.read() << "\n";
	os << "Signal " << tb.s_in2.name() << ":\t\t" << std::dec << tb.s_in2.read() << "\n";
	os << "Signal " << tb.s_res.name() << ":\t\t" << std::dec << tb.s_res.read() << "\n";
	os << "Signal " << tb.s_valid.name() << ":\t\t" << std::hex << tb.s_valid.read() << "\n";
	os << "Signal " << "s_enables" << ":\t" << (tb.s_enable[0].read() ? "1" : "0");
	os << (tb.s_enable[1].read() ? "1" : "0") << std::endl;

	return os;
}

cgra::TestBench_PE::TestBench_PE(): sc_core::sc_module(sc_core::sc_module_name("TestBench_PE")) {
	SC_THREAD(stimuli);
	sensitive << s_valid.pos();

	s_in1.initialize(5);
	s_in2.initialize(5);
	s_op.initialize(Processing_Element<8,8>::OP::ADD);
	s_enable[0].initialize(false);
	s_enable[1].initialize(false);
}

void cgra::TestBench_PE::stimuli() {
	wait(10, sc_core::SC_NS);
	s_enable[1].write(true);
//	std::cout << "@" << sc_core::sc_time_stamp() << " Disabled" << std::endl;
//	dump();

	wait(5, sc_core::SC_NS);
	s_enable[1].write(false);
	s_enable[0].write(true);
//	std::cout << "@" << sc_core::sc_time_stamp() << " Disabled"  << std::endl;
//	dump();

	wait(5, sc_core::SC_NS);
	s_enable[1].write(true);
	s_enable[0].write(true);
//	std::cout << "@" << sc_core::sc_time_stamp() << " Disabled"  << std::endl;
//	dump();

	wait();
	s_op.write(Processing_Element<8,8>::OP::MUL);
//	std::cout << "@" << sc_core::sc_time_stamp() << " ADD"  << std::endl;
//	dump();

	wait();
	s_op.write(Processing_Element<8,8>::OP::IDIV);
//	std::cout << "@" << sc_core::sc_time_stamp() << " MUL"  << std::endl;
//	dump();

	wait();
	s_op.write(Processing_Element<8,8>::OP::SUB);
//	std::cout << "@" << sc_core::sc_time_stamp() << " IDIV"  << std::endl;
//	dump();

	wait();
	s_op.write(Processing_Element<8,8>::OP::GRE);
//	std::cout << "@" << sc_core::sc_time_stamp() << " SUB"  << std::endl;
//	dump();

	wait();
	s_op.write(Processing_Element<8,8>::OP::EQU);
//	std::cout << "@" << sc_core::sc_time_stamp() << " GRE"  << std::endl;
//	dump();

	wait();
	s_op.write(Processing_Element<8,8>::OP::BUF);
//	std::cout << "@" << sc_core::sc_time_stamp() << " EQU"  << std::endl;
//	dump();


	wait();
//	std::cout << "@" << sc_core::sc_time_stamp() << " BUF"  << std::endl;
//	dump();
	sc_core::sc_stop();

}

void cgra::TestBench_PE::dump(std::ostream& os) const{


	os << name() << ":\t" << kind() << std::endl;
	os << "================================================================================\n";
	os << "Signal " << s_op.basename() << ":\t\t" << std::hex << s_op.read() << "\n";
	os << "Signal " << s_in1.basename() << ":\t\t" << std::dec << s_in1.read() << "\n";
	os << "Signal " << s_in2.basename() << ":\t\t" << std::dec << s_in2.read() << "\n";
	os << "Signal " << s_res.basename() << ":\t\t" << std::dec << s_res.read() << "\n";
	os << "Signal " << s_valid.basename() << ":\t\t" << std::hex << s_valid.read() << "\n";
	os << "Signal " << "s_enables" << ":\t" << (s_enable[0].read() ? "1" : "0");
	os << (s_enable[1].read() ? "1" : "0") << std::endl;
}
