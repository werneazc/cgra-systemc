/*
 * MU_TB.h
 *
 *  Created on: Dec 16, 2018
 *      Author: andrewerner
 */

#ifndef HEADER_MU_TB_H_
#define HEADER_MU_TB_H_

#include <systemc.h>
#include <cstdint>
#include <iostream>
#include "Management_Unit.h"

namespace cgra {

class TB_MU : public sc_core::sc_module
{
public:
	//Public type definitions
	//=======================
	typedef std::pair<sc_core::sc_in<cgra::data_input_cache_type_t::select_lines_type_t>, sc_core::sc_in<cgra::data_input_cache_type_t::select_lines_type_t>> dic_select_type_t;
	//!< \brief Select port type for select lines (select_in, select_out) of data input cache.
	typedef std::pair<sc_core::sc_in<cgra::data_output_cache_type_t::select_lines_type_t>, sc_core::sc_in<cgra::data_output_cache_type_t::select_lines_type_t>> doc_select_type_t;
	//!< \brief Select port type for select lines (select_in, select_out) of data output cache.
	typedef std::pair<sc_core::sc_in<cgra::pe_config_cache_type_t::select_type_t>, sc_core::sc_in<cgra::pe_config_cache_type_t::select_type_t>> pe_cc_select_type_t;
	//!< \brief Select port type for select lines (select_in, select_out) of Processing_Element ConfigurationCache
	typedef std::pair<sc_core::sc_in<cgra::ch_config_cache_type_t::select_type_t>, sc_core::sc_in<cgra::ch_config_cache_type_t::select_type_t>> ch_cc_select_type_t;
	//!< \brief Select port type for select lines (select_in, select_out) of VirtualChannel ConfigurationCache


	//Entity Ports
	//============
	sc_core::sc_out<ManagementUnit::run_type_t> run{"run"};
	//!< \brief TB_MU run port to start execution
	sc_core::sc_in<ManagementUnit::finish_type_t> finish{"finish"};
	//!< \brief TB_MU finish port indicate finish state
	sc_core::sc_out<ManagementUnit::pause_type_t> pause{"pause"};
	//!< \brief TB_MU pause port to pause execution
	sc_core::sc_out<reset_type_t> rst{"reset"};
	//!< \brief TB_MU reset port to restart execution
	sc_core::sc_out<ready_type_t> ready{"ready"};
	//!< \brief (Synchronization) Ready signal port of VCGRA
	sc_core::sc_in<start_type_t> start{"start"};
	//!< \brief (Synchronization) Start signal port for VCGRA
	sc_core::sc_in<CommandInterpreter::address_type_t> address{"Address"};
	//!< \brief Shared memory (start) address of data (sequence)
	sc_core::sc_in<CommandInterpreter::place_type_t> place{"Place"};
	//!< \brief Place in cache line to store data from or load data to
	sc_core::sc_in<MMU::cache_select_type_t> cache_select{"Cache_Select"};
	//!< \brief Selection of source/target cache
	sc_core::sc_out<MMU::ready_type_t> mmu_ready{"MMU_Ready"};
	//!< \brief If false, shared memory ready to process new command, else shared memory is processing.
	sc_core::sc_in<MMU::start_type_t> mmu_start{"MMU_start"};
	//!< \brief If false, shared memory ready to process new command, else shared memory is processing.
	dic_select_type_t dic_select_lines;
	//!< \brief Select line outputs (select_in, select_out) for DataInCache
	doc_select_type_t doc_select_lines;
	//!< \brief Select line outputs (select_in, select_out) for DataOutCache
	pe_cc_select_type_t pe_cc_select_lines;
	//!< \brief Select line outputs (select_in, select_out) for Processing_Element ConfigurationCache
	ch_cc_select_type_t ch_cc_select_lines;
	//!< \brief Select line outputs (select_in, select_out) for Processing_Element ConfigurationCache

	//Methods
	//=======

	SC_HAS_PROCESS(TB_MU);
	/*!
	 * \brief Default constructor
	 *
	 * \param[in] nameA Unique model name of TB_MU instance.
	 */
	TB_MU(const sc_core::sc_module_name& nameA);

	/*!
	 * \brief Initialize output signals of module
	 */
	virtual void end_of_elaboration() override;

	/*!
	 * \brief Print kind of SystemC module
	 */
	virtual const char* kind() const override
	{ return "TB Management Unit";	}

	/*!
	 * \brief Print name of TB for ManagementUnit
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void print(std::ostream& os = std::cout) const override
	{ os << this->name(); return; }

	/*!
	 * Dump information of TB for ManagementUnit
	 *
	 * \param[out] os Define used outstream [default: std::cout]
	 */
	virtual void dump(std::ostream& os = std::cout) const override;


	/*!
	 * \brief Destructor
	 */
	~TB_MU() = default;

	/*!
	 * \brief Stimuli for ManagementUnit
	 */
	void stimuli(void);

	/*!
	 * \brief React to start signal for VCGRA
	 */
	void simulate_vcgra(void);

	/*!
	 * \brief React to start signal for MMU
	 */
	void simulate_mmu(void);


private:
	//forbidden constructor
	TB_MU() = delete;
	TB_MU(const TB_MU &source) = delete;			//!< \brief because sc_module could not be copied
	TB_MU(TB_MU &&source) = delete;					//!< \brief because move not implemented for sc_module
	TB_MU &operator=(const TB_MU &source) = delete;	//!< \brief because sc_module could not be copied
	TB_MU &operator=(TB_MU &&source) = delete;		//!< \brief because move not implemented for sc_module

}; /* End namespace cgra */

} /* End namespace cgra */

#endif /* HEADER_MU_TB_H_ */

