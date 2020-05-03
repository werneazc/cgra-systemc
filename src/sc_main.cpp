/*
 * sc_main.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: andrewerner
 */

#include <cstdlib>
#include <systemc>
#include <array>
#include <fstream>
#include "TopLevel.h"
#include "Testbench_TopLevel.h"
#include "Assembler.hpp"

namespace {

/*!
 * \brief Read image file in pgm-format.
 * 
 * \param[in] file_p    Path to pgm-file
 * \param[in] dest_p    Pointer to target array for image pixel values. Values stored row-major.
 * \param[in] xsize_p   Number of columns of target array
 * \param[in] ysize_p   Number of rows of target array
 * 
 * \return true: Read from PGM successfull
 * \return false: Read from PGM has an error (file path wrong, target array to small)
 */
bool readPgm(const std::string &file_p, uint16_t *dest_p, const uint32_t xsize_p,
             const uint32_t ysize_p) {

  uint32_t width{0};
  uint32_t heigth{0};
  std::string t_lineBuf{};
  std::ifstream t_ifs{file_p};

  //File opens correctly
  if (t_ifs.fail()) {
    return false;
  }

  //Check valid grayscale format.
  std::getline(t_ifs,t_lineBuf);
  if(t_lineBuf.compare("P2"))
  {
      t_ifs.close();
      return false;
  }

  //ignore comment line
  std::getline(t_ifs,t_lineBuf);

  //get image dimensions
  t_ifs >> width >> heigth;
  if(width > xsize_p || heigth > ysize_p)
  {
      t_ifs.close();
      return false;
  }

  //ignore max value line
  std::getline(t_ifs,t_lineBuf);
  std::getline(t_ifs,t_lineBuf);

  uint16_t t_value{0x0000};
  uint32_t row{0};
  uint32_t column{0};
  do {

      t_ifs >> t_value;
      *(dest_p + row * width + column) = t_value;

      ++column;

      if(column >= width)
      {
          column = 0;
          ++row;
      }

  } while (t_ifs.good());

  t_ifs.close();

  return true;
}


bool writePgm(const std::string& file_p, const int16_t* image, const uint32_t size_y, const uint32_t size_x, uint16_t max_p = INT16_MAX)
{
  uint32_t iter_x{0}, iter_y{0};    //image iterator
  int8_t count{0};                  //pgm format limits number of characters per line to 70
  std::ofstream t_ofs{file_p, std::ios_base::out};

  //File opens correctly
  if (t_ofs.fail()) {
      return false;
  }

  //write header of pgm format
  t_ofs << "P2\n\r";                                //gray scale ascii
  t_ofs << "# result after convolution\n\r";        //comment to define picture
  t_ofs << size_x << "\t" << size_y << "\n\r";      //size of the image
  t_ofs << max_p << "\n\r";                     //max value within the picture


  for( iter_y = 0; iter_y < size_y; ++iter_y)
  {

      count = 70;

      for (iter_x = 0; iter_x < size_x; ++iter_x)
      {

          if(--count)
          {
            //   xil_printf("%d\t", (int)*((uint8_t*)(image + iter_y * size_x + iter_x)));
            t_ofs << std::dec << *(image + iter_y * size_x + iter_x) << "\t";
          }
          else
          {
            //   xil_printf("%d\n\r", (int)*((uint8_t*)(image + iter_y * size_x + iter_x)));
              t_ofs << std::dec << *(image + iter_y * size_x + iter_x) << "\n\r";
              count = 70;
          }

      }

      t_ofs << "\n\r";

  }

  t_ofs.close();

  return true;
}

} // namespace

