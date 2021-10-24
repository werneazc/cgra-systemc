#include "CommandInterpreter.h"

namespace cgra {

CommandInterpreter::CommandInterpreter(const sc_core::sc_module_name& nameA) : sc_core::sc_module(nameA)
{
    SC_METHOD(separate);
    sensitive << this->assembler.value_changed();
}

void CommandInterpreter::separate()
{
    //Get information parts from assembly command
    this->command.write(this->assembler.read()(5,0));
    this->place.write(this->assembler.read()(12,6));
    this->line.write(this->assembler.read()(15,13));
    this->address.write(this->assembler.read()(31,16));
}

} // end namespace cgra
