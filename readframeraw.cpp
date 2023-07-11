#include <cstdio>
#include "io/i2cunix.hpp"
#include "mlx90640/mlx90640.h"
#include "mlx90640/mlx90640util.h"

using namespace MLX90640;

const char * const i2cfilename = "/dev/i2c-1";
uint8_t frameData[832];
uint16_t control_register;
uint16_t status_register;

void hexdump(void *ptr, int buflen) {
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i = 0; i < buflen; i += 16) {
    printf("%06x: ", i);
    for (j = 0; j < 16; j++) 
      if (i + j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j = 0; j < 16; j++) 
      if (i + j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
}

int main()
{
puts("A");
    I2CRegisterDeviceDriver i2c_driver(i2cfilename, MLX90640ADDR);

    int16_t status_register;
    uint16_t data_ready = 0;
    while (data_ready == 0)
    {
        status_register = i2c_driver.Read16BitRegister(STATUS_Register_ADDR);
        data_ready = status_register & 0x0008;
    }
puts("B");
    uint8_t retry_count;
    for (
        retry_count = 0;
        data_ready != 0 && retry_count < 5;
        ++retry_count)
    {
        i2c_driver.Write16BitRegister(0x8000, 0x0030);
        i2c_driver.ReadBuffer(0x400, 832, frameData);

        status_register = i2c_driver.Read16BitRegister(STATUS_Register_ADDR);
        data_ready = status_register & 0x0008;
    }

    control_register = i2c_driver.Read16BitRegister(0x800D);
printf("Hey!\n");
    hexdump(frameData, 832);
    printf("Control register: %hx\n", control_register);
    printf("Status register: %hx\n", status_register);

    uint16_t t = i2c_driver.Read16BitRegister(0x400);
    printf("@0x400: %hx\n", t);

    MLX90640::SensorControl sensorControl(control_register);
    printf("Refresh rate: %s\n", 
      MLX90640Util::RfreshRateStrings[sensorControl.refreshRate].c_str());

    printf("ADC resolution: %s\n", 
      MLX90640Util::AdcResolutionStrings[sensorControl.adcResolution].c_str());

    return 0;
}