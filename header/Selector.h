#ifndef SELECTOR_H_
#define SELECTOR_H_

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
 * \brief Calculate size for part vector
 *
 * \param [in] length		Selection range of input bit vector
 * \param [in] numOfParts 	Number of parts from selection
 * \param [in] partWidth 	A parts bit width
 *
 * \return Necessary size for selected bitstream length
 */
static constexpr uint32_t array_size(uint32_t length, uint32_t numOfParts, uint32_t partWidth)
{
	return (static_cast<int32_t>(length - numOfParts * partWidth) < 0) ?
			((length % numOfParts) ? (length / numOfParts) - 1 : length / numOfParts) : numOfParts ;
}

/*!
 * \class Selector
 *
 * \brief Parameterizable Selector for configurations
 *
 * \details
 * Selector template module for configurations.
 * A Selector can be used to get ranges of a bit vector.
 * From the start bit to the end bit, the range is separated
 * into parts of the same bit width. If the number of parts
 * multiplied by the defined bit width for each part is
 * larger then the bitwidth of the input bit vector, the
 * Selector creates a vector with all full available parts
 * - all parts will have the full configured bit width.
 *
 * \tparam T 	Input logic vector type
 * 				(needs to have part selection (range))
 * \tparam N 	Number of output parts
 * \tparam M 	Bitwidth of each part
 * \tparam L 	Length of selected bit range
 */
template< typename T, uint32_t N, uint32_t M, uint32_t L>
	class Selector : public sc_core::sc_module {
private:

public:
	typedef T config_type_t;
	//!< \brief Type of input configuration vector
	typedef sc_dt::sc_lv<M> configpart_type_t;
	//!< \brief Select type for Selector input signals

#ifndef GSYSC
	//Entity ports:
	sc_core::sc_in<config_type_t> config_input{"full_config_vector"};
	//!< \brief Output port for corresponding selected input data
	std::array<sc_core::sc_out<configpart_type_t>, cgra::array_size(L, N, M)> config_parts;
	//!< \brief std::array of configuration parts
#else
	//Entity ports:
	sc_in<config_type_t> config_input{"full_config_vector"};
	//!< \brief Output port for corresponding selected input data
	std::array<sc_out<configpart_type_t>, cgra::array_size(L, N, M)> config_parts;
	//!< \brief std::array of configuration parts
#endif

	SC_HAS_PROCESS(Selector);
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA 	Name of the Selector as a SystemC Module
	 * \param[in] start 	Start position on selection range (index starts with zero)
	 * \param[in] length	Length of input configuration bitstream
	 */
	Selector(const sc_core::sc_module_name& nameA, uint32_t start, uint32_t length)
	: sc_core::sc_module(nameA)
	{
		sc_assert(start >= 0 && length >= start);

		m_start = start;
		m_length = array_size(L, N, M) * M;
		m_config_length = length;

		SC_METHOD(demultiplex);
		sensitive << config_input;
	}

	/*!
	 * \brief Initialize outputs of Selector
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
		return "Selector";
	}

	/*!
	 * \brief Dump Selector information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << "\t\t" << kind() << std::endl;
		os << "Configuration input:\t\t" << std::setw(3) << config_input.read().to_string(sc_dt::SC_HEX,true) << std::endl;
		os << "Number of output parts:\t\t" << std::setw(3) << static_cast<int>(N) << std::endl;

		os << "Parts content\n";
		os << "=============\n" << std::endl;

		uint32_t i = 0;
		for(auto& part : config_parts)
			os << "part_" << ++i << ":\t\t" << part.read().to_string(sc_dt::SC_HEX,true) << std::endl;

		os << std::endl;
	}

	/*!
	 * \brief Defaulted Destructor
	 */
	virtual ~Selector() = default;

	/*!
	 * \brief Demultiplexing data inputs to multiple output
	 *
	 * \details
	 * Parts from configuration selection are linked to its
	 * corresponding outputs.
	 */
	void demultiplex()
	{
		for (uint32_t i = 0; m_length / M > i; ++i)
			config_parts.at(i).write(config_input.read()((m_config_length - m_start - 1) - i * M, (m_config_length - m_start) - (i + 1) * M));
		return;
	}

private:
	//Forbidden Constructors
	Selector() = delete;
	Selector(const Selector& src) = delete;					//!< \brief because sc_module could not be copied
	Selector& operator=(const Selector& src) = delete;		//!< \brief because move not implemented for sc_module
	Selector(Selector&& src) = delete;						//!< \brief because sc_module could not be copied
	Selector& operator=(Selector&& src) = delete;			//!< \brief because move not implemented for sc_module

	uint32_t m_start{0};
	//!< \brief Start bit index in input bit vector (indexing starts with zero)
	uint32_t m_length{0};
	//!< \brief Length of selected bits from start bit
	uint32_t m_config_length{0};
	//!< \brief Length of input configuration bitstream

};


} //end namespace cgra


#endif // SELECTOR_H_
