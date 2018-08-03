#ifndef SYNCHRONIZER_H_
#define SYNCHRONIZER_H_

#include <systemc>
#include <cstdint>
#include <iostream>
#include <array>
#include "Typedef.h"

namespace cgra {

/*!
 * \class Synchronizer
 *
 * \brief Parameterizable Synchronizer for configurations
 *
 * \details
 * Synchronizer template module for configurations.
 *
 * \tparam T 	Valid signal type of PEs
 * \tparam N 	Number inputs
 */
template< typename T, uint32_t N>
	class Synchronizer : public sc_core::sc_module {
private:

public:
	typedef T valid_type_t;
	//!< \brief Type of input valid signal vector
	typedef cgra::ready_type_t ready_type_t;
	//!< \brief Type of output ready signal
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief Type of clock signal
	typedef sc_dt::sc_lv<N> config_type_t;
	//!< \brief Type of configuration input signal

	//Entity ports:
	std::array<sc_core::sc_in<T>, N> valid_inputs;
	//!< \brief Output port for corresponding selected input data
	sc_core::sc_in<config_type_t> conf{"Configuration"};
	//!< \brief configuration input
	sc_core::sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock input
	sc_core::sc_out<ready_type_t> ready{"ready"};
	//!< \brief Synchronization output to indicate finished data processing

	SC_HAS_PROCESS(Synchronizer);
	/*!
	 * \brief General Constructor
	 *
	 * \param nameA 	Name of the Synchronizer as a SystemC Module
	 */
	Synchronizer(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA)
	{
		SC_METHOD(sync);
		sensitive << clk.pos();
	}

	/*!
	 * \brief Initialize outputs of Synchronizer
	 */
	virtual void end_of_elaboration() override
	{
		ready.write(0);
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "Synchronizer";
	}

	/*!
	 * \brief Defaulted Destructor
	 */
	virtual ~Synchronizer() = default;

	/*!
	 * \brief Multiplexing data inputs to one output
	 *
	 * \details
	 *
	 */
	void sync()
	{
		//Write current input signals to logic vector
		for(uint32_t i = 0; N > i; ++i)
			m_status_word[i] = valid_inputs.at(i).read();

		//Update current status of inputs with configuration
		//Unused valid signals are set to '1' by synchronization configuration
		m_status_word |= conf.read();

		//And all bits in current status word. If positive, ready signal raises high.
		ready.write(m_status_word.and_reduce());
	}

private:
	//Forbidden Constructors
	Synchronizer() = delete;
	Synchronizer(const Synchronizer& src) = delete;					//!< \brief because sc_module could not be copied
	Synchronizer& operator=(const Synchronizer& src) = delete;		//!< \brief because move not implemented for sc_module
	Synchronizer(Synchronizer&& src) = delete;						//!< \brief because sc_module could not be copied
	Synchronizer& operator=(Synchronizer&& src) = delete;			//!< \brief because move not implemented for sc_module

	//Internal properties
	config_type_t m_status_word{0};
	//!< \brief Helper variable which temporary saves current status of valid signals
};


} //end namespace cgra


#endif // SYNCHRONIZER_H_
