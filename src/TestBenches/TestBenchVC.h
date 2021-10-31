/*
 * TestBenchVC.h
 *
 *  Created on: Aug 8, 2017
 *      Author: andrewerner
 */

#ifndef HEADER_TESTBENCHVC_H_
#define HEADER_TESTBENCHVC_H_

#include <systemc>
#include <iostream>
#include "../VirtualChannel.h"

namespace cgra {

/*!
 * \class TestBenchVC
 *
 * \brief Test bench for a virtual channel (VC)
 *
 * \details
 * This test bench creates is for a VC with four
 * inputs and 8 outputs. It creates a configuration
 * where each input is linked to two outputs.
 * Then the configuration is rotated to left.
 * The data inputs and corresponding valid signals
 * are fixed during this tests.
 * Afterwards, the configuration is fixed and a set of
 * input values are changed. Moreover, the reset-signal
 * is used to test reset functionality.
 */
class TestBenchVC: public sc_core::sc_module {
public:
    typedef typename VirtualChannel<4, 8, 8, 8, 2>::conf_type_t conftype_t;
    typedef typename VirtualChannel<4, 8, 8, 8, 2>::enables_type_t enables_type_t;
    typedef typename VirtualChannel<4, 8, 8, 8, 2>::reset_type_t reset_type_t;
    typedef typename VirtualChannel<4, 8, 8, 8, 2>::valid_type_t valid_type_t;
    typedef typename VirtualChannel<4, 8, 8, 8, 2>::input_type_t input_type_t;
    typedef typename VirtualChannel<4, 8, 8, 8, 2>::output_type_t output_type_t;

#ifndef GSYSC
    //Entity Ports
    sc_core::sc_out<conftype_t> s_config{"vcConfig"};
    sc_core::sc_out<reset_type_t> s_rst{"reset"};
    std::array<sc_core::sc_in<enables_type_t>, 8> r_enables;
    std::array<sc_core::sc_in<output_type_t>, 8> r_outData;
    std::array<sc_core::sc_out<input_type_t>, 4> s_inData;
    std::array<sc_core::sc_out<valid_type_t>, 4> s_inValid;
#else
    //Entity Ports
    sc_out<conftype_t> s_config{"vcConfig"};
    sc_out<reset_type_t> s_rst{"reset"};
    std::array<sc_in<enables_type_t>, 8> r_enables;
    std::array<sc_in<output_type_t>, 8> r_outData;
    std::array<sc_out<input_type_t>, 4> s_inData;
    std::array<sc_out<valid_type_t>, 4> s_inValid;
#endif

    //Constructors
    SC_HAS_PROCESS(TestBenchVC);
    /*!
     * \brief Named Constructor
     *
     * \param[out] nameA SystemC module name of TestBench
     */
    TestBenchVC(sc_core::sc_module_name nameA);
    /*!
     * \brief Destructor (defaulted)
     */
    virtual ~TestBenchVC() = default;

    /*!
     * \brief Stimuli process to test a VC
     */
    void stimuli();

    /*!
     * \brief Initialize output signals
     */
    virtual void end_of_elaboration() override;

    /*!
     * \brief Return kind of SystemC module
     */
    virtual const char* kind() const override
    { return "Test bench for a virtual channel"; }

    /*!
     * \brief print current signal values
     *
     * \param[out] os Output stream to write information to
     */
    void dump(std::ostream& os = ::std::cout) const override;

private:
    //Forbidden Constructors
    TestBenchVC() = delete;
    TestBenchVC(const TestBenchVC& src) = delete;                //!< \brief because sc_module could not be copied
    TestBenchVC& operator=(const TestBenchVC& src) = delete;    //!< \brief because move not implemented for sc_module
    TestBenchVC(TestBenchVC&& src) = delete;                    //!< \brief because sc_module could not be copied
    TestBenchVC& operator=(TestBenchVC&& src) = delete;            //!< \brief because move not implemented for sc_module

};

} /* namespace cgra */

#endif /* HEADER_TESTBENCHVC_H_ */
