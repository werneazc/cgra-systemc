/**
 * @file Testbench_VcgraPreloaders.hpp
 * @author André Werner (andre.werner@b-tu.de)
 * @brief Testbench for architecture with a VCGRA and configuration pre-fetchers
 * @version 0.1
 * @date 2021-01-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef TESTBENCH_VCGRAPRELOADERS_H_
#define TESTBENCH_VCGRAPRELOADERS_H_

#include "TopLevel_VcgraPreloaders.hpp"
#include <systemc>

#ifdef GSYSC
#include <gsysc.h>
#endif

namespace cgra
{

struct Testbench : public sc_core::sc_module
{
    // Type definitions

    #ifndef GSYSC
        // Entity Ports
        // ------------
        sc_core::sc_in<TopLevel::clock_type_t> clk{"clk"};
        //!< Clock signal for TopLevel components

        // VCGRA
        sc_core::sc_out<TopLevel::start_type_t> start{"vcgra_start"};
        //!< VCGRA start signal
        sc_core::sc_out<TopLevel::reset_type_t> reset{"vcgra_reset"};
        //!< VCGRA reset signal
        sc_core::sc_vector<sc_core::sc_out<TopLevel::data_input_type_t>> data_inputs{"vcgra_data_inputs",
                                                                                    cgra::cInputChannel_NumOfInputs};
        //!< VCGRA data inputs
        sc_core::sc_in<TopLevel::ready_type_t> ready{"vcgra_ready"};
        sc_core::sc_vector<sc_core::sc_in<TopLevel::data_output_type_t>> data_outputs{"vcgra_data_outputs",
                                                                                       cgra::cPeLevels.back()};
        //!< VCGRA data outputs

        // PE configuration pre-fetcher
        sc_core::sc_out<TopLevel::write_enable_type_t> pe_write_enable{"pe_prefetcher_wren"};
        //!< Write enable signal for PE pre-fetcher
        sc_core::sc_out<TopLevel::pe_stream_type_t> pe_input_stream{"pe_prefetcher_istream"};
        //!< Data input stream for PE pre-fetcher
        sc_core::sc_out<TopLevel::pe_select_type_t> pe_slct_in{"pe_select_in"};
        //!< Select input signal for PE pre-fetcher
        sc_core::sc_out<TopLevel::pe_select_type_t> pe_slct_out{"pe_select_out"};
        //!< Select output signal for PE pre-fetcher
        sc_core::sc_in<TopLevel::ack_type_t> pe_ack{"pe_prefetcher_ack"};
        //!< Acknowledge signal from PE pre-fetcher

        // Virtual channel configuration pre-fetcher
        sc_core::sc_out<TopLevel::write_enable_type_t> ch_write_enable{"ch_prefetcher_wren"};
        //!< Write enable signal for virtual channel pre-fetcher
        sc_core::sc_out<TopLevel::ch_stream_type_t> ch_input_stream{"ch_prefetcher_istream"};
        //!< Data input stream for virtual channel pre-fetcher
        sc_core::sc_out<TopLevel::ch_select_type_t> ch_slct_in{"ch_select_in"};
        //!< Select input signal for vitual channel pre-fetcher
        sc_core::sc_out<TopLevel::ch_select_type_t> ch_slct_out{"ch_select_out"};
        //!< Select output signal for virtual channel pre-fetcher
        sc_core::sc_in<TopLevel::ack_type_t> ch_ack{"ch_prefetcher_ack"};
        //!< Acknowledge signal from virtual channel pre-fetcher
    #else
        // Entity Ports
        // ------------
        sc_in<TopLevel::clock_type_t> clk{"clk"};
        //!< Clock signal for TopLevel components

        // VCGRA
        sc_out<TopLevel::start_type_t> start{"vcgra_start"};
        //!< VCGRA start signal
        sc_out<TopLevel::reset_type_t> reset{"vcgra_reset"};
        //!< VCGRA reset signal
        sc_vector<sc_out<TopLevel::data_input_type_t>> data_inputs{"vcgra_data_inputs",
                                                                    cgra::cInputChannel_NumOfInputs};
        //!< VCGRA data inputs
        sc_in<TopLevel::ready_type_t> ready{"vcgra_ready"};
        sc_vector<sc_in<TopLevel::data_output_type_t>> data_outputs{"vcgra_data_outputs",
                                                                        cgra::cPeLevels.back()};
        //!< VCGRA data outputs

        // PE configuration pre-fetcher
        sc_out<TopLevel::write_enable_type_t> pe_write_enable{"pe_prefetcher_wren"};
        //!< Write enable signal for PE pre-fetcher
        sc_out<TopLevel::pe_stream_type_t> pe_input_stream{"pe_prefetcher_istream"};
        //!< Data input stream for PE pre-fetcher
        sc_out<TopLevel::pe_select_type_t> pe_slct_in{"pe_select_in"};
        //!< Select input signal for PE pre-fetcher
        sc_out<TopLevel::pe_select_type_t> pe_slct_out{"pe_select_out"};
        //!< Select output signal for PE pre-fetcher
        sc_in<TopLevel::ack_type_t> pe_ack{"pe_prefetcher_ack"};
        //!< Acknowledge signal from PE pre-fetcher

        // Virtual channel configuration pre-fetcher
        sc_out<TopLevel::write_enable_type_t> ch_write_enable{"ch_prefetcher_wren"};
        //!< Write enable signal for virtual channel pre-fetcher
        sc_out<TopLevel::ch_stream_type_t> ch_input_stream{"ch_prefetcher_istream"};
        //!< Data input stream for virtual channel pre-fetcher
        sc_out<TopLevel::ch_select_type_t> ch_slct_in{"ch_select_in"};
        //!< Select input signal for vitual channel pre-fetcher
        sc_out<TopLevel::ch_select_type_t> ch_slct_out{"ch_select_out"};
        //!< Select output signal for virtual channel pre-fetcher
        sc_in<TopLevel::ack_type_t> ch_ack{"ch_prefetcher_ack"};
        //!< Acknowledge signal from virtual channel pre-fetcher
    #endif

    // Forbidden Constructors
    Testbench() = delete;
    Testbench(const Testbench &src) = delete;
    Testbench(Testbench &&src) = delete;
    Testbench &operator=(const Testbench &rhs) = delete;
    Testbench &operator=(Testbench &&rhs) = delete;

    SC_HAS_PROCESS(Testbench);
    /**
     * @brief Construct a new Testbench object
     *
     * @param[in] nameA SystemC unique module name
     * @param[in] imagePathA Path to image to process
     */
    explicit Testbench(const sc_core::sc_module_name &nameA, std::string imagePathA);

    /**
     * @brief Initialize ports and signals
     */
    void end_of_elaboration() override;

    /**
     * @brief Return sc_module kind
     *
     * @return const char*
     */
    const char *kind() const override
    {
        return "Testbench";
    }

    /**
     * @brief Print sc_module name to os
     *
     * @param[out] os Out stream to write to
     */
    void print(::std::ostream &os = std::cout) const override
    {
        os << name();
    }

    /**
     * @brief Dump sc_module information to os
     *
     * @param[out] os Out stream to write to
     */
    void dump(::std::ostream &os = std::cout) const override;

    /**
     * @brief Append new PE configuration
     *
     * @param[in] peConfA PE configuration as a byte stream
     *
     * @return Index of added configuration
     */
    std::size_t appendPeConfiguration(std::vector<uint8_t> peConfA);

    /**
     * @brief Append new virtual channel configuration
     *
     * @param[in] chConfA  Virtual channel configuration as a byte stream
     *
     * @return Index of added configuration
     */
    std::size_t appendChConfiguration(std::vector<uint8_t> chConfA);

    /**
     * @brief Write result image as pgm-file
     *
     * @param[in] pathA Path to the target file
     * @return true Success
     * @return false Failure
     */
    bool writeResultImagetoFile(const std::string &pathA) const;

    /**
     * @brief Perform access to the architecture and back
     */
    void stimuli();

    /**
     * @brief Destroy the Testbench object
     */
    ~Testbench() override = default;

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
#endif // TESTBENCH_VCGRAPREFETCHERS_H_