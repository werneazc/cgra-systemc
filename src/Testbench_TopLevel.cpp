/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2019  andrewerner <andre.werner-w2m@ruhr-uni-bochum.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Testbench_TopLevel.h"

namespace cgra {

Testbench_TopLevel::Testbench_TopLevel(const sc_core::sc_module_name& nameA) :
sc_core::sc_module(nameA)
{
    SC_THREAD(stimuli);
    sensitive << finish.pos();
    
    return;
}

void Testbench_TopLevel::end_of_elaboration()
{
    run.write(false);
    pause.write(false);
    rst.write(true);
    
    return;
}

void Testbench_TopLevel::print(::std::ostream& os) const
{
    os << name() << "\t\t" << kind() << std::endl;
    
    return;
}

void Testbench_TopLevel::dump(::std::ostream& os) const
{
    os << name() << "\t\t" << kind() << std::endl;
    os << "Port signal values:\n";
    os << "-------------------" << std::endl;
    os << "Signal state \"run\":\t\t" << std::setw(3) << std::boolalpha << run.read() << std::endl;
    os << "Signal state \"pause\":\t\t" << std::setw(3) << std::boolalpha << pause.read() << std::endl;
    os << "Signal state \"rst\":\t\t" << std::setw(3) << std::boolalpha << rst.read() << std::endl;
    os << "Signal state \"finish\":\t\t" << std::setw(3) << std::boolalpha << finish.read() << std::endl;
    
    return;
}

void Testbench_TopLevel::stimuli(void)
{
    wait(20, sc_core::SC_NS);
    run.write(true);
    wait(finish.posedge_event());
    wait(50, sc_core::SC_NS);
    sc_core::sc_stop();
    
    return;
}

} /* End namespace cgra */
