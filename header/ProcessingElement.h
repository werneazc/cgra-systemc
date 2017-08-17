/*
 * ProcessingElement.h
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#ifndef HEADER_PROCESSINGELEMENT_H_
#define HEADER_PROCESSINGELEMENT_H_

#include <systemc>
#include <cstdint>
#include <array>
#include <string>

namespace cgra
{

/************************************************************************/
// Processing Element
//!
//! \class Processing_Element
//! \brief Processing element within a CGRA
//!
//! \details
//! The vertex waits for the notification of his two incoming values and
//! process them.
//!
//! \tparam N Bitwidth of incoming data
//! \tparam O Bitwidth of outgoing data
/************************************************************************/
template <uint16_t N = 8, uint16_t K = 8, uint16_t L = 4>
class Processing_Element : public sc_core::sc_module
{
  public:
	typedef sc_dt::sc_int<N> input_type_t;
	typedef sc_dt::sc_lv<L> config_type_t;
	typedef sc_dt::sc_int<K> output_type_t;
	typedef sc_dt::sc_lv<2> enable_type_t;
	typedef bool clock_type_t;
	typedef bool valid_type_t;

	//! \enum OP
	//! \brief Operations of a processing element
	enum OP
	{
		NONE = 0,
		ADD,
		SUB,
		MUL,
		IDIV,
		MDL,
		GRE,
		EQU,
		BUF
	};

	//interfaces
	sc_core::sc_in<input_type_t> in1{"In1"};  		//!< \brief input one
	sc_core::sc_in<input_type_t> in2{"In2"};   	//!< \brief input two
	sc_core::sc_in<config_type_t> conf{"conf"};   	//!< \brief configuration of PE (operation)
	sc_core::sc_in<clock_type_t> clk{"clk"};		//!< \brief clock
	sc_core::sc_in<enable_type_t> enable{"enable"}; 	//!< \brief synchronization of inputs

	sc_core::sc_out<output_type_t> res{"res"}; 	//!< \brief operation result
	sc_core::sc_out<valid_type_t> valid{"valid"};	//!< \brief synchronization of output

	//Constructor
	SC_HAS_PROCESS(Processing_Element);
	Processing_Element(const sc_core::sc_module_name& nameA, const uint32_t pe_numberA) : sc_module(nameA), m_peId{pe_numberA}
	{
		SC_METHOD(perform);
		sensitive << clk;
	}


