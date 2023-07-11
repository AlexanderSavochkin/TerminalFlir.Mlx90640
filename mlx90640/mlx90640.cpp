#include "mlx90640.h"

#include <cmath>

namespace MLX90640
{

SensorControl::SensorControl(uint16_t control_register) {
    refreshRate = (RefreshRate)((control_register & REFRESH_RATE_MASK) >> 7);
    adcResolution = (AdcResolution)((control_register & RESOLUTION_CONTROL_MASK) >> 10);
    readingPattern = control_register & READINGG_PATTERN_MASK == 0 ? 
        ReadingPattern::Interleaved : ReadingPattern::Chess;
    selectedSubpage = (control_register & SELECTED_SUBPAGE_MASK) >> 4;

    subpageEnabled = (control_register & 0x0001) != 0;

    enableDataHold = (control_register & 0x0004) != 0;

    enableSubpagesRepeat = (control_register & 0x0008) != 0;
}

uint16_t buildUint16HiLo(const uint8_t* addr)
{
    uint16_t register_value = (addr[0] << 8) + addr[1];
    return register_value;
}

double sqrt4(double x) {
    return sqrt(sqrt(x));
}

int pixelNumber(int i, int j)
{
    return i * 32 + j;
}

int patternChess(int i, int j)
{
    int pixnum = MLX90640::pixelNumber(i, j);
    int a = pixnum / 32;
    int b = (pixnum / 32) / 2;
    return a - b*2;
}

} //namespace MLX90640