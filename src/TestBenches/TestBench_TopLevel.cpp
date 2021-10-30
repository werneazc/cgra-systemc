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

#include "TestBench_TopLevel.h"

namespace cgra {

Testbench_TopLevel::Testbench_TopLevel(const sc_core::sc_module_name& nameA, cgra::MMU& mmuA) :
sc_core::sc_module(nameA), m_mmu(mmuA)
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

    //Perform Sobel X direction
    std::array<int16_t, 3*3> sobelx{1, 0, -1, 2, 0, -2, 1, 0, -1};
    m_mmu.write_shared_memory(0x170, sobelx.data(), sobelx.max_size() * sizeof(int16_t));
    auto t_start = sc_core::sc_time_stamp();
    run.write(true);
    wait(finish.posedge_event());
    auto t_stopSobelx = sc_core::sc_time_stamp();
    run.write(false);
    wait(220, sc_core::SC_NS);
    
    std::array<int16_t, 62*62> t_resultx;
    t_resultx.fill(0);
    m_mmu.read_shared_memory<int16_t>(0x2300, t_resultx.data(), 62*62);
    
    //Perform Sobel Y direction
    std::array<int16_t, 3*3> sobely{1, 2, 1, 0, 0, 0, -1, -2, -1};
    m_mmu.write_shared_memory(0x170, sobely.data(), sobely.max_size() * sizeof(int16_t));
    auto t_startSobely = sc_core::sc_time_stamp();
    run.write(true);
    wait(finish.posedge_event());
    auto t_stopSobely = sc_core::sc_time_stamp();
    wait(50, sc_core::SC_NS);
    sc_core::sc_stop();
    
    std::cout << "\n\nSimulation timings: " << "\n" ;
    std::cout << "overall: " << (t_stopSobely - t_start).to_string() << "\n";
    std::cout << "sobel x-direction: " << (t_stopSobelx - t_start)<< "\n";
    std::cout << "sobel y-direction: " << (t_stopSobely - t_startSobely) << std::endl;
    
    std::array<int16_t, 62*62> t_resulty;
    t_resulty.fill(0);
    m_mmu.read_shared_memory<int16_t>(0x2300, t_resulty.data(), 62*62);
    
    //Build sum of absolute values
    for(uint32_t idx = 0; idx < t_resultx.size(); ++idx) {
        t_resultx.at(idx) = std::sqrt(t_resultx.at(idx) * t_resultx.at(idx) + t_resulty.at(idx) * t_resulty.at(idx));
    }
    
    m_mmu.write_shared_memory<int16_t>(0x2300, t_resultx.data(), t_resultx.size() * sizeof(int16_t));
    
    return;
}

} /* End namespace cgra */
