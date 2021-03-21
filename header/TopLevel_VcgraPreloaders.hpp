/**
 * @file TopLevel_VcgraPreloaders.hpp
 * @author André Werner (andre.werner@b-tu.de)
 * @brief Top level for architecture including a VCGRA and confiuration prefetchers
 * @version 0.1
 * @date 2021-01-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef HEADER_TOPLEVEL_VCGRAPRELOADERS_H_
#define HEADER_TOPLEVEL_VCGRAPRELOADERS_H_

#include "ConfigurationCache.h"
#include "Typedef.h"
#include "VCGRA.h"
#include <systemc>

namespace cgra
{

struct TopLevel : public sc_core::sc_module
{
    // Type definitions
    typedef cgra::clock_type_t clock_type_t;
    //!< Typedef for clock type of module
    typedef cgra::reset_type_t reset_type_t;
    //!< Typedef for reset type of module
    typedef cgra::ready_type_t ready_type_t;
    //!< Typedef for ready type of module
    typedef cgra::start_type_t start_type_t;
    //!< Typedef for start type of module
    typedef cgra::cache_write_type_t write_enable_type_t;
    //!< Typedef for write enable type of module
    typedef cgra::cache_ack_type_t ack_type_t;
    //!< Typedef for acknowledge type of module

    typedef typename cgra::ch_config_cache_type_t::stream_type_t pe_stream_type_t;
    //!< Typedef for configuration data input stream
    typedef typename cgra::pe_config_cache_type_t::select_type_t pe_select_type_t;
    //!< Typedef for pre-fetcher input and output selection
    typedef typename cgra::pe_config_cache_type_t::config_type_t pe_config_type_t;
    //!< Typedef for PE configuration pre-fetcher configuration store and output type

    typedef typename cgra::ch_config_cache_type_t::stream_type_t ch_stream_type_t;
    //!< Typedef for configuration data input stream
    typedef typename cgra::ch_config_cache_type_t::select_type_t ch_select_type_t;
    //!< Typedef for pre-fetcher input and output selection
    typedef typename cgra::ch_config_cache_type_t::config_type_t ch_config_type_t;
    //!< Typedef for virtual CH configuration pre-fetcher configuration store and output type

    typedef typename cgra::VCGRA::data_input_type_t data_input_type_t;
    //!<
    typedef typename cgra::VCGRA::data_output_type_t data_output_type_t;
    //!<

    // Entity Ports
    // ------------
    sc_core::sc_in<clock_type_t> clk{"clk"};
    //!< Clock signal for TopLevel components

    // VCGRA
    sc_core::sc_in<start_type_t> start{"vcgra_start"};
    //!< VCGRA start signal
    sc_core::sc_in<reset_type_t> reset{"vcgra_reset"};
    //!< VCGRA reset signal
    sc_core::sc_vector<sc_core::sc_in<data_input_type_t>> data_inputs{"vcgra_data_inputs",
                                                                      cgra::cInputChannel_NumOfInputs};
    //!< VCGRA data inputs
    sc_core::sc_out<ready_type_t> ready{"vcgra_ready"};
    sc_core::sc_vector<sc_core::sc_out<data_output_type_t>> data_outputs{"vcgra_data_outputs", cgra::cPeLevels.back()};
    //!< VCGRA data outputs

    // PE configuration pre-fetcher
    sc_core::sc_in<write_enable_type_t> pe_write_enable{"pe_prefetcher_wren"};
    //!< Write enable signal for PE pre-fetcher
    sc_core::sc_in<pe_stream_type_t> pe_input_stream{"pe_prefetcher_istream"};
    //!< Data input stream for PE pre-fetcher
    sc_core::sc_in<pe_select_type_t> pe_slct_in{"pe_select_in"};
    //!< Select input signal for PE pre-fetcher
    sc_core::sc_in<pe_select_type_t> pe_slct_out{"pe_select_out"};
    //!< Select output signal for PE pre-fetcher
    sc_core::sc_out<ack_type_t> pe_ack{"pe_prefetcher_ack"};
    //!< Acknowledge signal from PE pre-fetcher

    // Virtual channel configuration pre-fetcher
    sc_core::sc_in<write_enable_type_t> ch_write_enable{"ch_prefetcher_wren"};
    //!< Write enable signal for virtual channel pre-fetcher
    sc_core::sc_in<ch_stream_type_t> ch_input_stream{"ch_prefetcher_istream"};
    //!< Data input stream for virtual channel pre-fetcher
    sc_core::sc_in<ch_select_type_t> ch_slct_in{"ch_select_in"};
    //!< Select input signal for vitual channel pre-fetcher
    sc_core::sc_in<ch_select_type_t> ch_slct_out{"ch_select_out"};
    //!< Select output signal for virtual channel pre-fetcher
    sc_core::sc_out<ack_type_t> ch_ack{"ch_prefetcher_ack"};
    //!< Acknowledge signal from virtual channel pre-fetcher

    // Forbidden constructors
    // ----------------------
    TopLevel() = delete;
    TopLevel(const TopLevel &src) = delete;
    TopLevel(TopLevel &&src) = delete;
    TopLevel &operator=(const TopLevel &rhs) = delete;
    TopLevel &operator=(TopLevel &&rhs) = delete;

    /**
     * @brief Construct a new TopLevel_VcgraPreloaders object
     *
     * @param[in] nameA Unique SystemC module name
     */
    explicit TopLevel(const sc_core::sc_module_name &nameA);

    /**
     * @brief Initialize output signals
     */
    void end_of_elaboration() override;

    /**
     * @brief Print sc_module kind
     *
     * @return const char*
     */
    const char *kind() const override
    {
        return "TopLevel_VCGRA_PreFetchers";
    }

    /**
     * @brief print module instance name
     *
     * @param[out] os Target output stream
     */
    void print(::std::ostream &os = std::cout) const override
    {
        os << name();
    }

    /**
     * @brief Dump sc_module information
     *
     * @param[out] os Target output stream
     */
    void dump(::std::ostream &os = std::cout) const override;

    /**
     * @brief Destructor
     */
    ~TopLevel() override = default;

    template <typename T>
    friend void sc_core::sc_trace(sc_core::sc_trace_file *tf, const sc_core::sc_signal_in_if<T> &object,
                                  const char *name);
    template <typename T>
    friend void sc_core::sc_trace(sc_core::sc_trace_file *tf, const sc_core::sc_signal_in_if<T> &object,
                                  const std::string &name);

  private:
    // Internal signals:
    // -----------------
    sc_core::sc_signal<pe_config_type_t> s_pe_config{"pe_configuration"};
    //!< PE configuration signal binds VCGRA and PE configuration pre-fetcher
    sc_core::sc_signal<ch_config_type_t> s_ch_config{"ch_configuration"};
    //!< PE configuration signal binds VCGRA and virtual channel configuration pre-fetcher
  public:
    // Components:
    // -----------
    cgra::VCGRA vcgra{"vcgra"};
    //!< VCGRA instance
    cgra::pe_config_cache_type_t pe_config_prefetcher{"pe_prefetcher", cgra::cPeConfigBitWidth};
    //!< PE configuration pre-fetcher instance
    cgra::ch_config_cache_type_t ch_config_prefetcher{"ch_prefetcher", cgra::cVChConfigBitWidth};
    //!< Virtual channel configuration pre-fetcher instance
};

} // namespace cgra
#endif // HEADER_TOPLEVEL_VCGRAPRELOADERS_H_