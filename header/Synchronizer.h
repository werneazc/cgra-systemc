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

#ifndef GSYSC
	//Entity ports:
	std::array<sc_core::sc_in<T>, N> valid_inputs;
	//!< \brief Output port for corresponding selected input data
	sc_core::sc_in<config_type_t> conf{"Configuration"};
	//!< \brief configuration input
	sc_core::sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock input
	sc_core::sc_out<ready_type_t> ready{"ready"};
	//!< \brief Synchronization output to indicate finished data processing
#else
	//Entity ports:
	std::array<sc_in<T>, N> valid_inputs;
	//!< \brief Output port for corresponding selected input data
	sc_in<config_type_t> conf{"Configuration"};
	//!< \brief configuration input
	sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock input
	sc_out<ready_type_t> ready{"ready"};
	//!< \brief Synchronization output to indicate finished data processing
#endif

	SC_HAS_PROCESS(Synchronizer);
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA 	Name of the Synchronizer as a SystemC Module
	 */
	Synchronizer(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA)
#ifdef MCPAT
	,m_totalCycles{0U}, m_idleCycles{0U}, m_busyCycles{0U}
#endif
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

#ifdef MCPAT
	/**
	 * \brief Dump runtime statistics for McPAT simulation
	 *
	 * \param os Define used outstream [default: std::cout]
	 */
	void dumpMcpatStatistics(std::ostream& os = ::std::cout) const
	{
		os << name() << "\t\t" << kind() << "\n";
		os << "total cycles: " << m_totalCycles << "\n";
		os << "idle cycles: " << m_idleCycles << "\n";
		os << "busy cycles: " << m_busyCycles << "\n";
		os << std::endl;
	}
#endif

	/*!
	 * \brief Multiplexing data inputs to one output
	 *
	 * \details
	 * First, the current status word from valid input signals
	 * from previous ProcessingElement level is ORed with the
	 * configuration mask. The configuration mask consists of
	 * Ones where no valid signal from a previous PE will appear.
	 * The other mask bits are zero.
	 *
	 * Afterwards, all bits of the vector are sequentially ANDed
	 * and the result is the status of the ready signal.
	 */
	void sync()
	{
#ifdef MCPAT
		/* A synchronizer always updates its input and output buffer states.
		 * Thus the component is always busy and has no idle state.
		 */
		++m_totalCycles;
		++m_busyCycles;
#endif
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
#ifdef MCPAT
	//McPAT dynamic statistic counters:
	uint32_t m_totalCycles;   //!< \brief Count total number of executed cycles
	uint32_t m_idleCycles;    //!< \brief Count number of idle cycles
	uint32_t m_busyCycles;    //!< \brief Count number of working cycles
#endif
};


} //end namespace cgra


#endif // SYNCHRONIZER_H_
