#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

using std::ostream;
using std::ostringstream;
using std::stod;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::invalid_argument;

#include "term/rawterminal.hpp"
#include "mlx90640/mlx90640.h"

using MLX90640::MLX90640ADDR;
using MLX90640::MLX90640Sensor;
using MLX90640::MLX90640Measurment;

const string CMDL_KEY_PALETTE = "--palette";
const string CMDL_KEY_LOW_TEMP = "--low-temperature";
const string CMDL_KEY_HIGH_TEMP = "--high-temperature";
const string CMDL_KEY_REFRESH_RATE = "--refresh-rate";


const char * const i2cfilename = "/dev/i2c-1";
double lowtemp = 19.0;
double hitemp = 42.0;
string palette_name = "iron"; //"whot";

/*** input ***/
void process_key_press(char c)  {
    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}


int main(int argc, char** argv)
{
    I2CRegisterDeviceDriver i2c_driver(i2cfilename, MLX90640ADDR);
    MLX90640Sensor<I2CRegisterDeviceDriver> sensor(i2c_driver);
    RawTerminal terminal;

    try {
        for (int iarg = 1; iarg < argc; ++iarg) {
            if (argv[iarg] == CMDL_KEY_PALETTE) {
                if(iarg >= argc) {
                    ostringstream ostr;
                    ostr << "paletter name expecter after" << CMDL_KEY_PALETTE;
                    throw invalid_argument(ostr.str());
                }

                palette_name = argv[iarg + 1];
                cout << "Setting palette_name = " << palette_name << endl;
                ++iarg;
            } else if ((argv[iarg]) == CMDL_KEY_LOW_TEMP) {
                if(iarg >= argc) {
                    ostringstream ostr;
                    ostr << "temperature value expecter after" << CMDL_KEY_HIGH_TEMP;
                    throw invalid_argument(ostr.str());
                }
                string tmp(argv[iarg + 1]);
                lowtemp = stod(tmp);
                ++iarg;
            } else if ((argv[iarg]) == CMDL_KEY_HIGH_TEMP) {
                if(iarg >= argc) {
                    ostringstream ostr;
                    ostr << "temperature value expecter after" << CMDL_KEY_HIGH_TEMP;
                    throw invalid_argument(ostr.str());
                }
                string tmp(argv[iarg + 1]);
                hitemp = stod(tmp);
                ++iarg;
            } else {
                ostringstream ostr;
                ostr << "unknown key: " << argv[iarg];
                throw invalid_argument(ostr.str());
            }


        }
    }
    catch (invalid_argument e) {
    }

    const Palette& palette = PaletteFactory::getPalette(palette_name);

    int continue_work = 1;
    int n_key_read;
    char c;

    const uint16_t data_ready_mask = 0x0008;

    while (continue_work) {
        int16_t status_register;
        uint16_t data_ready = 0;

        while (data_ready == 0)
        {
            status_register = i2c_driver.Read16BitRegister(MLX90640::STATUS_Register_ADDR);
            data_ready = status_register & data_ready_mask;
        }

        MLX90640Measurment measurment;
        sensor.TemperatureFrameMLX90640(measurment);

        status_register = status_register & (~data_ready_mask);
        i2c_driver.Write16BitRegister(MLX90640::STATUS_Register_ADDR, status_register);

        terminal.draw_image(measurment, palette, lowtemp, hitemp);

        n_key_read = read(STDIN_FILENO, &c, 1);
        if (n_key_read > 0) {
            process_key_press(c);
        }
    }

    return 0;
}