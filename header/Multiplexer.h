#ifndef MULTIPLEXER_H_
#define MULTIPLEXER_H_

#include <systemc>
#include <cstdint>
#include <iostream>
#include <array>

#ifdef GSYSC
#include <gsysc.h>
#include "utils.hpp"
#endif

namespace cgra {

/*!
 * \class Multiplexer
 *
 * \brief Parameterizable Multiplexer for virtual Channels
 *
 * \details
 * Multiplexer template module for virtual channel.
 * The default bitwidth is 8 bits. During coding
 * the number of inputs and the necessary bitwidth of
 * the select input has to be set. In general, L depends
 * on K and is ceil(log2(K)).
 *
 * \tparam K 	Number of multiplexer inputs (data and
 * 				corresponding valids
 * \tparam L 	Bitwidth of select input (see details)
 * \tparam N 	Bitwidth of multiplexer data-path (default 8)
 */
template< uint32_t K, uint32_t L, uint32_t N = 8>
class Multiplexer : public sc_core::sc_module {

public:
	typedef sc_dt::sc_int<N> data_type_t;
	//!< \brief Type of multiplexed data
	typedef bool valid_type_t;
	//!< \brief Valid signal type
	typedef sc_dt::sc_lv<L> select_type_t;
	//!< \brief Select type for Multiplexer input signals

#ifndef GSYSC
	//Entity ports:
	std::array<sc_core::sc_in<data_type_t>, K> data_inputs;
	//!< \brief std::array of input ports for incoming mux data
	std::array<sc_core::sc_in<valid_type_t>, K> valid_inputs;
	//!< \brief std::array of input ports for incoming valid signals
	sc_core::sc_in<select_type_t> select{"select"};
	//!< \brief Select line to switch between data and valid inports
	sc_core::sc_out<valid_type_t> sel_valid{"selected_valid"};
	//!< \brief Output port for selected input data
	sc_core::sc_out<data_type_t> sel_data{"selected_data"};
	//!< \brief Output port for corresponding selected input data
#else
	//Entity ports:
	std::array<sc_in<data_type_t>, K> data_inputs;
	//!< \brief std::array of input ports for incoming mux data
	std::array<sc_in<valid_type_t>, K> valid_inputs;
	//!< \brief std::array of input ports for incoming valid signals
	sc_in<select_type_t> select{"select"};
	//!< \brief Select line to switch between data and valid inports
	sc_out<valid_type_t> sel_valid{"selected_valid"};
	//!< \brief Output port for selected input data
	sc_out<data_type_t> sel_data{"selected_data"};
	//!< \brief Output port for corresponding selected input data
#endif

	SC_HAS_PROCESS(Multiplexer);
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA Name of the Multiplexer as a SystemC Module
	 */
	Multiplexer(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA)
	{
		SC_METHOD(multiplex);
		sensitive << select;

		for (uint32_t i = 0; i < K; ++i)
			sensitive << valid_inputs[i] << data_inputs[i];
		#ifdef GSYSC
        {
        	size_t i=0;
        	for(auto &in : data_inputs){
        	    RENAME_PORT(&in, (cgra::create_name<std::string, uint32_t>("p_input",i++)));
        	}
        	i=0;
        	for(auto &in : valid_inputs){
        	    RENAME_PORT(&in, (cgra::create_name<std::string, uint32_t>("p_valid_in",i++)));
        	}
        }
		#endif
	}

	/*!
	 * \brief Initialize outputs of Multiplexer
	 */
	virtual void end_of_elaboration() override
	{
		sel_data.write(0);
		sel_valid.write(false);
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "Multiplexer";
	}

	/*!
	 * \brief Dump Multiplexer information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << "\t\t" << kind() << std::endl;
		os << "Selected port:\t\t" << select.read().to_string() << std::endl;
		os << "Selected data:\t\t" << sel_data.read().to_string() << std::endl;
		os << "Selected valid:\t\t" << sel_valid.read() << std::endl;

		os << "Data & valid inputs\n";
		os << "===================" << std::endl;

		uint32_t i = 0;
		for(auto& in : data_inputs)
			os << "input_" << ++i << ":\t\t" << in.read().to_string() << std::endl;
		i = 0;
		for(auto& val : valid_inputs)
			os << "valid_" << ++i << ":\t\t" << val.read() << std::endl;
	}


	/*!
	 * \brief Defaulted Destructor
	 */
	virtual ~Multiplexer() = default;

	/*!
	 * \brief Multiplexing data inputs to one output
	 *
	 * \details
	 * A datum from a Multiplexers data inputs and its
	 * corresponding valid signal are linked to the
	 * Multiplexer's output depending on the select line.
	 */
	void multiplex()
	{
		uint32_t tmp_select = select.read().to_uint();

		// Check, that selecting index is valid
		sc_assert(tmp_select < K);

		sel_data.write(data_inputs[tmp_select].read());
		sel_valid.write(valid_inputs[tmp_select].read());
	}

private:
	//Forbidden Constructors
	Multiplexer() = delete;
	Multiplexer(const Multiplexer& src) = delete;
	Multiplexer& operator=(const Multiplexer& src) = delete;
	Multiplexer(Multiplexer&& src) = delete;
	Multiplexer& operator=(Multiplexer&& src) = delete;
};


} //end namespace cgra


#endif // MULTIPLEXER_H_
