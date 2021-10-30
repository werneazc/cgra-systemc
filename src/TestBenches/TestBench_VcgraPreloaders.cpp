/**
 * @file Testbench_VcgraPreloader.cpp
 * @author André Werner (andre.werner@b-tu.de)
 * @brief Implementation of testbench for architecture with VCGRA and pre-fetchers
 * @version 0.1
 * @date 2021-01-31
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "TestBench_VcgraPreloaders.hpp"
#include <algorithm>
#include <fstream>

namespace
{
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
bool readPgm(const std::string &file_p, uint16_t *dest_p, const uint32_t xsize_p, const uint32_t ysize_p)
{

    uint32_t width{0};
    uint32_t heigth{0};
    std::string t_lineBuf{};
    std::ifstream t_ifs{file_p};

    // File opens correctly
    if (t_ifs.fail()) {
        return false;
    }

    // Check valid grayscale format.
    std::getline(t_ifs, t_lineBuf);
    if (0 != static_cast<uint32_t>(t_lineBuf.compare("P2"))) {
        t_ifs.close();
        return false;
    }

    // Ignore lines of comments
    while (t_lineBuf.front() == '#' || t_lineBuf.empty()) {
        std::getline(t_ifs, t_lineBuf);
    }

    // get image dimensions
    t_ifs >> width >> heigth;
    if (width > xsize_p || heigth > ysize_p) {
        t_ifs.close();
        return false;
    }

    // ignore max value line
    std::getline(t_ifs, t_lineBuf);
    std::getline(t_ifs, t_lineBuf);

    uint16_t t_value{0x0000};
    uint32_t row{0};
    uint32_t column{0};
    do {

        t_ifs >> t_value;
        *(dest_p + (row * width + column)) = t_value;

        ++column;

        if (column >= width) {
            column = 0;
            ++row;
        }
    } while (t_ifs.good());

    t_ifs.close();

    return true;
}

/**
 * @brief Write result image in PGM format
 *
 * @param[in] file_p    Path of result file in PGM format (overwritten)
 * @param[in] image     Result image to write to file
 * @param[in] size_y    Y-dimension of result image
 * @param[in] size_x    X-dimension of result image
 * @param[in] max_p     Maximum value of result image (normation)
 * @return true     Success
 * @return false    Failure
 */
