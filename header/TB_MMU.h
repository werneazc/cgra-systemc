/*
 * TB_MMU.h
 *
 *  Created on: Nov 3, 2018
 *      Author: andrewerner
 */

#ifndef HEADER_TB_MMU_H_
#define HEADER_TB_MMU_H_

#include <systemc>
#include <cstdint>
#include "Typedef.h"
#include "CommandInterpreter.h"
#include "ConfigurationCache.h"
#include "DataInCache.h"
#include "DataOutCache.h"

namespace cgra
{

/*!
 * \class TB_MMU
 *
 * \brief Testbench for MMU of a VCGRA instance
 *
 * \details
 *
 */
class TB_MMU : public sc_core::sc_module
{
public:
	typedef cgra::clock_type_t clock_type_t;
	//!< \brief MMU clock type
	typedef bool ready_type_t;
	//!< \brief Signal type to show current processing; Ready for new command if false
	typedef bool start_type_t;
	//!< \brief Start processing using data at address, line, cache select and place port
	typedef sc_dt::sc_lv<3> cache_select_type_t;
	//!< \brief MMU selection type for available caches
	typedef CommandInterpreter::address_type_t address_type_t;
	//!< \brief MMU address type for shared memory data location
	typedef CommandInterpreter::place_type_t place_type_t;
	//!< \brief MMU place type for data position in cache line of target/source cache
	typedef cgra::cache_ack_type_t ack_type_t;
	//!< \brief Cache transaction acknowledge type
	typedef cgra::cache_write_type_t write_type_t;
	//!< \brief Cache transmission write enable type
	typedef sc_dt::sc_lv<cgra::cDataStreamBitWidthConfCaches> conf_stream_type_t;
	//!< \brief Configuration stream type
	typedef sc_dt::sc_lv<cgra::cDataValueBitwidth> data_stream_type_t;
	//!< \brief Data stream type
	typedef cgra::data_input_cache_type_t::value_type_t value_type_t;
	//!< \brief Data Input Cache output value type
	typedef cgra::data_output_cache_type_t::value_type_t result_type_t;
	//!< \brief Data Output Cache result value type

	sc_core::sc_out<address_type_t> sAddress{"Address"};
	//!< \brief Shared memory address to load from/store to data
	sc_core::sc_out<place_type_t> sPlace{"Place"};
	//!< \brief Cache line place in target cache
	sc_core::sc_out<cache_select_type_t> sCache_select{"Cache_Select"};
	//!< \brief Cache selection for latest operation
	sc_core::sc_out<start_type_t> sStart{"Start"};
	//!< \brief A positive edge at this port starts a new operation sequence
	sc_core::sc_in<ready_type_t> sReady{"Ready"};
	//!< \brief Signal to show current processing; Ready for new command if false
	sc_core::sc_out<sc_dt::sc_lv<cgra::cSelectLineBitwidthPeConfCache>> sPe_sltIn{"PE_SelectIn"};
	//!< \brief Select current input line of PE configuration cache
	sc_core::sc_out<sc_dt::sc_lv<cgra::cSelectLineBitwidthPeConfCache>> sPe_sltOut{"PE_SelectOut"};
	//!< \brief Select current output line of PE configuration cache
	sc_core::sc_out<sc_dt::sc_lv<cgra::cSelectLineBitwidthVChConfCache>> sVCh_sltIn{"vCh_SelectIn"};
	//!< \brief Select current input line of vCh configuration cache
	sc_core::sc_out<sc_dt::sc_lv<cgra::cSelectLineBitwidthVChConfCache>> sVCh_sltOut{"vCh_SelectOut"};
	//!< \brief Select current output line of vCh configuration cache
	sc_core::sc_out<sc_dt::sc_lv<cgra::calc_bitwidth(cNumberDataInCacheLines)>> sDic_sltIn{"DataInputCache_SelectIn"};
	//!< \brief Select current input line of data input cache of a VCGRA
	sc_core::sc_out<sc_dt::sc_lv<cgra::calc_bitwidth(cNumberDataInCacheLines)>> sDic_sltOut{"DataInputCache_SelectOut"};
	//!< \brief Select current output line of data input cache of a VCGRA
	sc_core::sc_out<sc_dt::sc_lv<cgra::calc_bitwidth(cNumberDataOutCacheLines)>> sDoc_sltIn{"DataOutputCache_SelectIn"};
	//!< \brief Select current input line of data output cache of a VCGRA
	sc_core::sc_out<sc_dt::sc_lv<cgra::calc_bitwidth(cNumberDataOutCacheLines)>> sDoc_sltOut{"DataOutputCache_SelectOut"};
	//!< \brief Select current output line of data output cache of a VCGRA
	sc_core::sc_in<cgra::pe_config_cache_type_t::config_type_t> sPeCcCurrentConf{"PE_CC_CurrentConfig"};
	//!< \brief Current configuration from PE configuration cache
	sc_core::sc_in<cgra::ch_config_cache_type_t::config_type_t> sVChCcCurrentConf{"VCH_CC_CurrentConfig"};
	//!< \brief Current configuration from VirtualChannel configuration cache
	sc_core::sc_out<cgra::data_output_cache_type_t::update_type_t> sUpdate{"Update"};
	//!< \brief DataOutputCache update signal
	std::array<sc_core::sc_in<value_type_t>, cgra::cNumberOfValuesPerCacheLine> sValues;
	//!< \brief Values from Data Input Cache to first VirtulChannel in first VCGRA level
	std::array<sc_core::sc_out<result_type_t>, cgra::cNumberOfValuesPerCacheLine> sResults;
	//!< \brief Values from Data Input Cache to first VirtulChannel in first VCGRA level

	//Ctor
	SC_HAS_PROCESS(TB_MMU);
	/*!
	 * \brief General constructor
	 *
	 * \param[in] nameA SystemC module name for TB_MMU instance
	 */
	TB_MMU(const sc_core::sc_module_name& nameA);

	/*!
	 * \brief Thread to test MMU instance
	 *
	 * \details
	 * TBD
	 */
	void stimuli(void);

	/*!
	 * \brief Initialize outputs of TB_MMU
	 */
	virtual void end_of_elaboration() override;

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override
	{ return "Testbench for MMU of a VCGRA.";	}

	/*!
	 * \brief Print name of MMU test bench
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void print(std::ostream& os = std::cout) const override
	{ os << this->name(); return; }

	/*!
	 * \brief Dump MMU test bench information
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override;

	//Dtor
	virtual ~TB_MMU() = default;

private:

	//Deleted constructors
	TB_MMU() = delete;
	TB_MMU(const TB_MMU& src) = delete;
	TB_MMU& operator=(const TB_MMU& src) = delete;
	TB_MMU(TB_MMU&& src) = delete;
	TB_MMU& operator=(TB_MMU&& src) = delete;

}; /* end class TB_MMU */

} /* end namespace cgra */

#endif /* HEADER_TB_MMU_H_ */
