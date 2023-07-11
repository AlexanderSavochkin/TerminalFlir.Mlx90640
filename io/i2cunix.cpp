#include "i2cunix.hpp"

void I2CRegisterDeviceDriver::ReadBuffer(
        uint16_t registerAddress,
        uint16_t length,
        uint8_t* buffer)
{
    uint8_t outbuff[2];
    i2c_msg msgs[2];
    i2c_rdwr_ioctl_data msgset[1];

    outbuff[0] = registerAddress >> 8;
    outbuff[1] = registerAddress & 0xFF;

    msgs[0].addr = deviceAddress;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = outbuff;

    msgs[1].addr = deviceAddress;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len = length;
    msgs[1].buf = buffer;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 2;

    if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0) {
        int err = errno;
        printf("ioctl failed, fd = %d, err = %d", i2c_fd, err);
        exit(-1);
    }
}

uint16_t I2CRegisterDeviceDriver::Read16BitRegister(uint16_t registerAddress)
{
    uint8_t inbuff[2];
    ReadBuffer(registerAddress, 2, inbuff);
    uint16_t register_value = (inbuff[0] << 8) + inbuff[1];
    return register_value;
}

void I2CRegisterDeviceDriver::Write16BitRegister(uint16_t registerAddress, uint16_t value)
{
    uint8_t outbuff[4];
    i2c_msg msgs[1];
    i2c_rdwr_ioctl_data msgset[1];

    outbuff[0] = registerAddress >> 8;
    outbuff[1] = registerAddress & 0xFF;
    outbuff[2] = value >> 8;
    outbuff[3] = value & 0xFF;

    msgs[0].addr = deviceAddress;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = outbuff;    

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;

    if (ioctl(i2c_fd, I2C_RDWR, &msgset) < 0) {
        int err = errno;
        printf("ioctl failed, fd = %d, err = %d", i2c_fd, err);
        exit(-1);
    }    
}
