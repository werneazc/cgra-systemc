
#include "TestBench_PE.h"
#include "ProcessingElement.h"



std::ostream& operator<<(std::ostream& os, const cgra::TestBench& tb)
{

	os << "Signal " << tb.s_op.name() << ":\t\t" << std::hex << tb.s_op.read() << "\n";
	os << "Signal " << tb.s_in1.name() << ":\t\t" << std::dec << tb.s_in1.read() << "\n";
	os << "Signal " << tb.s_in2.name() << ":\t\t" << std::dec << tb.s_in2.read() << "\n";
	os << "Signal " << tb.s_res.name() << ":\t\t" << std::dec << tb.s_res.read() << "\n";
	os << "Signal " << tb.s_valid.name() << ":\t\t" << std::hex << tb.s_valid.read() << "\n";
	os << "Signal " << tb.s_enable.name() << ":\t" << tb.s_enable.read().to_string() << std::endl;

	return os;
}

cgra::TestBench::TestBench(): sc_core::sc_module(sc_core::sc_module_name("TestBench_PE")) {
	SC_THREAD(stimuli);
	sensitive << s_valid.pos();

	s_in1.initialize(5);
	s_in2.initialize(5);
	s_op.initialize(Processing_Element<8,8>::OP::ADD);
	s_enable.initialize("00");
}

void cgra::TestBench::stimuli() {
	wait(10, sc_core::SC_NS);
	s_enable.write("10");
	std::cout << "@" << sc_core::sc_time_stamp() << " Disabled" << std::endl;
	print();

	wait(5, sc_core::SC_NS);
	s_enable.write("01");
	std::cout << "@" << sc_core::sc_time_stamp() << " Disabled"  << std::endl;
	print();

	wait(5, sc_core::SC_NS);
	s_enable.write("11");
	std::cout << "@" << sc_core::sc_time_stamp() << " Disabled"  << std::endl;
	print();

	wait();
	s_op.write(Processing_Element<8,8>::OP::MUL);
	std::cout << "@" << sc_core::sc_time_stamp() << " ADD"  << std::endl;
	print();

	wait();
	s_op.write(Processing_Element<8,8>::OP::IDIV);
	std::cout << "@" << sc_core::sc_time_stamp() << " MUL"  << std::endl;
	print();

	wait();
	s_op.write(Processing_Element<8,8>::OP::SUB);
	std::cout << "@" << sc_core::sc_time_stamp() << " IDIV"  << std::endl;
	print();

	wait();
	s_op.write(Processing_Element<8,8>::OP::GRE);
	std::cout << "@" << sc_core::sc_time_stamp() << " SUB"  << std::endl;
	print();

	wait();
	s_op.write(Processing_Element<8,8>::OP::EQU);
	std::cout << "@" << sc_core::sc_time_stamp() << " GRE"  << std::endl;
	print();

	wait();
	s_op.write(Processing_Element<8,8>::OP::BUF);
	std::cout << "@" << sc_core::sc_time_stamp() << " EQU"  << std::endl;
	print();


	wait();
	std::cout << "@" << sc_core::sc_time_stamp() << " BUF"  << std::endl;
	print();
	sc_core::sc_stop();

}

void cgra::TestBench::print() {

	using namespace std;

	cout << "Signal " << s_op.name() << ":\t\t" << std::hex << s_op.read() << "\n";
	cout << "Signal " << s_in1.name() << ":\t\t" << std::dec << s_in1.read() << "\n";
	cout << "Signal " << s_in2.name() << ":\t\t" << std::dec << s_in2.read() << "\n";
	cout << "Signal " << s_res.name() << ":\t\t" << std::dec << s_res.read() << "\n";
	cout << "Signal " << s_valid.name() << ":\t\t" << std::hex << s_valid.read() << "\n";
	cout << "Signal " << s_enable.name() << ":\t" << s_enable.read().to_string() << endl;
}
