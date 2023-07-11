#pragma once

#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

class I2CRegisterDeviceDriver
{
    int i2c_fd;
    int deviceAddress;
public:
    I2CRegisterDeviceDriver(const char *filename, int deviceAddress) :
        deviceAddress(deviceAddress) 
    {
        i2c_fd = ::open(filename, O_RDWR);
        if (i2c_fd < 0) {
            int err = errno;
            printf("Failed to open i2c: fn = %s, fd = %d, err = %d\n", filename, i2c_fd, err);
            exit(-1);            
        }
    }

    ~I2CRegisterDeviceDriver()
    {
        ::close(i2c_fd);
    }

    uint16_t Read16BitRegister(uint16_t registerAddress);
    void Write16BitRegister(uint16_t registerAddress, uint16_t value);
    void ReadBuffer(
        uint16_t registerAddress,
        uint16_t length,
        uint8_t* buffer);
};