	//callback
	/*!
	 * \brief SystemC Module callback after port bindings to initialize outputs
	 */
	virtual void end_of_elaboration() override
	{
		valid.write(false);

		m_invalues[0].write(0);
		m_invalues[1].write(0);
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override {
		return "Processing Element";
	}

	//process
	/*!
	 * \brief PE process method
	 *
	 * \details
	 * on every positive clock cycle depending on the internal state
	 * an operation is processed, outputs are set to valid or the PE
	 * waits for new values.
	 */
	void perform()
	{
		if (clk.posedge())
		{
			//save current signals in internal buffers
			this->m_invalues[0].write(in1.read());
			this->m_invalues[1].write(in2.read());

			//state machine of a processing element
			switch (this->m_current_state)
			{
			case STATE::AWAIT_DATA:

//				std::cout << "@ " << sc_core::sc_time_stamp() << " STATE=AWAIT_DATA" << std::endl;

				this->valid.write(false);

				if (this->enable.read() == "11")
					this->m_current_state = STATE::PROCESS_DATA;
				else
					this->m_current_state = STATE::AWAIT_DATA;
				break;

case STATE::PROCESS_DATA:

//				std::cout << "@ " << sc_core::sc_time_stamp() << " STATE=PROCESS_DATA" << std::endl;

				m_current_state = STATE::VALID_DATA;

				switch (this->conf.read().to_uint())
				{
				case OP::ADD: //ADD
					addition();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=ADD" << std::endl;
					break;
				case OP::SUB: //SUBTRACT
					subtraction();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=SUB" << std::endl;
					break;
				case OP::MUL: //MULTIPLY
					multiplication();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=MUL" << std::endl;
					break;
				case OP::IDIV: //DIVIDE
					idivide();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=IDIV" << std::endl;
					break;
				case OP::MDL: //MODULO
					modulo();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=MDL" << std::endl;
					break;
				case OP::GRE: //GREATER
					greater();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=GRE" << std::endl;
					break;
				case OP::EQU: //EQUAL
					equal();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=EQU" << std::endl;
					break;
				case OP::BUF: //BUFFER
					buffer();
//					std::cout << "@ " << sc_core::sc_time_stamp() << " OP=BUF" << std::endl;
					break;
				default:
//					std::cout << "@ " << sc_core::sc_time_stamp() << "default stage" << std::endl;
					this->m_current_state = STATE::AWAIT_DATA;
				}
				break;

			case STATE::VALID_DATA:
//				std::cout << "@ " << sc_core::sc_time_stamp() << " STATE=VALID_DATA" << std::endl;
				this->m_current_state = STATE::AWAIT_DATA;
				this->valid.write(true);
				break;

			default:
				this->m_current_state = STATE::AWAIT_DATA;
			};
		}
	}

	//Destructor
	virtual ~Processing_Element() = default;

  private:
	//internal signals
	//! \enum STATE
	//! \brief States of the processing element's state machine
	enum STATE
	{
		AWAIT_DATA,
		PROCESS_DATA,
		VALID_DATA
	};

	//private member:
	std::array<sc_core::sc_buffer<input_type_t>, 2> m_invalues; 	//!< \brief Input buffers
	const uint32_t m_peId;										//!< \brief A processing element's unique ID
	STATE m_current_state{STATE::AWAIT_DATA};						//!< \brief The internal state of the processin element

	//forbidden constructor
	Processing_Element() = delete;
	Processing_Element(const Processing_Element &source) = delete;			  //!< \brief because sc_module could not be copied
	Processing_Element(Processing_Element &&source) = delete;				  //!< \brief because move not implemented for sc_module
	Processing_Element &operator=(const Processing_Element &source) = delete; //!< \brief because sc_module could not be copied
	Processing_Element &operator=(Processing_Element &&source) = delete;	  //!< \brief because move not implemented for sc_module

	//static members
	static uint32_t pe_generation_counter;

	//a processing element's operations
	/*!
	 * \brief Add the two inputs
	 */
	inline void addition()
	{
		res.write(m_invalues[0].read() + m_invalues[1].read());
	}
	/*!
	 * \brief Subtracts in2 from in1
	 */
	inline void subtraction()
	{
		res.write(m_invalues[0].read() - m_invalues[1].read());
	}
	/*!
	 * \brief Multiplies the two inputs
	 */
	inline void multiplication()
	{
		sc_assert(m_invalues[0].read() <= 32 || m_invalues[1].read().length() <= 32);
		res.write(m_invalues[0].read() * m_invalues[1].read());
	}
	/*!
	 * \brief Performs an integer devision in1 / in2
	 */
	inline void idivide()
	{
		sc_dt::int64 temp_res = 0;
		temp_res = static_cast<sc_dt::int64>(m_invalues[0].read().to_int() / m_invalues[1].read().to_int());
		res.write(temp_res);
	}
	/*!
	 * \brief Perform modulo operation in1 & in2
	 */
	inline void modulo()
	{
		res.write(m_invalues[0].read() % m_invalues[1].read());

	}
	/*!
	 * \brief Returns the greater input of in1 and in2
	 */
	inline void greater()
	{
		res.write((m_invalues[0].read() >= m_invalues[1].read()) ? m_invalues[0].read() : m_invalues[1].read());
	}
	/*!
	 * \brief Returns 1 if both are value-equal or 0 otherwise
	 */
	inline void equal()
	{
		res.write((m_invalues[0].read() == m_invalues[1].read()) ? 1 : 0);
	}
	/*!
	 * \brief Copies in1 to res
	 */
	inline void buffer()
	{
		res.write(m_invalues[0].read());
	}
};

} /* namespace cgra */

#endif /* HEADER_PROCESSINGELEMENT_H_ */