int sc_main(int argc, char* arcv[])
{
    //include TB
    auto tb_toplevel = new cgra::Testbench_TopLevel{"Architecture_TestBench"};

//#############################################################################

    /* 
    This small assembler program load data and configuration from main memory. 
    Afterwards, the preloaded configuration is chosen for the architecture 
    configuration, before the VCGRA is started. The architecture process the data,
    then the results are stored back into the main memory. 
    */
    // std::array<cgra::TopLevel::assembler_type_t, 12> assembly {
    //     "0x00001FC6", //LOADDA 0 0
    //     "0x00321FC9", //LOADPE 50 0
    //     "0x00281FCA", //LOADCH 40 0
    //     "0x00000010", //SLCT_DOC_LINE 0
    //     "0x0000000F", //SLCT_DIC_LINE 0
    //     "0x00000011", //SLCT_PECC_LINE 0
    //     "0x00000012", //SLCT_CHCC_LINE 0
    //     "0x0000000B", //START
    //     "0x00000004", //WAIT_READY
    //     "0x00002010", //SLCT_DOC_LINE 1
    //     "0x00101FC8", //STOREDA 16 0
    //     "0x0000000C"  //FINISH
    // };
	//instantiate modules
	auto toplevel = new cgra::TopLevel{"TopLevel", cgra::assembly.data(), cgra::assembly.size()};

//#############################################################################

	//signals
 	sc_core::sc_clock clk{"clk", 200, sc_core::SC_NS};
 	sc_core::sc_signal<cgra::TopLevel::run_type_t> run{"run", true};
 	sc_core::sc_signal<cgra::TopLevel::reset_type_t> rst{"rst", false};
 	sc_core::sc_signal<cgra::TopLevel::finish_type_t> finish{"finish"};
 	sc_core::sc_signal<cgra::TopLevel::pause_type_t> pause{"pause", false};

//#############################################################################

	//Port bindings

 	//clock
 	toplevel->clk.bind(clk);
    tb_toplevel->clk.bind(clk);

	//control signals
 	toplevel->run.bind(run);
 	toplevel->finish.bind(finish);
 	toplevel->rst.bind(rst);
    toplevel->pause.bind(pause);
 	tb_toplevel->run.bind(run);
 	tb_toplevel->finish.bind(finish);
 	tb_toplevel->rst.bind(rst);
    tb_toplevel->pause.bind(pause);
    
//#############################################################################

	//create and setup trace file;
	auto fp_toplevel = sc_core::sc_create_vcd_trace_file("architecture_test");

	sc_core::sc_trace(fp_toplevel,clk,"clock");
	sc_core::sc_trace(fp_toplevel,run,"run");
	sc_core::sc_trace(fp_toplevel,rst,"rst");
	sc_core::sc_trace(fp_toplevel,pause,"pause");
	sc_core::sc_trace(fp_toplevel,finish,"finish");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.m_currentAssembler, "assembler");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.address, "address");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.place, "place");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.cache_select, "cache_select");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.mmu_start, "mmu_start");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.mmu_ready, "mmu_ready");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.start, "vcgra_start");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.ready, "vcgra_ready");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.write_enable, "mmu_we");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::DATA_INPUT), "we_dataIn");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::DATA_OUTPUT), "we_dataOut");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::CONF_PE), "we_pe_cc");
    sc_core::sc_trace(fp_toplevel,toplevel->we_dmux.outputs.at(cgra::MMU::CACHE_TYPE::CONF_CC), "we_vc_cc");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.ack, "cache_ack");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::DATA_INPUT), "ack_dataIn");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::DATA_OUTPUT), "ack_dataOut");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::CONF_PE), "ack_pe_cc");
    sc_core::sc_trace(fp_toplevel,toplevel->ack_mux.inputs.at(cgra::MMU::CACHE_TYPE::CONF_CC), "ack_vc_cc");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.cache_place, "cache_place");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.data_value_out_stream, "data_cache_outstream");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.data_value_in_stream, "data_cache_instream");
    sc_core::sc_trace(fp_toplevel, toplevel->mmu.conf_cache_stream, "conf_cache_stream");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.dic_select_lines.first, "dic_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.dic_select_lines.second, "dic_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.doc_select_lines.first, "doc_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.doc_select_lines.second, "doc_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.pe_cc_select_lines.first, "pe_cc_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.pe_cc_select_lines.second, "pe_cc_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.ch_cc_select_lines.first, "ch_cc_select_in");
    sc_core::sc_trace(fp_toplevel, toplevel->mu.ch_cc_select_lines.second, "ch_cc_select_out");
    sc_core::sc_trace(fp_toplevel, toplevel->vcgra.ch_config, "vch_configuration");
    sc_core::sc_trace(fp_toplevel, toplevel->vcgra.pe_config, "pe_configuration");

