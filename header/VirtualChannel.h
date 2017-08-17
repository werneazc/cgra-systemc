#ifndef VIRTUALCHANNEL_H_
#define VIRTUALCHANNEL_H_

#include <systemc>
#include <cstdint>
#include <array>
#include "Multiplexer.h"

namespace cgra {

/*!
 * \class VirtualChannel
 *
 * \brief Templated VirtualChannel to connect levels of processing elements
 *
 * \details
 * A VCGRA consists of alternating levels of processing elements (PE) and
 * virtual channels (VC). VCs are used to reroute data from predecessing
 * PEs to its succeeding PEs depending on the configuration bitstream conf.
 * A VC buffers his inputs and outputs during one clock cycle. It consists
 * of a number of Multiplexers  which are parameterized by the conf-input
 * of a VC. The number of bits to parameterize a Multiplexer within the
 * VC depends on the number of inputs of a VC. The necessary bitwith for
 * a select-input of a Multiplexer must be fixed during design-time and set
 * as a Template-parameter.
 * The input- and output-data-bitwidth can differ. However, the internal bitwidth
 * needs to be fixed to carry the biggest of them.
 *
 * \tparam R Number preceding PEs of a VC
 * \tparam S Bitwidth of the inputs of a VC
 * \tparam T Number of VC outputs = 2 x Number of succeeding PEs of a VC
 * \tparam U Bitwidth of the outputs of a VC
 * \tparam L Bitwidth of select lines of internal Multiplexers
 * \tparam N Internal Bitwidth of a VC (default = 8); Needs to be >= max(S,U)
 */
template <uint32_t R, uint32_t S, uint32_t T, uint32_t U, uint32_t L, uint32_t N = U>
class VirtualChannel : public sc_core::sc_module {
private:
	typedef sc_dt::sc_int<N> internal_type_t;
	typedef Multiplexer<R, L, N> mux_type_t;

public:
	typedef sc_dt::sc_int<S> input_type_t;
	typedef bool clock_type_t;
	typedef bool reset_type_t;
	typedef sc_dt::sc_lv<T * L> conf_type_t;
	typedef typename mux_type_t::valid_type_t valid_type_t;
	typedef typename mux_type_t::valid_type_t enables_type_t;
	typedef typename mux_type_t::select_type_t select_type_t;
	typedef sc_dt::sc_int<U> output_type_t;

	//Entity ports
	sc_core::sc_in<clock_type_t> clk{"clk"};
	//!< \brief Clock port of a VC
	sc_core::sc_in<reset_type_t> rst{"rst"};
	//!< \brief Reset VC buffers if rst equals false
	std::array<sc_core::sc_in<valid_type_t>, R > valids;
	//!< \brief Input port for all valid signals of the preceding PEs
	sc_core::sc_in<conf_type_t> conf{"conf"};
	//!< \brief Configuration bitstream for internal Multiplexers
	std::array<sc_core::sc_in<input_type_t>, R> channel_inputs;
	//!< \brief Data inputs of all preceding PEs
	std::array<sc_core::sc_out<output_type_t>, T> channel_outputs;
	//!< \brief Data outputs for all succeding PEs
	std::array<sc_core::sc_out<enables_type_t>, T> enables;
	//!< \brief Corresponding Enable signals for succeeding PEs

private:
	//Internal Signals
	std::array<sc_core::sc_buffer<valid_type_t>, R> m_validBuffer;
	//!< \brief Internal buffer for incoming valid-signals
	std::array<sc_core::sc_buffer<enables_type_t>, T> m_enablesBuffer;
	//!< \brief Internal buffer for outgoing enable-signals
	std::array<sc_core::sc_buffer<internal_type_t>, R> m_inputBuffers;
	//!< \brief Internal buffers for data-inputs of a VC
	std::array<sc_core::sc_buffer<internal_type_t>, T> m_outputBuffers;
	//!< \brief Internal buffers for data-outputs of a VC
	sc_core::sc_vector<mux_type_t> m_multiplexers{"Multiplexers", T};
	//!< \brief Array of internal Multiplexers (# depends on # of outputs)
	std::array<sc_core::sc_signal<select_type_t>, T> m_selectLines;
	//!< \brief Select signals for every Multiplexer

public:
	SC_HAS_PROCESS(VirtualChannel);
	/*!
	 * \brief Named Ctor for a VC
	 *
	 * \param nameA Nume of the VC within the simulation
	 */
	VirtualChannel(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA)
	{
		//Register SystemC methods at simulator
		SC_METHOD(buffer_input);
		sensitive << clk.pos();
		SC_METHOD(buffer_output);
		sensitive << clk.pos();
		SC_METHOD(split_select);
		sensitive << conf.value_changed();


		//Build virtual channel from buffers and multiplexers
		//----------------------------------------------------

//		// channel input to  input-buffer connection
//		for(uint32_t i = 0; i < R; ++i)
//		{
//			channel_inputs[i].bind(m_inputBuffers[i]);
//			valids[i].bind(m_validBuffer[i]);
//		}

		// buffer to multiplexer connection
		for(uint32_t i = 0; i < T; ++i)
		{
			for(uint32_t j = 0; j < R; j++)
			{
				m_multiplexers[i].data_inputs[j].bind(m_inputBuffers[j]);
				m_multiplexers[i].valid_inputs[j].bind(m_validBuffer[j]);
			}
				m_multiplexers[i].select.bind(m_selectLines[i]);
		}

		//multiplexer to output-buffer connection
		for(uint32_t i = 0; i < T; ++i)
		{
			m_multiplexers[i].sel_data.bind(m_outputBuffers[i]);
			m_multiplexers[i].sel_valid.bind(m_enablesBuffer[i]);
		}
	}

