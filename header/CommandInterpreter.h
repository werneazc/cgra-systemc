#ifndef COMMANDINTERPRETER_H_
#define COMMANDINTERPRETER_H_

#include <cstdint>
#include <systemc>
#include <iostream>

namespace cgra {

/*!
 * \class CommandInterpreter
 *
 * \brief Separate Assembler Command parts
 *
 * \details
 * A Assembler Command consists of 4 parts
 * which are separated for further processing.
 *
 */
class CommandInterpreter : public sc_core::sc_module
{
public:
	typedef sc_dt::sc_lv<32> assembler_type_t;
	//!< \brief Assembler Command type to control ManagementUnit
	typedef sc_dt::sc_lv<6> command_type_t;
	//!< \brief Command type to control ManagementUnit
	typedef sc_dt::sc_lv<7> place_type_t;
	//!< \brief Type to select place in cache line in data cache
	typedef sc_dt::sc_lv<3> line_type_t;
	//!< \brief Type to select cache line in data cache
	typedef sc_dt::sc_lv<16> address_type_t;
	//!< \brief Type to select address in shared memory

#ifndef GSYSC
	//Entity ports
	sc_core::sc_in<assembler_type_t> assembler{"Assembler_Command_In"};
	//!< \brief Input port for Assembler Command
	sc_core::sc_out<address_type_t> address{"Address_Out"};
	//!< \brief Output for shared memory address
	sc_core::sc_out<line_type_t> line{":Line_Out"};
	//!< \brief Output for cache line
	sc_core::sc_out<place_type_t> place{"Place_Out"};
	//!< \brief Output for place in cache line
	sc_core::sc_out<command_type_t> command{"Command_Out"};
	//!< \brief Output for command to process
#else
	//Entity ports
	gsys_in<assembler_type_t> assembler{"Assembler_Command_In"};
	//!< \brief Input port for Assembler Command
	gsys_out<address_type_t> address{"Address_Out"};
	//!< \brief Output for shared memory address
	gsys_out<line_type_t> line{":Line_Out"};
	//!< \brief Output for cache line
	gsys_out<place_type_t> place{"Place_Out"};
	//!< \brief Output for place in cache line
	gsys_out<command_type_t> command{"Command_Out"};
	//!< \brief Output for command to process
#endif

	//Constructor
	SC_HAS_PROCESS(CommandInterpreter);
	/*!
	 * \brief General Constructor
	 *
	 * \param[in] nameA Name of the CommandInterpreter as a SystemC Module
	 */
	CommandInterpreter(const sc_core::sc_module_name& nameA);

	/*!
	 * \brief Initialize outputs of CommandInterpreter
	 */
	virtual void end_of_elaboration() override
	{
		command.write("0d0");
		line.write("0d0");
		place.write("0d0");
		address.write("0d0");
	}

	/*!
	 * \brief Print kind of SystemC-module
	 */
	virtual const char* kind() const override
	{
		return "CommandInterpeter";
	}

	/*!
	 * \brief Dump CommmandInterpreter information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override
	{
		os << name() << "\t\t" << kind() << std::endl;
		os << "Input Assembler Command:\t\t" << assembler.read().to_string(sc_dt::SC_HEX) << std::endl;
		os << "Address in shared memory:\t\t" << address.read().to_string(sc_dt::SC_HEX) << std::endl;
		os << "Line in data cache:\t\t" << line.read().to_string(sc_dt::SC_BIN) << std::endl;
		os << "Place in data cache line:\t\t" << place.read().to_string(sc_dt::SC_BIN) << std::endl;
	}

	/*!
	 *  \brief Separate Assembler Command parts
	 *
	 *  \details
	 *  A Assembler Command consists of 32bits with the following structure:
	 *
	 *  [31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|10|09|08|07|06|05|04|03|02|01|00]
	 *	- Address: 31 ... 16 [max 64KiB memory]
	 *	- Line: 15 ... 13 [max 8 Lines]
	 *	- Place: 12 ... 06 [max 126 Places, '0b1111111'(127d) decodes whole cache line]
	 *	- command: 05 ... 00 [max 32 Commands]
	 *
	 */
	void separate();

	/*!
	 * \brief Defaulted Destructor
	 */
	virtual ~CommandInterpreter() = default;


private:
	//Forbidden Constructors
	CommandInterpreter() = delete;
	CommandInterpreter(const CommandInterpreter& src) = delete;
	CommandInterpreter& operator=(const CommandInterpreter& src) = delete;
	CommandInterpreter(CommandInterpreter&& src) = delete;
	CommandInterpreter& operator=(CommandInterpreter&& src) = delete;

}; // end class CommandInterpreter

} // end namespace CGRA

#endif // COMMANDINTERPRETER_H_
