/*
 * General_Mux.h
 *
 *  Created on: Nov 18, 2018
 *      Author: andrewerner
 */

#ifndef HEADER_GENERAL_MUX_H_
#define HEADER_GENERAL_MUX_H_

#include <systemc>
#include <cstdint>
#include <iostream>
#include "Typedef.h"

#ifdef GSYSC
#include <gsysc.h>
#endif

namespace cgra {

/*!
 * \class General_Mux
 *
 * \brief Contribute arbitrary signals of different sources to one target
 *
 * \details
 * With select input one can contribute arbitrary signals of different sources to one target.
 * The value type is a template parameter as well as the number of sources.
 * The bitwidth for select input is calculated automatically and depends on the number
 * of sources. Signal type for data output and all data inputs are identically.
 *
 * \tparam T Signal type for data input and data output
 * \tparam N Number of sources for data input signals
 * \tparam B Bitwidth for select signal
 */
template<typename T, uint16_t N, uint16_t B = cgra::calc_bitwidth(N)>
class General_Mux : public sc_core::sc_module
{
public:
    //public type definitions
    typedef T value_type_t;
    //!< \brief Value type for multiplexed data.
    typedef sc_dt::sc_lv<B> select_type_t;
    //!< \brief Type for select port of Multiplexer

#ifndef GSYSC
    //Module ports
    sc_core::sc_vector<sc_core::sc_in<value_type_t>> inputs{"InputPorts", N};
    //!< \brief These are the multiplexed inputs, selected by select-input.
    sc_core::sc_in<select_type_t> select{"select"};
    //!< \brief Select input to choose an input port for output data.
    sc_core::sc_out<value_type_t> output{"output"};
    //!< \brief Data output port
#else
    //Module ports
    sc_vector<sc_in<value_type_t>> inputs{"InputPorts", N};
    //!< \brief These are the multiplexed inputs, selected by select-input.
    sc_in<select_type_t> select{"select"};
    //!< \brief Select input to choose an input port for output data.
    sc_out<value_type_t> output{"output"};
    //!< \brief Data output port
#endif

    //Ctor
    SC_HAS_PROCESS(General_Mux);
    /*!
     * \brief General constructor for Multiplexer.
     *
     * \param[in] nameA Unique module name
     */
    General_Mux(const sc_core::sc_module_name& nameA):
        sc_core::sc_module(nameA)
    {
        SC_METHOD(multiplex);
        sensitive << select.value_changed();
        for(uint16_t idx = 0; N > idx; ++idx) {
            sensitive << inputs.at(idx).value_changed();
#ifdef GSYSC
            RENAME_PORT(inputs.at(idx), (cgra::create_name<std::string, uint32_t>("p_gen_in",idx)));
#endif
        }
        return;
    }

    /*!
     * \brief Initialize outputs of General_Mux
     */
    virtual void end_of_elaboration() override
    {
        output.write(0);
        return;
    }

    /*!
     * \brief Print kind of SystemC-module
     */
    virtual const char* kind() const override {
        return "General_Mux";
    }

    /*!
     * \brief Dump Multiplexer information
     *
     * \param[out] os Define used outstream [default: std::cout]
     */
    virtual void dump(std::ostream& os = std::cout) const override
    {
        os << name() << ": " << kind() << std::endl;
        os << "Data output:\t\t" << output.read().to_string(sc_dt::SC_HEX, true) << std::endl;
        os << "Select:\t\t\t" << select.read().to_uint() << std::endl;

        os << "Input content\n";
        os << "=============\n" << std::endl;

        for(auto& out : inputs)
            os << out.basename() << ":\t\t" << out.read().to_string(sc_dt::SC_HEX, true) << std::endl;
    }


    /*!
     * \brief Multiplex input data to output port
     *
     * \details
     * The function calculates the modulo of the select port value.
     * Thus, different numbers will address the same port. This is because
     * the data cache types are specified in MMU, but not all Multiplexers
     * have all inputs.
     */
    void multiplex()
    {
        //Get current selected value from select input.
        uint16_t t_select = static_cast<uint16_t>(select.read().to_uint()) % N;

        if(N <= t_select)
            SC_REPORT_WARNING("General_Mux", "Selected input out of range. Selection for input left unchanged.");
        else
            output.write(inputs.at(t_select).read());

        return;
    }

