/*
 * testbench_vcgra_only.hpp
 *
 *  Created on: Dec 04, 2020
 *      Author: andrewerner
 */

#include "../VCGRA.h"
#include <systemc>
#include <vector>

namespace cgra
{

class Testbench : public sc_core::sc_module
{
  public:
    // Type definitions
    typedef typename VCGRA::data_input_type_t data_input_type_t;
    //!< \brief VCGRA data input type
    typedef typename VCGRA::data_output_type_t data_output_type_t;
    //!< \brief VCGRA data output type

#ifndef GSYSC
    // Entity ports
    sc_core::sc_in<cgra::clock_type_t> clk{"clk"};
    //!< \brief VCGRA clock port
    sc_core::sc_out<cgra::start_type_t> start{"start"};
    //!< \brief VCGRA start port
    sc_core::sc_out<cgra::reset_type_t> rst{"rst"};
    //!< \brief VCGRA reset port
    sc_core::sc_out<cgra::pe_config_type_t> pe_config{"pe_config"};
    //!< \brief VCGRA PE configuration port
    sc_core::sc_out<cgra::ch_config_type_t> ch_config{"ch_config"};
    //!< \brief VCGRA VirtualChannel configuration port
    std::array<sc_core::sc_out<data_input_type_t>, cgra::cInputChannel_NumOfInputs> data_inputs;
    //!< \brief VCGRA data inputs to first VirtualChannel level
    sc_core::sc_in<cgra::ready_type_t> ready{"ready"};
    //!< \brief VCGRA ready port
    std::array<sc_core::sc_in<data_output_type_t>, cgra::cPeLevels.back()> data_outputs;
    //!< \brief VCGRA data outputs
#else
    // Entity ports
    sc_in<cgra::clock_type_t> clk{"clk"};
    //!< \brief VCGRA clock port
    sc_out<cgra::start_type_t> start{"start"};
    //!< \brief VCGRA start port
    sc_out<cgra::reset_type_t> rst{"rst"};
    //!< \brief VCGRA reset port
    sc_out<cgra::pe_config_type_t> pe_config{"pe_config"};
    //!< \brief VCGRA PE configuration port
    sc_out<cgra::ch_config_type_t> ch_config{"ch_config"};
    //!< \brief VCGRA VirtualChannel configuration port
    std::array<sc_out<data_input_type_t>, cgra::cInputChannel_NumOfInputs> data_inputs;
    //!< \brief VCGRA data inputs to first VirtualChannel level
    sc_in<cgra::ready_type_t> ready{"ready"};
    //!< \brief VCGRA ready port
    std::array<sc_in<data_output_type_t>, cgra::cPeLevels.back()> data_outputs;
    //!< \brief VCGRA data outputs
#endif

    // Member functions
    //-----------------

    SC_HAS_PROCESS(Testbench);
    /**
     * @brief General construtor
     *
     * @param nameA         SystemC module name
     * @param imagePathA    Path to image in PGM-format
     */
    Testbench(const sc_core::sc_module_name &nameA, std::string imagePathA);

    // Deleted constructors
    Testbench() = delete;
    //!< @brief SystemC requires unique module name
    Testbench(const Testbench &rhs) = delete;
    //!< @brief sc_module cannot being copied
    Testbench(Testbench &&rhs) = delete;
    //!< @brief sc_module cannot being moved
    Testbench &operator=(const Testbench &rhs) = delete;
    //!< @brief sc_module cannot being copied
    Testbench &operator=(Testbench &&rhs) = delete;
    //!< @brief sc_module cannot being moved

    /**
     * @brief Destroy the Testbench object
     */
    ~Testbench() override = default;

    /**
     * @brief Append new PE configuration
     *
     * @param peConfA PE configuration as a byte stream
     *
     * @return Index of added configuration
     */
    std::size_t appendPeConfiguration(std::vector<uint8_t> peConfA);

    /**
     * @brief Append new virtual channel configuration
     *
     * @param chConfA  Virtual channel configuration as a byte stream
     *
     * @return Index of added configuration
     */
    std::size_t appendChConfiguration(std::vector<uint8_t> chConfA);

    /**
     * @brief Write result image as pgm-file
     *
     * @param pathA Path to the target file
     * @return true Success
     * @return false Failure
     */
    bool writeResultImagetoFile(const std::string &pathA) const;

    /**
     * @brief Print kind of SystemC module
     */
    const char *kind() const override
    {
        return "VCGRA Testbench";
    }

    /**
     * @brief Return name of SystemC module
     */
    void print(::std::ostream &os = ::std::cout) const override
    {
        os << name();
    }

    void dump(::std::ostream &os = ::std::cout) const override;

    /**
     * @brief Process the stimulation of the VCGRA instance
     */
    void stimuli();

    /**
     * @brief Initialize ports before starting simulation
     */
    void end_of_elaboration() override;

  private:
    std::string mImagePath;
    //!< @brief Stores the path to the local image to process

    const std::array<int16_t, 3 * 3> sobelx{1, 0, -1, 2, 0, -2, 1, 0, -1};
    //!< Sobel coefficients in x-direction
    const std::array<int16_t, 3 * 3> unit{0, 0, 0, 0, 1, 0, 0, 0, 0};
    //!< Sobel coefficients in x-direction
    const std::array<int16_t, 3 * 3> sobely{1, 2, 1, 0, 0, 0, -1, -2, -1};
    //!< Sobel coefficients in y-direction

    static constexpr uint8_t cInputSize{64};
    //!< Input image dimension
    std::array<uint16_t, cInputSize * cInputSize> mInputValues;
    //!< Input image values

    static constexpr uint8_t cOutputSize{64};
    //!< Output image dimension
    std::array<int16_t, cOutputSize * cOutputSize> mSobelxValues;
    //!< Output image values after sobel operation in x direction
    std::array<int16_t, cOutputSize * cOutputSize> mSobelyValues;
    //!< Output image values after sobel operation in y direction
    std::array<int16_t, cOutputSize * cOutputSize> mResultValues;
    //!< Output image values

    std::vector<std::vector<uint8_t>> mPeConfigs;
    //!< @brief Stores the available configurations for the VCGRA PEs
    std::vector<std::vector<uint8_t>> mChConfigs;
    //!< @brief Stores the available configurations for the VCGRA virtual channels
};

} // namespace cgra
