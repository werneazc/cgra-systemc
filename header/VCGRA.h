/*
 * VCGRA.h
 *
 *  Created on: Jan 20, 2018
 *      Author: andrewerner
 */

#ifndef VCGRA_H_
#define VCGRA_H_

#include <systemc>
#include <cstdint>
#include <array>
#include "Typedef.h"
#include "ProcessingElement.h"
#include "VirtualChannel.h"
#include "Demultiplexer.h"
#include "Synchronizer.h"
#include "Selector.h"

#ifdef GSYSC
#include <gsysc.h>
#endif

namespace cgra {
/*!
 * \class VCGRA
 *
 * \brief VCGRA instance with PEs and VirtualChannels
 *
 * \details
 * VCGRA instance with alternating levels of Processing Elements and Virtual
 * Channels. IT also includes Demultiplexers and Selectors for a the configuration
 * bitstreams loaded from the configuration caches.
 *
 */
class VCGRA : public sc_core::sc_module
{
public:
    typedef cgra::Processing_Element<cgra::cPeDataBitwidth,
                cgra::cPeDataBitwidth, cgra::cPeConfigLvSize> pe_type_t;
    //!< \brief Processing_Element type definition for VCGRA instance
    typedef cgra::VirtualChannel<
                cgra::cInputChannel_NumOfInputs,
                cgra::cInputChannel_InputBitwidth,
                cgra::cInputChannel_NumOfOutputs,
                cgra::cInputChannel_OutputBitwidth,
                cgra::cInputChannel_MuxScltBitwidth,
                cgra::cInputChannel_InternalBitwidth> input_channel_type_t;
    //!< \brief VirtualChannel type for first layer of a VCGRA instance
    typedef cgra::VirtualChannel<
                cgra::cChannel_NumOfInputs,
                cgra::cChannel_InputBitwidth,
                cgra::cChannel_NumOfOutputs,
                cgra::cChannel_OutputBitwidth,
                cgra::cChannel_MuxScltBitwidth,
                cgra::cChannel_InternalBitwidth> channel_type_t;
    //!< \brief VirtualChannel type for standard layer of a VCGRA instance
    typedef cgra::Demultiplexer<
                cgra::pe_config_type_t,
                cgra::cNumOfPe,
                cgra::cPeConfigBitWidth> demux_type_t;
    //!< \brief Demultiplexer type for distributing PE configuration
    typedef cgra::Selector<cgra::ch_config_type_t,1,24,24> in_ch_config_selector_type_t;
    //!< \brief Selector type for input channel configuration
    typedef cgra::Selector<cgra::ch_config_type_t,3,16,48> ch_config_selector_type_t;
    //!< \brief Selector type for general channel configurations
    typedef cgra::Selector<cgra::ch_config_type_t,1,4,4> sync_selector_type_t;
    //!< \brief Selector type for synchronization configuration
    typedef cgra::Synchronizer<pe_type_t::valid_type_t,
            cgra::cPeLevels.back()> synchronizer_type_t;
    //!< \brief Synchonizer type for PS synchronization with VCGRA
    typedef cgra::start_type_t start_type_t;
    //!< \brief VCGRA start port type definition
    typedef cgra::reset_type_t reset_type_t;
    //!< \brief VCGRA reset port type definition
    typedef cgra::ready_type_t ready_type_t;
    //!< \brief VCGRA ready port type definition
    typedef cgra::clock_type_t clock_type_t;
    //!< \brief VCGRA clock port type definition
    typedef pe_type_t::input_type_t data_input_type_t;
    //!< \brief VCGRA data input type
    typedef pe_type_t::output_type_t data_output_type_t;
    //!< \brief VCGRA data output type


#ifndef GSYSC
    //Entity ports
    sc_core::sc_in<cgra::clock_type_t> clk{"clk"};
    //!< \brief VCGRA clock port
    sc_core::sc_in<cgra::start_type_t> start{"start"};
    //!< \brief VCGRA start port
    sc_core::sc_in<cgra::reset_type_t> rst{"rst"};
    //!< \brief VCGRA reset port
    sc_core::sc_in<cgra::pe_config_type_t> pe_config{"pe_config"};
    //!< \brief VCGRA PE configuration port
    sc_core::sc_in<cgra::ch_config_type_t> ch_config{"ch_config"};
    //!< \brief VCGRA VirtualChannel configuration port
    std::array<sc_core::sc_in<data_input_type_t>, cgra::cInputChannel_NumOfInputs> data_inputs;
    //!< \brief VCGRA data inputs to first VirtualChannel level
    sc_core::sc_out<cgra::ready_type_t> ready{"ready"};
    //!< \brief VCGRA ready port
    std::array<sc_core::sc_out<data_output_type_t>, cgra::cPeLevels.back()> data_outputs;
    //!< \brief VCGRA data outputs
#else
    sc_in<cgra::clock_type_t> clk{"clk"};
    //!< \brief VCGRA clock port
    sc_in<cgra::start_type_t> start{"start"};
    //!< \brief VCGRA start port
    sc_in<cgra::reset_type_t> rst{"rst"};
    //!< \brief VCGRA reset port
    sc_in<cgra::pe_config_type_t> pe_config{"pe_config"};
    //!< \brief VCGRA PE configuration port
    sc_in<cgra::ch_config_type_t> ch_config{"ch_config"};
    //!< \brief VCGRA VirtualChannel configuration port
    std::array<sc_in<data_input_type_t>, cgra::cInputChannel_NumOfInputs> data_inputs;
    //!< \brief VCGRA data inputs to first VirtualChannel level
    sc_out<cgra::ready_type_t> ready{"ready"};
    //!< \brief VCGRA ready port
    std::array<sc_out<data_output_type_t>, cgra::cPeLevels.back()> data_outputs;
    //!< \brief VCGRA data outputs
#endif

