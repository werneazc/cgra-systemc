/*
 * General_DeMux.h
 *
 *  Created on: Nov 18, 2018
 *      Author: andrewerner
 */

#ifndef HEADER_GENERAL_DEMUX_H_
#define HEADER_GENERAL_DEMUX_H_

#include <systemc>
#include <cstdint>
#include <iostream>
#include "Typedef.h"

#ifdef GSYSC
#include "utils.hpp"
#include <gsysc.h>
#endif

namespace cgra {

/*!
 * \class General_DeMux
 *
 * \brief Contribute an arbitrary signal to different targets
 *
 * \details
 * With select input one can contribute an arbitrary signal to different targets.
 * The value type is a template parameter as well as the number of targets.
 * The bitwidth for select input is calculated automatically and depends on the number
 * of targets. Signal type for data input and all data outputs are identically.
 *
 * \tparam T Signal type for data input and data output
 * \tparam N Number of targets for data output signals
 * \tparam B Bitwidth for select signal
 */
template<typename T, uint16_t N, uint16_t B = cgra::calc_bitwidth(N)>
class General_DeMux : public sc_core::sc_module
{
public:
	//public type definitions
	typedef T value_type_t;
	//!< \brief Value type for demultiplexed data.
	typedef sc_dt::sc_lv<B> select_type_t;
	//!< \brief Type for select port of demultiplexer

	#ifndef GSYSC
		//Module ports
		sc_core::sc_vector<sc_core::sc_out<value_type_t>> outputs{"OutputPorts", N};
		//!< \brief These are the demultiplexed outputs of input data, selected by select-input.
		sc_core::sc_in<select_type_t> select{"select"};
		//!< \brief Select input to choose an output port for input data.
		sc_core::sc_in<value_type_t> input{"input"};
		//!< \brief Data input port which should be distributed to output consumers.
	#else
		//Module ports
	    sc_vector<sc_out<value_type_t>> outputs{"OutputPorts", N};
		//!< \brief These are the demultiplexed outputs of input data, selected by select-input.
		sc_in<select_type_t> select{"select"};
		//!< \brief Select input to choose an output port for input data.
		sc_in<value_type_t> input{"input"};
		//!< \brief Data input port which should be distributed to output consumers.
	#endif

	//Ctor
	SC_HAS_PROCESS(General_DeMux);
	/*!
	 * \brief General constructor for demultiplexer.
	 *
	 * \param[in] nameA Unique module name
	 */
	General_DeMux(const sc_core::sc_module_name& nameA):
		sc_core::sc_module(nameA)
	{
		SC_METHOD(demultiplex);
		sensitive << select.value_changed();

		SC_METHOD(update);
		sensitive << input.value_changed();

		return;
	}

	/*!
	 * \brief Initialize outputs of General_DeMux
	 */
	virtual void end_of_elaboration() override
	{
		for (auto& out : outputs)
			out.write(0);

		m_latestSelect.write(0);

		return;
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "General_DeMux";
	}

	/*!
	 * \brief Dump Demultiplexer information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << "\t\t" << kind() << std::endl;
		os << "Data input:\t\t" << input.read().to_string(sc_dt::SC_HEX, true) << std::endl;
		os << "Select:\t\t\t" << select.read().to_uint() << std::endl;

		os << "Output content\n";
		os << "=============\n" << std::endl;

		for(auto& out : outputs)
			os << out.basename() << ":\t\t" << out.read().to_string(sc_dt::SC_HEX, true) << std::endl;
	}


	/*!
	 * \brief Demultiplex input data to selected output port
	 *
	 * \details
	 * The function calculates the modulo of the select port value.
	 * Thus, different numbers will address the same port. This is because
	 * the data cache types are specified in MMU, but not all demultiplexers
	 * have all outputs.
	 */
	void demultiplex()
	{
		//Get current selected value from select input.
		uint16_t t_select = static_cast<uint16_t>(select.read().to_uint()) % N;

		if(N <= t_select)
			SC_REPORT_WARNING("General_DeMux", "Selected output out of range. Selection for output left unchanged.");
		else
		{
			outputs.at(m_latestSelect.read().to_uint()).write(0);
			m_latestSelect = t_select;
			outputs.at(m_latestSelect.read().to_uint()).write(input.read());
		}

		return;
	}

	/*!
	 * \brief Update output value if input value changes
	 */
	void update()
	{
			outputs.at(m_latestSelect.read().to_uint()).write(input.read());
			return;
	}