	/*!
	 * \brief Callback of sc_core::sc_module class to initialize outputs of VC
	 */
	virtual void end_of_elaboration() override
	{
		for (uint32_t i = 0; i < T; ++i)
		{
			channel_outputs[i].write(0);;
			enables[i].write(0);
		}
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "Virtual Channel";
	}

	//Processes
	/*!
	 * \brief Buffer VC inputs in internal buffers for one clock cycle
	 */
	void buffer_input()
	{
		if(!rst.read())
		{
			for(uint32_t i = 0; i < R; ++i)
			{
				m_inputBuffers[i].write(0);
				m_validBuffer[i].write(false);
			}
		}
		else
		{
			for(uint32_t i = 0; i < R; ++i)
			{
				m_inputBuffers[i].write(channel_inputs[i].read());
				m_validBuffer[i].write(valids[i].read());
			}
		}
	}

	/*!
	 * \brief Buffer VC outputs in internal buffers for one clock cycle
	 */
	void buffer_output()
	{
		for(uint32_t i = 0; i < T; ++i)
		{
			channel_outputs[i].write(m_outputBuffers[i].read().range(U - 1, 0));
			enables[i].write(m_enablesBuffer[i].read());
		}
	}

	/*!
	 * \brief Split the configuration bitstream into parts to parameterize a Multiplexer
	 *
	 * \details
	 * This function splits the configuration stream of a VC into parts for the
	 * included Multiplexers. The length of the bitstream depends on the number of
	 * outputs of a VC and the bitwidth of the select inputs of the Multiplexers.
	 * The bitorder within a configuration bitstream is explained on an example:
	 * Imagine you have n + 1 data outputs. They are numbered from 0 to n internally
	 * from left to right within the array:
	 *
	 * | data_mux_0 | data_mux_1 | data_mux_n-1 | data_mux_n |
	 *
	 * The config-bit oder within the config.-bitstream is reverse.
	 *
	 * | data_mux_n | data_mux_n-1| ... | data_mux_1 | data_mux_0 |
	 *
	 * However, the select-bits for a multiplexer have little-endian
	 * byte order. For instance a configuration with 2bit select-inputs
	 * and eight outputs will have a 16bits configuration-bitstream as follows:
	 *
	 * |77|66|55|44|33|22|11|00|
	 *
	 * Imagine you want the following configuration:
	 *
	 *  - input_0 -> output_0: 00
	 *	- input_0 -> output_1: 00
	 *	- input_1 -> output_2: 01
	 *	- input_1 -> output_3: 01
	 *	- input_2 -> output_4: 10
	 *	- input_2 -> output_5: 10
	 *	- input_3 -> output_6: 11
	 *	- input_3 -> output_7: 11
	 *
	 *	|11|11|10|10|01|01|00|00|
	 *
	 */
	void split_select()
	{
		for(uint32_t i = 0; i < T; ++i){
			m_selectLines[i].write(conf.read().range((i + 1) * L - 1, i * L));
//			std::cout << conf.read().range((i + 1) * L - 1, i * L) << std::endl;
		}
	}

private:
	//Forbidden Constructors
	VirtualChannel() = delete;
	VirtualChannel(const VirtualChannel& src) = delete;
	VirtualChannel& operator=(const VirtualChannel& src) = delete;
	VirtualChannel(VirtualChannel&& src) = delete;
	VirtualChannel& operator=(VirtualChannel&& src) = delete;

};


} // namespace cgra

#endif // VIRTUALCHANNEL_H_
;