    //Member functions
    //------------------
    /*!
     * \brief General constructor
     */
    VCGRA(const sc_core::sc_module_name& nameA);

    /*!
     * \brief Initialize output signals
     */
    virtual void end_of_elaboration() override;

    /*!
     * \brief Print sc_module type
     *
     * \return Module kind.
     */
    const char * kind() const override
    { return "VCGRA"; }


    /*!
     * \brief Print VCGRA instance name
     *
     * \param[out] os Outstream for string of sc_module instance name
     */
    virtual void print(::std::ostream& os = std::cout) const override
    { os << name(); }

    /*!
     * \brief Dump sc_module internal information
     *
     * \param[out] os Outstream for information dumping
     */
    virtual void dump(::std::ostream& os = std::cout) const override;

#ifdef MCPAT
    /**
     * \brief Dump runtime statistics for McPAT simulation
     *
     * \param os Define used outstream [default: std::cout]
     */
    void dumpMcpatStatistics(std::ostream& os = ::std::cout) const;
#endif

    /*!
     * \brief Destructor
     */
    ~VCGRA() = default;

private:

    //Forbidden Constructors
    VCGRA() = delete;
    VCGRA(const VCGRA& src) = delete;               //!< \brief because sc_module could not be copied
    VCGRA& operator=(const VCGRA& src) = delete;    //!< \brief because sc_module could not be copied
    VCGRA(VCGRA&& src) = delete;                    //!< \brief because move not implemented for sc_module
    VCGRA& operator=(VCGRA&& src) = delete;         //!< \brief because move not implemented for sc_module

#ifndef GSYSC
    //VCGRA component instances
    sc_core::sc_vector<pe_type_t> m_pe_instances{"VCGRA_PEs",};
    //!< \brief Array of PE instances of the current VCGRA
    sc_core::sc_vector<channel_type_t> m_channel_instances{"VCGRA_Channels"};
    //!< \brief Array of VirtualChannel instances of the current VCGRA
#else
    //VCGRA component instances
    sc_vector<pe_type_t> m_pe_instances{"VCGRA_PEs",};
    //!< \brief Array of PE instances of the current VCGRA
    sc_vector<channel_type_t> m_channel_instances{"VCGRA_Channels"};
    //!< \brief Array of VirtualChannel instances of the current VCGRA
#endif

