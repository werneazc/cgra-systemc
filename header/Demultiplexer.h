#ifndef DEMULTIPLEXER_H_
#define DEMULTIPLEXER_H_

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
 * \class Demultiplexer
 *
 * \brief Parameterizable Demultiplexer for configurations
 *
 * \details
 * Demultiplexer template module for configurations.
 * A input string is separated into parts of same bitwidth.
 * The input logic vector needs to have part selection
 * functionality. The number of parts and the configured bitwidth
 * for each part needs to be lower or equal to the input vector
 * bitwidth.
 *
 * \tparam T 	Input logic vector type
 * 				(needs to have part selection (range))
 * \tparam N 	Number of output parts
 * \tparam L 	Bitwidth of full configuration vector
 */
template< typename T, uint32_t N, uint32_t L>
class Demultiplexer : public sc_core::sc_module {

public:
	typedef T config_type_t;
	//!< \brief Type of input configuration vector
	typedef sc_dt::sc_lv<L / N> configpart_type_t;
	//!< \brief Select type for Demultiplexer input signals

	//Entity ports:
	sc_core::sc_in<config_type_t> config_input{"full_config_vector"};
	//!< \brief Output port for corresponding selected input data
	std::array<sc_core::sc_out<configpart_type_t>, N> config_parts;
	//!< \brief std::array of configuration parts

	SC_HAS_PROCESS(Demultiplexer);
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA Name of the Demultiplexer as a SystemC Module
	 */
	Demultiplexer(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA)
	{
		sc_assert(L % N == 0);

		SC_METHOD(demultiplex);
		sensitive << config_input;

		#ifdef GSYSC
        {
            RENAME_PORT(config_input, (create_name<std::string, uint32_t>("p_conf_in",0)));
        }
		#endif
	}

	/*!
	 * \brief Initialize outputs of Demultiplexer
	 */
	virtual void end_of_elaboration() override
	{
		for (auto& out : config_parts)
			out.write(0);
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "Demultiplexer";
	}

	/*!
	 * \brief Dump Demultiplexer information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << "\t\t" << kind() << std::endl;
		os << "Configuration input:\t\t" << std::setw(3) << config_input.read().to_string() << std::endl;
		os << "Number of output parts:\t\t" << std::setw(3) << static_cast<int>(N) << std::endl;

		os << "Parts content\n";
		os << "=============\n" << std::endl;

		uint32_t i = 0;
		for(auto& part : config_parts)
			os << "part_" << ++i << ":\t\t" << part.read().to_string() << std::endl;
	}


	/*!
	 * \brief Defaulted Destructor
	 */
	virtual ~Demultiplexer() = default;

	/*!
	 * \brief Demultiplexing data inputs to multiple output
	 *
	 * \details
	 * Configuration bit vector is sliced into parts.
	 */
	void demultiplex()
	{
		for (uint32_t i = 0; N > i; ++i)
			config_parts.at(i).write(config_input.read()((L - 1) - i * (L / N), L - (i + 1) * (L / N)));

		return;
	}

private:
	//Forbidden Constructors
	Demultiplexer() = delete;
	Demultiplexer(const Demultiplexer& src) = delete;				//!< \brief because sc_module could not be copied
	Demultiplexer& operator=(const Demultiplexer& src) = delete;	//!< \brief because move not implemented for sc_module
	Demultiplexer(Demultiplexer&& src) = delete;					//!< \brief because sc_module could not be copied
	Demultiplexer& operator=(Demultiplexer&& src) = delete;			//!< \brief because move not implemented for sc_module
};


} //end namespace cgra


#endif // DEMULTIPLEXER_H_
