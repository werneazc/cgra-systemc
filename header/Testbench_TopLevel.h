/*
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

#ifndef TESTBENCH_TOPLEVEL_H
#define TESTBENCH_TOPLEVEL_H

#include <systemc>
#include "Typedef.h"
#include "TopLevel.h"

namespace cgra {
/*!
 * \class Testbench_TopLevel
 * 
 * \brief Testbench vor a complete architecture instance.
 * 
 * \details
 * 
 */
class Testbench_TopLevel :  sc_core::sc_module
{
public:
    //Component ports
    sc_core::sc_in<cgra::clock_type_t> clk{"clock"};
    //!< \brief Clock input port for Testbench
    sc_core::sc_in<cgra::TopLevel::finish_type_t> finish{"finish"};
    //!< \brief Finish signal of TopLevel to show a finished execution of the assembler code.
    sc_core::sc_out<cgra::TopLevel::run_type_t> run{"run"};
    //!< \brief Start execution of architecture.
    sc_core::sc_out<cgra::TopLevel::pause_type_t> pause{"pause"};
    //!< \brief Pause execution of the architecture.
    sc_core::sc_out<cgra::reset_type_t> rst{"reset"};
    //!< \brief Reset VirtualChannels and assembler program.
    
    
    
    //Constructor
    SC_HAS_PROCESS(Testbench_TopLevel);
    /*! 
     * \brief Standard constructor for Testbench_TopLevel
     */
    Testbench_TopLevel(const sc_core::sc_module_name& nameA, cgra::MMU& mmuA);
    
    /*!
     * \brief Destructor
     */
    ~Testbench_TopLevel() = default;
    
    /*!
     * \brief Return module kind.
     */
    virtual const char* kind() const { return "Testbench"; };
    
    /*!
     * \brief Write module name to defined outstream.
     * 
     * \param[out] os Target outstream for module name. [default: std::cout]
     */
    virtual void print(::std::ostream& os = ::std::cout) const;
    /*!
     * \brief Dump architecture information.
     * 
     * \param[out] os Target outstream for information dump.
     */
    virtual void dump(::std::ostream& os = ::std::cout) const;
    
    /*!
     * \brief Initialize output ports
     */
    virtual void end_of_elaboration();
    
    /*!
     * \brief Stimulus function for TopLevel
     */
    void stimuli(void);
    
private: 
    
    cgra::MMU& m_mmu;
    
    //Forbidden Constructors
    Testbench_TopLevel() = delete;                                          //!< \brief Every component needs a unique module name.
    Testbench_TopLevel(const Testbench_TopLevel& src) = delete;             //!< \brief Every component needs a unique module name. 
    Testbench_TopLevel& operator=(const Testbench_TopLevel& src) = delete;  //!< \brief Every component needs a unique module name. 
    Testbench_TopLevel(Testbench_TopLevel&& src) = delete;                  //!< \brief Move constructor not implemented for sc_module base class.
    Testbench_TopLevel& operator=(Testbench_TopLevel&& src) = delete;       //!< \brief Move constructor not implemented for sc_module base class.

    
}; /* End of Testbench_TopLevel */

} /* End of namespace cgra */

#endif // TESTBENCH_TOPLEVEL_H