    input_channel_type_t m_input_channel{"Input_Channel"};
    //!< \brief VirtualChannel instance of the first level
    synchronizer_type_t m_sync{"VCGRA_Sync"};
    //!< \brief Synchronizer for VCGRA ready signal generation.
    demux_type_t m_pe_config_demux{"PE_config_Demux"};
    //!< \brief Distributes PE configuration bitstream to PE instances
    in_ch_config_selector_type_t m_input_channel_selector{
        "In_Channel_Selector",
        0,
        cgra::cVChConfigBitWidth};
    //!< \brief Selector to distribute VirtualChannel configuration to input channel.
    ch_config_selector_type_t m_channel_selector{
        "Channel_Selector",
        24,
        cgra::cVChConfigBitWidth};
    //!< \brief Selector to distribute VirtualChannel configurations to general channels.
    sync_selector_type_t m_sync_selector{
        "Sync_Selector",
        72,
        cgra::cVChConfigBitWidth};
    //!< \brief Selector to distribute synchronization mask to general Synchronizer.

#ifndef GSYSC
    //Internal signals
    sc_core::sc_signal<in_ch_config_selector_type_t::configpart_type_t>
        s_input_ch_configuration{"S_input_channel_configuration"};
    //!< \brief Configuration part for input channel.
    std::array<sc_core::sc_signal<ch_config_selector_type_t::configpart_type_t>,
        3> s_ch_configurations;
    //!< \brief Configuration parts for each VirtualChannel per layer.
    std::array<sc_core::sc_signal<demux_type_t::configpart_type_t>,
        cgra::cNumOfPe> s_pe_configurations;
    //!< \brief Configuration parts for each PE in VCGRA instance.
    sc_core::sc_signal<sync_selector_type_t::configpart_type_t>
        s_sync_configuration{"s_synchronizer_configuration"};
    //!< \brief Configuration part for Synchronizer mask
    std::array<sc_core::sc_signal<pe_type_t::enable_type_t>,
        2 * cgra::cNumOfPe> s_enables;
    //!< \brief PE enable signals
   std::array<sc_core::sc_signal<pe_type_t::input_type_t>,
        2* cgra::cNumOfPe> s_pe_data_input_signals;
    //!< \brief PE input signals
   std::array<sc_core::sc_signal<pe_type_t::output_type_t>,
        cgra::cNumOfPe - cgra::cPeLevels.back()> s_pe_data_output_signals;
    //!< \brief PE output signals
    std::array<sc_core::sc_signal<pe_type_t::valid_type_t>,
        cgra::cNumOfPe> s_pe_valid_signals;
    //!< \brief PE valid signals
#else
    //Internal signals
    sc_signal<in_ch_config_selector_type_t::configpart_type_t>
        s_input_ch_configuration{"S_input_channel_configuration"};
    //!< \brief Configuration part for input channel.
    std::array<sc_signal<ch_config_selector_type_t::configpart_type_t>,
        3> s_ch_configurations;
    //!< \brief Configuration parts for each VirtualChannel per layer.
    std::array<sc_signal<demux_type_t::configpart_type_t>,
        cgra::cNumOfPe> s_pe_configurations;
    //!< \brief Configuration parts for each PE in VCGRA instance.
    sc_signal<sync_selector_type_t::configpart_type_t>
        s_sync_configuration{"s_synchronizer_configuration"};
    //!< \brief Configuration part for Synchronizer mask
    std::array<sc_signal<pe_type_t::enable_type_t>,
        2 * cgra::cNumOfPe> s_enables;
    //!< \brief PE enable signals
   std::array<sc_signal<pe_type_t::input_type_t>,
        2* cgra::cNumOfPe> s_pe_data_input_signals;
    //!< \brief PE input signals
   std::array<sc_signal<pe_type_t::output_type_t>,
        cgra::cNumOfPe - cgra::cPeLevels.back()> s_pe_data_output_signals;
    //!< \brief PE output signals
    std::array<sc_signal<pe_type_t::valid_type_t>,
        cgra::cNumOfPe> s_pe_valid_signals;
    //!< \brief PE valid signals
#endif

    /*!
     * \struct pe_creator
     *
     * \brief Functor to create PEs for Processing_Element vector.
     *
     * \details
     * It is necessary to create a unique ID for a Processing_Element.
     * The standard Constructor for a vector of SystemC
     * modules doesn't consists additional parameters
     * for further constructor arguments of its elements.
     * Therefore, we use a Functor to create the Processing_Element which than
     * is passed into the array.
     *
     * \param[in] pe_id Unique ID of a processing element.
     *
     */
    struct pe_creator
    {
    public:

        //Contructor
        /*!
         * \brief Constructor for Processing_Element creator
         * \param[in] pe_id Unique ID of a processing element.
         */
        pe_creator(uint32_t pe_id) : m_pe_id(pe_id) {};

        //Functor
        /*!
         * \brief Functor for an pe_type instance
         * \details
         * Parameters are defined by sc_core::sc_vector
         * constructor with creator.
         */
        pe_type_t* operator()(const char* name, size_t size)
        {
            return new pe_type_t(name, m_pe_id++);
        }

        uint32_t m_pe_id;
        //!< Unique Processing_Element ID
    };
};

} /* End namespace cgra */

#endif /* VCGRA_H_ */