//#############################################################################

  // Initialize configuration and data
  {
    // Small example configuation and input data for testing
    // uint16_t tdataValues[] = {10, 20, 50, 30, 16, 4, 64, 8};
    // toplevel->mmu.write_shared_memory(0,tdataValues,sizeof(tdataValues));
    // uint8_t tPeConfig[] = {0x12, 0x34, 0x86, 0x87, 0x88, 0x58, 0x88, 0x88};
    // toplevel->mmu.write_shared_memory(50, tPeConfig, sizeof(tPeConfig));
    // uint8_t tChConfig[] = {0x05, 0x39, 0x77, 0x01, 0xAB, 0x05, 0x6F, 0x05, 0xAF, 0x00};
    // toplevel->mmu.write_shared_memory(40, tChConfig, sizeof(tChConfig));

    // std::array<uint16_t, 3*3> tcoefficients{0, 0, 0, 0, 1, 0, 0, 0, 0};
    std::array<int16_t, 3*3> tcoefficients{1, 0, -1, 2, 0, -2, 1, 0, -1};
    toplevel->mmu.write_shared_memory(0x170, tcoefficients.data(), tcoefficients.max_size() * sizeof(int16_t));
      
    std::array<uint16_t, 64*64> tdataValues;
    if(!readPgm("./example.pgm", tdataValues.data(), 64, 64))
    {
        return EXIT_FAILURE;
    }
    toplevel->mmu.write_shared_memory(0x200, tdataValues.data(), tdataValues.max_size() * sizeof(uint16_t));
    uint8_t tPeConfig1[] = {0x33, 0x33, 0x01, 0x01, 0x00, 0x10, 0x00, 0x80};
    uint8_t tPeConfig2[] = {0x38, 0x80, 0x01, 0x80, 0x00, 0x10, 0x00, 0x80};
    toplevel->mmu.write_shared_memory(0x00, tPeConfig1, sizeof(tPeConfig1));
    toplevel->mmu.write_shared_memory(0x40, tPeConfig2, sizeof(tPeConfig2));
    uint8_t tChConfig1[] = {0x05, 0x39, 0x77, 0x01, 0xAB,
                            0x05, 0x7F, 0x05, 0xAF, 0xB0};
    uint8_t tChConfig2[] = {0x05, 0x26, 0xE4, 0x01, 0xAF,
                            0x05, 0x6F, 0x05, 0xAF, 0xB0};
    toplevel->mmu.write_shared_memory(0x80, tChConfig1, sizeof(tChConfig1));
    toplevel->mmu.write_shared_memory(0x120, tChConfig2, sizeof(tChConfig2));
  }

  // Run simulation
  sc_core::sc_start(500, sc_core::SC_MS);

  {
      std::array<int16_t, 62*62> t_result;
      t_result.fill(0);

      toplevel->mmu.read_shared_memory<int16_t>(0x2300, t_result.data(), 62*62);

      auto t_max = *(std::max_element(t_result.begin(), t_result.end()));

      writePgm("./outimage.pgm", t_result.data(), 62, 62, t_max);

  }

  //#ifdef DEBUG
  std::ofstream fp_dump{"simulation_dump.log", std::ios_base::out};
  toplevel->dump(fp_dump);
  fp_dump << "Memory Dump" << std::endl;
  toplevel->mmu.dump_memory<int16_t>(0x170, 0x180, sc_dt::SC_DEC, true, fp_dump);
  fp_dump << "\n\n";
  toplevel->mmu.dump_memory<uint16_t>(0x200, 0x400, sc_dt::SC_DEC, true, fp_dump);
  fp_dump << "\n\n";
  toplevel->mmu.dump_memory<int16_t>(0x2300, 0x2500, sc_dt::SC_DEC, true, fp_dump);
  fp_dump.close();
  //#endif //DEBUG

//#############################################################################

	//Finish simulation
	sc_core::sc_close_vcd_trace_file(fp_toplevel);

	return EXIT_SUCCESS;
};
