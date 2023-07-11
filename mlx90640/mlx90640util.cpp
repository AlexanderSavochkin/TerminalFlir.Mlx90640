#include "mlx90640util.h"

namespace MLX90640Util {

unordered_map<ReadingPattern, string, ehash<ReadingPattern>> ReadingPatternStrings =
{
    {ReadingPattern::Interleaved,"Interleaved"},
    {ReadingPattern::Chess, "Chess"}
};

unordered_map<AdcResolution, string, ehash<AdcResolution>> AdcResolutionStrings = 
{
    {AdcResolution::_16_Bit, "16 Bit"},
    {AdcResolution::_17_Bit, "17 Bit"},
    {AdcResolution::_18_Bit, "18 Bit"},
    {AdcResolution::_19_Bit, "19 Bit"}    
};

unordered_map<RefreshRate, string, ehash<RefreshRate>> RfreshRateStrings = 
{
    {RefreshRate::_0_5_Hz, "0.5 Hz"},
    {RefreshRate::_1_Hz, "1 Hz"},
    {RefreshRate::_2_Hz, "2 Hz"},
    {RefreshRate::_4_Hz, "4 Hz"},
    {RefreshRate::_8_Hz, "8 Hz"},
    {RefreshRate::_16_Hz, "16 Hz"},
    {RefreshRate::_32_Hz, "32 Hz"},
    {RefreshRate::_64_Hz, "64 Hz"}
};

}