    /*!
     * \brief Destructor
     */
    virtual ~General_Mux() = default;

private:
    //Deleted Constructors
    General_Mux() = delete;
    General_Mux(const General_Mux& src) = delete;                 //!< \brief Because sc_module could not be copied
    General_Mux(General_Mux&& src) = delete;                     //!< \brief Move semantic not implemented for sc_module
    General_Mux& operator=(const General_Mux& src) = delete;     //!< \brief Because sc_module could not be copied
    General_Mux& operator=(General_Mux&& src) = delete;         //!< \brief Move semantic not implemented for sc_module

}; /* end module General_Mux */

/*!
 * \brief Special version for boolean data type
 *
 * \details
 * With select input one can contribute arbitrary signals of different sources to one target.
 * The value type is a template parameter as well as the number of sources.
 * The bitwidth for select input is calculated automatically and depends on the number
 * of sources. Signal type for data output and all data inputs are identically.
 *
 * \tparam N Number of sources for data input signals
 * \tparam B Bitwidth for select signal
 */
template<uint16_t N, uint16_t B >
class General_Mux<bool, N, B> : sc_core::sc_module
{
public:
    //public type definitions
    typedef bool value_type_t;
    //!< \brief Value type for multiplexed data.
    typedef sc_dt::sc_lv<B> select_type_t;
    //!< \brief Type for select port of Multiplexer

#ifndef GSYSC
    //Module ports
    sc_core::sc_vector<sc_core::sc_in<value_type_t>> inputs{"InputPorts", N};
    //!< \brief These are the multiplexed inputs, selected by select-input.
    sc_core::sc_in<select_type_t> select{"select"};
    //!< \brief Select input to choose an input port for output data.
    sc_core::sc_out<value_type_t> output{"output"};
    //!< \brief Data output port
#else
    //Module ports
    sc_vector<sc_in<value_type_t>> inputs{"InputPorts", N};
    //!< \brief These are the multiplexed inputs, selected by select-input.
    sc_in<select_type_t> select{"select"};
    //!< \brief Select input to choose an input port for output data.
    sc_out<value_type_t> output{"output"};
    //!< \brief Data output port
#endif

    //Ctor
    SC_HAS_PROCESS(General_Mux);
    /*!
     * \brief General constructor for Multiplexer.
     *
     * \param[in] nameA Unique module name
     */
    General_Mux(const sc_core::sc_module_name& nameA):
        sc_core::sc_module(nameA)
    {
        SC_METHOD(multiplex);
        sensitive << select.value_changed();
        for(uint16_t idx = 0; N > idx; ++idx) 
            sensitive << inputs.at(idx).value_changed();
        
        return;
    }

    /*!
     * \brief Initialize outputs of General_Mux
     */
    virtual void end_of_elaboration() override
    {
        output.write(false);

        return;
    }

    /*!
     * \brief Print kind of SystemC-module
     */
    virtual const char* kind() const override {
        return "General_Mux";
    }

    /*!
     * \brief Dump Multiplexer information
     *
     * \param[out] os Define used outstream [default: std::cout]
     */
    virtual void dump(std::ostream& os = std::cout) const override
    {
        os << name() << ": " << kind() << std::endl;
        os << "Data out:\t\t" << std::hex << output.read() << std::endl;
        os << "Select:\t\t\t" << select.read().to_uint() << std::endl;

        os << "Input content\n";
        os << "=============\n" << std::endl;

        for(auto& out : inputs)
            os << out.basename() << ":\t\t" << std::hex <<out.read() << std::endl;
    }


    /*!
     * \brief Multiplex input data to output port
     *
     * \details
     * The function calculates the modulo of the select port value.
     * Thus, different numbers will address the same port. This is because
     * the data cache types are specified in MMU, but not all Multiplexers
     * have all inputs.
     */
    void multiplex()
    {
        //Get current selected value from select input.
        uint16_t t_select = static_cast<uint16_t>(select.read().to_uint()) % N;

        if(N <= t_select)
            SC_REPORT_WARNING("General_Mux", "Selected output out of range. Selection for output left unchanged.");
        else
            output.write(inputs.at(t_select).read());

        return;
    }

    /*!
     * \brief Destructor
     */
    virtual ~General_Mux() = default;

private:
    //Deleted Constructors
    General_Mux() = delete;
    General_Mux(const General_Mux& src) = delete;                 //!< \brief Because sc_module could not be copied
    General_Mux(General_Mux&& src) = delete;                     //!< \brief Move semantic not implemented for sc_module
    General_Mux& operator=(const General_Mux& src) = delete;     //!< \brief Because sc_module could not be copied
    General_Mux& operator=(General_Mux&& src) = delete;         //!< \brief Move semantic not implemented for sc_module

}; /* end module General_Mux boolean*/

} /* end namespace cgra */

#endif /* HEADER_GENERAL_MUX_H_ */