bool writePgm(const std::string &file_p, const int16_t *image, const uint32_t size_y, const uint32_t size_x,
              uint16_t max_p = INT16_MAX)
{
    uint32_t iter_x{0}; // image iterator x direction
    uint32_t iter_y{0}; // image iterator y direction
    size_t count{size_x};
    std::ofstream t_ofs{file_p, std::ios_base::out};

    // File opens correctly
    if (t_ofs.fail()) {
        return false;
    }

    // write header of pgm format
    t_ofs << "P2\n";                           // gray scale ascii
    t_ofs << "# result after convolution\n";   // comment to define picture
    t_ofs << size_x << "\t" << size_y << "\n"; // size of the image
    t_ofs << max_p << "\n";                    // max value within the picture

    for (iter_y = 0; iter_y < size_y; ++iter_y) {

        for (iter_x = 0; iter_x < size_x; ++iter_x) {

            if (0 > --count) {
                t_ofs << std::dec << *(image + (iter_y * size_x + iter_x)) << "\n";
                count = size_x;
            }
            else {
                t_ofs << std::dec << *(image + (iter_y * size_x + iter_x)) << "\t";
            }
        }
    }

    t_ofs.close();

    return true;
}

} // namespace
namespace cgra
{

Testbench::Testbench(const sc_core::sc_module_name &nameA, std::string imagePathA)
    : sc_core::sc_module(nameA), mImagePath(std::move(imagePathA))
{
    if (mImagePath.empty()) {
        throw std::invalid_argument("Path to input image is empty.");
    };

    if (!readPgm(mImagePath, mInputValues.data(), Testbench::cInputSize, Testbench::cInputSize)) {
        throw std::invalid_argument("Size of input image does not fit to buffer 64x64 pixel.");
    }

    mResultValues.fill(0);
    mSobelxValues.fill(0);
    mSobelyValues.fill(0);

    SC_THREAD(stimuli);
}

std::size_t Testbench::appendPeConfiguration(std::vector<uint8_t> peConfigA)
{
    mPeConfigs.emplace_back(std::move(peConfigA));
    return mPeConfigs.size() - 1;
}

std::size_t Testbench::appendChConfiguration(std::vector<uint8_t> chConfigA)
{
    mChConfigs.emplace_back(std::move(chConfigA));
    return mChConfigs.size() - 1;
}

bool Testbench::writeResultImagetoFile(const std::string &pathA) const
{
    return writePgm(pathA, mResultValues.data(), Testbench::cOutputSize, Testbench::cOutputSize,
                    *std::max_element(mResultValues.cbegin(), mResultValues.cend()));
}

void Testbench::stimuli()
{

    // Time delay for switching to new configuration.
    const sc_core::sc_time changePeConfigDelay(6.4, sc_core::SC_US);
    const sc_core::sc_time changeChConfigDelay(6.5, sc_core::SC_US);

    // Temporary values for output pixel calculation
    cgra::VCGRA::data_output_type_t temp0{};
    cgra::VCGRA::data_output_type_t temp1{};

    /*
     * The delay is approximated from the VHDL implementation. It takes
     * approximately 160 microseconds to calculate one pixel value. The
     * time to transmit all necessary data is around 50%, that is around 80 microseconds.
     * The payload is 160 byte for data and configuration. That means one byte
     * takes around 500ns to transmit. The delay is a multiple of 500ns times the
     * number of bytes to transmit.
     */
    auto calcDelay = [](std::size_t numbytesA) -> sc_core::sc_time {
        constexpr double cDelayPerByte = 500.0 /*ns*/;

        return sc_core::sc_time(numbytesA * cDelayPerByte, sc_core::SC_NS);
    };

    auto t_start = sc_core::sc_time_stamp();
    auto t_startConfig = t_start;

    // Transmit configuration to pre-fetchers
    pe_slct_out.write(1);
    pe_slct_in.write(0);
    for (auto &part : mPeConfigs.at(0)) {

        pe_input_stream.write(part);
        wait(calcDelay(sizeof(uint8_t)));
        pe_write_enable.write(true);
        wait(pe_ack.posedge_event());
        pe_write_enable.write(false);
        wait(pe_ack.negedge_event());
    }

    pe_slct_out.write(0);
    pe_slct_in.write(1);
    for (auto &part : mPeConfigs.at(1)) {

        pe_input_stream.write(part);
        wait(calcDelay(sizeof(uint8_t)));
        pe_write_enable.write(true);
        wait(pe_ack.posedge_event());
        pe_write_enable.write(false);
        wait(pe_ack.negedge_event());
    }

    ch_slct_out.write(1);
    ch_slct_in.write(0);
    for (auto &part : mChConfigs.at(0)) {

        ch_input_stream.write(part);
        wait(calcDelay(sizeof(uint8_t)));
        ch_write_enable.write(true);
        wait(ch_ack.posedge_event());
        ch_write_enable.write(false);
        wait(ch_ack.negedge_event());
    }

    ch_slct_out.write(0);
    ch_slct_in.write(1);
    for (auto &part : mChConfigs.at(1)) {

        ch_input_stream.write(part);
        wait(calcDelay(sizeof(uint8_t)));
        ch_write_enable.write(true);
        wait(ch_ack.posedge_event());
        ch_write_enable.write(false);
        wait(ch_ack.negedge_event());
    }

    auto t_stopConfig = sc_core::sc_time_stamp();
    auto t_startSobelx = t_stopConfig;

    for (size_t yiter = 1; yiter < Testbench::cInputSize - 1; ++yiter) {
        for (size_t xiter = 1; xiter < Testbench::cInputSize - 1; ++xiter) {

            // Select configurations for channels and processing elements
            pe_slct_out.write(0);
            ch_slct_out.write(0);

            // Transmit values for first calculation
            data_inputs[1].write(mInputValues.at((yiter - 1) * cInputSize + (xiter - 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[3].write(mInputValues.at((yiter - 1) * cInputSize + xiter));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[5].write(mInputValues.at((yiter - 1) * cInputSize + (xiter + 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[7].write(mInputValues.at(yiter * cInputSize + (xiter - 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[0].write(sobelx[0]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[2].write(sobelx[1]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[4].write(sobelx[2]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[6].write(sobelx[3]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Start execution and wait for ready signal of VCGRA
            start.write(true);
            sc_core::wait(clk.posedge_event());
            sc_core::wait(clk.posedge_event());
            start.write(false);
            sc_core::wait(ready.posedge_event());

            // Read first temporary value
            temp0 = data_outputs[2].read();
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Transmit values for second calculation
            data_inputs[1].write(mInputValues.at(yiter * cInputSize + xiter));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[3].write(mInputValues.at(yiter * cInputSize + (xiter + 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[5].write(mInputValues.at((yiter + 1) * cInputSize + (xiter - 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[7].write(mInputValues.at((yiter + 1) * cInputSize + xiter));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[0].write(sobelx[4]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[2].write(sobelx[5]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[4].write(sobelx[6]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[6].write(sobelx[7]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Start execution and wait for ready signal of VCGRA
            start.write(true);
            sc_core::wait(clk.posedge_event());
            sc_core::wait(clk.posedge_event());
            start.write(false);
            sc_core::wait(ready.posedge_event());

            // Read second temporary value
            temp1 = data_outputs[2].read();
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Select configurations for channels and processing elements
            pe_slct_out.write(1);
            ch_slct_out.write(1);

            // Transmit values for third calculation
            data_inputs[1].write(mInputValues.at((yiter + 1) * cInputSize + (xiter + 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[2].write(temp0);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[3].write(temp1);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[0].write(sobelx[8]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Start execution and wait for ready signal of VCGRA
            start.write(true);
            sc_core::wait(clk.posedge_event());
            sc_core::wait(clk.posedge_event());
            start.write(false);
            sc_core::wait(ready.posedge_event());

            mSobelxValues.at((yiter)*cOutputSize + (xiter)) = static_cast<int16_t>(data_outputs[2].read().to_int());
            sc_core::wait(calcDelay(sizeof(uint32_t)));
        }
    }

    auto t_stopSobelx = sc_core::sc_time_stamp();
    auto t_startSobely = t_stopSobelx;

    for (size_t yiter = 1; yiter < Testbench::cInputSize - 1; ++yiter) {
        for (size_t xiter = 1; xiter < Testbench::cInputSize - 1; ++xiter) {

            // Select configurations for channels and processing elements
            pe_slct_out.write(0);
            ch_slct_out.write(0);

            // Transmit values for first calculation
            data_inputs[1].write(mInputValues.at((yiter - 1) * cInputSize + (xiter - 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[3].write(mInputValues.at((yiter - 1) * cInputSize + xiter));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[5].write(mInputValues.at((yiter - 1) * cInputSize + (xiter + 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[7].write(mInputValues.at(yiter * cInputSize + (xiter - 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[0].write(sobely[0]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[2].write(sobely[1]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[4].write(sobely[2]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[6].write(sobely[3]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Start execution and wait for ready signal of VCGRA
            start.write(true);
            sc_core::wait(clk.posedge_event());
            sc_core::wait(clk.posedge_event());
            start.write(false);
            sc_core::wait(ready.posedge_event());

            // Read first temporary value
            temp0 = data_outputs[2].read();
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Transmit values for second calculation
            data_inputs[1].write(mInputValues.at(yiter * cInputSize + xiter));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[3].write(mInputValues.at(yiter * cInputSize + (xiter + 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[5].write(mInputValues.at((yiter + 1) * cInputSize + (xiter - 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[7].write(mInputValues.at((yiter + 1) * cInputSize + xiter));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[0].write(sobely[4]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[2].write(sobely[5]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[4].write(sobely[6]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[6].write(sobely[7]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Start execution and wait for ready signal of VCGRA
            start.write(true);
            sc_core::wait(clk.posedge_event());
            sc_core::wait(clk.posedge_event());
            start.write(false);
            sc_core::wait(ready.posedge_event());

            // Read second temporary value
            temp1 = data_outputs[2].read();
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Select configurations for channels and processing elements
            pe_slct_out.write(1);
            ch_slct_out.write(1);

            // Transmit values for third calculation
            data_inputs[1].write(mInputValues.at((yiter + 1) * cInputSize + (xiter + 1)));
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[2].write(temp0);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[3].write(temp1);
            sc_core::wait(calcDelay(sizeof(uint32_t)));
            data_inputs[0].write(sobely[8]);
            sc_core::wait(calcDelay(sizeof(uint32_t)));

            // Start execution and wait for ready signal of VCGRA
            start.write(true);
            sc_core::wait(clk.posedge_event());
            sc_core::wait(clk.posedge_event());
            start.write(false);
            sc_core::wait(ready.posedge_event());

            mSobelyValues.at((yiter)*cOutputSize + (xiter)) = static_cast<int16_t>(data_outputs[2].read().to_int());
            sc_core::wait(calcDelay(sizeof(uint32_t)));
        }
    }

    auto t_stopSobely = sc_core::sc_time_stamp();
    sc_core::sc_stop();

    auto sqrt_sum = [](const auto &a, const auto &b) -> auto
    {
        return std::sqrt(a * a + b * b);
    };

    std::transform(mSobelxValues.cbegin(), mSobelxValues.cend(), mSobelyValues.cbegin(), mResultValues.begin(),
                   sqrt_sum);

    std::cout << "\n\nSimulation timings: "
              << "\n";
    std::cout << "overall: " << (t_stopSobely - t_start).to_string() << "\n";
    std::cout << "Prepare configuration: " << (t_stopConfig - t_startConfig) << "\n";
    std::cout << "sobel x-direction: " << (t_stopSobelx - t_startSobelx) << "\n";
    std::cout << "sobel y-direction: " << (t_stopSobely - t_startSobely) << std::endl;
}

void Testbench::end_of_elaboration()
{
   start.write(false);
   reset.write(true);
   pe_write_enable.write(false);
   pe_slct_out.write(0);
   pe_slct_in.write(0);
   pe_input_stream.write(0x00);
   ch_write_enable.write(false);
   ch_slct_out.write(0);
   ch_slct_in.write(0);
   ch_input_stream.write(0x00);
   for(auto & val : data_inputs) {
       val.write(0x0000);
   }

}

void Testbench::dump(::std::ostream &os) const
{
    os << name() << ": function not implemented";
}

} // namespace cgra