	/*!
	 * \brief Destructor
	 */
	virtual ~General_DeMux() = default;

private:
	//Deleted Constructors
	General_DeMux() = delete;
	General_DeMux(const General_DeMux& src) = delete; 				//!< \brief Because sc_module could not be copied
	General_DeMux(General_DeMux&& src) = delete; 					//!< \brief Move semantic not implemented for sc_module
	General_DeMux& operator=(const General_DeMux& src) = delete; 	//!< \brief Because sc_module could not be copied
	General_DeMux& operator=(General_DeMux&& src) = delete; 		//!< \brief Move semantic not implemented for sc_module

#ifndef GSYSC
	sc_core::sc_signal<select_type_t> m_latestSelect{"Latest_Select"};
	//!< \brief Store latest select value to set latest output to zero for new selected value
#else
	sc_signal<select_type_t> m_latestSelect{"Latest_Select"};
	//!< \brief Store latest select value to set latest output to zero for new selected value
#endif

}; /* end module General_DeMux */

/*!
 * \brief Special version for boolean data type
 *
 * \details
 * With select input one can contribute an arbitrary signal to different targets.
 * The value type is a template parameter as well as the number of targets.
 * The bitwidth for select input is calculated automatically and depends on the number
 * of targets. Signal type for data input and all data outputs are identically.
 *
 * \tparam N Number of targets for data output signals
 * \tparam B Bitwidth for select signal
 */
template<uint16_t N, uint16_t B >
class General_DeMux<bool, N, B> : sc_core::sc_module
{
public:
	//public type definitions
	typedef bool value_type_t;
	//!< \brief Value type for demultiplexed data.
	typedef sc_dt::sc_lv<B> select_type_t;
	//!< \brief Type for select port of demultiplexer

	#ifndef GSYSC
		//Module ports
		sc_core::sc_vector<sc_core::sc_out<value_type_t>> outputs{"OutputPorts", N};
		//!< \brief These are the demultiplexed outputs of input data, selected by select-input.
		sc_core::sc_in<select_type_t> select{"select"};
		//!< \brief Select input to choose an output port for input data.
		sc_core::sc_in<value_type_t> input{"input"};
		//!< \brief Data input port which should be distributed to output consumers.
	#else
		//Module ports
	    sc_vector<sc_out<value_type_t>> outputs{"OutputPorts", N};
		//!< \brief These are the demultiplexed outputs of input data, selected by select-input.
		sc_in<select_type_t> select{"select"};
		//!< \brief Select input to choose an output port for input data.
		sc_in<value_type_t> input{"input"};
		//!< \brief Data input port which should be distributed to output consumers.
	#endif

	//Ctor
	SC_HAS_PROCESS(General_DeMux);
	/*!
	 * \brief General constructor for demultiplexer.
	 *
	 * \param[in] nameA Unique module name
	 */
	General_DeMux(const sc_core::sc_module_name& nameA):
		sc_core::sc_module(nameA)
	{
		SC_METHOD(demultiplex);
		sensitive << select.value_changed();

		SC_METHOD(update);
		sensitive << input.value_changed();

		return;
	}

	/*!
	 * \brief Initialize outputs of General_DeMux
	 */
	virtual void end_of_elaboration() override
	{
		for (auto& out : outputs)
			out.write(false);

		m_latestSelect.write(0);

		return;
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "General_DeMux";
	}

	/*!
	 * \brief Dump Demultiplexer information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << ": " << kind() << std::endl;
		os << "Data input:\t\t" << std::hex << input.read() << std::endl;
		os << "Select:\t\t\t" << select.read().to_uint() << std::endl;

		os << "Output content\n";
		os << "=============\n" << std::endl;

		for(auto& out : outputs)
			os << out.basename() << ":\t\t" << std::hex <<out.read() << std::endl;
	}


	/*!
	 * \brief Demultiplex input data to selected output port
	 *
	 * \details
	 * The function calculates the modulo of the select port value.
	 * Thus, different numbers will address the same port. This is because
	 * the data cache types are specified in MMU, but not all demultiplexers
	 * have all outputs.
	 */
	void demultiplex()
	{
		//Get current selected value from select input.
		uint16_t t_select = static_cast<uint16_t>(select.read().to_uint()) % N;

		if(N <= t_select)
			SC_REPORT_WARNING("General_DeMux", "Selected output out of range. Selection for output left unchanged.");
		else
			m_latestSelect.write(t_select);

		return;
	}

	/*!
	 * \brief Update output value if input value changes
	 */
	void update()
	{
			outputs.at(m_latestSelect.read().to_uint()).write(input.read());
			return;
	}

	/*!
	 * \brief Destructor
	 */
	virtual ~General_DeMux() = default;

private:
	//Deleted Constructors
	General_DeMux() = delete;
	General_DeMux(const General_DeMux& src) = delete; 				//!< \brief Because sc_module could not be copied
	General_DeMux(General_DeMux&& src) = delete; 					//!< \brief Move semantic not implemented for sc_module
	General_DeMux& operator=(const General_DeMux& src) = delete; 	//!< \brief Because sc_module could not be copied
	General_DeMux& operator=(General_DeMux&& src) = delete; 		//!< \brief Move semantic not implemented for sc_module

#ifndef GSYSC
	sc_core::sc_signal<select_type_t> m_latestSelect{"Latest_Select"};
	//!< \brief Store latest select value to set latest output to zero for new selected value
#else
	sc_signal<select_type_t> m_latestSelect{"Latest_Select"};
	//!< \brief Store latest select value to set latest output to zero for new selected value
#endif

}; /* end module General_DeMux boolean*/


} /* end namespace cgra */

#endif /* HEADER_GENERAL_DEMUX_H_ */
