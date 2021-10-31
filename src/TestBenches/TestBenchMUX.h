/*
 * TestBenchMUX.h
 *
 * Created on: Aug 4, 2017
 * Author: andrewerner
 */

#ifndef HEADER_TESTBENCHMUX_H_
#define HEADER_TESTBENCHMUX_H_

#include <systemc>
#include <cstdint>
#include <array>
#include "../Multiplexer.h"

namespace cgra {

/*!
 * \class TestBench_MUX
 *
 * \brief Test bench to test multiplexer module
 *
 * \details
 * Create stimuli signals for data ports of Multiplexer module
 */
class TestBench_MUX: public sc_core::sc_module {
public:

#ifndef GSYSC
    //Entity Ports
    sc_core::sc_in<Multiplexer<4,3>::data_type_t> r_sel_data{"selected_data"};
    //!< \brief result of selected data input
    sc_core::sc_in<Multiplexer<4,3>::valid_type_t> r_sel_valid{"selected_valid"};
    //!< \brief result of selected valid input
    std::array<sc_core::sc_out<Multiplexer<4,3>::data_type_t>, 4> s_mux_in;
    //!< \brief vector of mux-data-stimuli-ports
    std::array<sc_core::sc_out<Multiplexer<4,3>::valid_type_t>, 4> s_valid_in;
    //!< \brief vector of mux-valid-stimuli-ports
    sc_core::sc_out<Multiplexer<4,3>::select_type_t> s_select{"s_select"};
    //!< \brief select stimuli for mux
#else
    //Entity Ports
    sc_in<Multiplexer<4,3>::data_type_t> r_sel_data{"selected_data"};
    //!< \brief result of selected data input
    sc_in<Multiplexer<4,3>::valid_type_t> r_sel_valid{"selected_valid"};
    //!< \brief result of selected valid input
    std::array<sc_out<Multiplexer<4,3>::data_type_t>, 4> s_mux_in;
    //!< \brief vector of mux-data-stimuli-ports
    std::array<sc_out<Multiplexer<4,3>::valid_type_t>, 4> s_valid_in;
    //!< \brief vector of mux-valid-stimuli-ports
    sc_out<Multiplexer<4,3>::select_type_t> s_select{"s_select"};
    //!< \brief select stimuli for mux
#endif

    //CTOR
    SC_HAS_PROCESS(TestBench_MUX);
    /*!
     * \brief General Constructor
     *
     * \param[in] nameA Name of the SystemC module
     */
    TestBench_MUX(const sc_core::sc_module_name& nameA);
    //!< \brief Ctor
    virtual ~TestBench_MUX() = default;
    //!< \brief Dtor

    //stimuli process
    void stimuli();
    //!< \brief Stimuli process for Multiplexer module
};

} /* namespace cgra */

#endif /* HEADER_TESTBENCHMUX_H_ */
