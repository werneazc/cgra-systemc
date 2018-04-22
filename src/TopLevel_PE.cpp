#include "TopLevel_PE.h"

unsigned int cgra::TopLevel::pe_generation_counter = 0;

cgra::TopLevel::TopLevel(sc_core::sc_module_name nameA) : sc_core::sc_module(nameA) {

	using namespace cgra;

	pe = new Processing_Element<8,8>("PE_DUT", pe_generation_counter++);
	tb = new TestBench_PE();

	pe->clk.bind(s_clk);
	pe->conf.bind(s_op);
	tb->s_op.bind(s_op);
	pe->enable[0].bind(s_enable[0]);
	pe->enable[1].bind(s_enable[1]);
	tb->s_enable[0].bind(s_enable[0]);
	tb->s_enable[1].bind(s_enable[1]);
	pe->in1.bind(s_in1);
	pe->in2.bind(s_in2);
	pe->res.bind(s_res);

	tb->s_valid.bind(s_valid);
	pe->valid.bind(s_valid);
	tb->s_in1.bind(s_in1);
	tb->s_in2.bind(s_in2);
	tb->s_res.bind(s_res);

}

cgra::TopLevel::~TopLevel() {
	delete pe;
	delete tb;
}
