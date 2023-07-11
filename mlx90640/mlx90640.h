#pragma once

#include <cstdint>
#include <cmath>
#include <tuple>

#include "../io/i2cunix.hpp"

namespace MLX90640
{

using std::make_tuple;
using std::tie;
using std::tuple;

enum struct ReadingPattern : uint8_t {
    Interleaved = 0x00,
    Chess = 0x01
};

enum struct AdcResolution : uint8_t {
    _16_Bit = 0x00,
    _17_Bit = 0x01,
    _18_Bit = 0x02,
    _19_Bit = 0x03
};

enum struct RefreshRate : uint8_t {
    _0_5_Hz = 0x00,
    _1_Hz = 0x1,
    _2_Hz = 0x2,
    _4_Hz = 0x3,
    _8_Hz = 0x4,
    _16_Hz = 0x5,
    _32_Hz = 0x6,
    _64_Hz = 0x7
};

class SensorControl
{
    const uint16_t REFRESH_RATE_MASK = 0x380;
    const uint16_t RESOLUTION_CONTROL_MASK = 0xC00;
    const uint16_t READINGG_PATTERN_MASK = 0x1000;
    const uint16_t SELECTED_SUBPAGE_MASK = 0x70;

public:
    SensorControl(uint16_t control_register);
    operator uint16_t() const 
    {
        uint16_t result = 
            ((uint16_t)readingPattern << 12) +
            ((uint16_t)adcResolution << 10) +
            ((uint16_t)refreshRate << 7) +
            (selectedSubpage << 4) + 
            (enableSubpagesRepeat ? 0x0008 : 0x0000 ) +
            (enableDataHold ? 0x0004 : 0x0000) +
            (subpageEnabled ? 0x0001 : 0x0000);

        return result;
    }

    ReadingPattern readingPattern;
    AdcResolution adcResolution;
    RefreshRate refreshRate;
    int8_t selectedSubpage;
    bool subpageEnabled;
    bool enableDataHold;
    bool enableSubpagesRepeat;
};

const int MLX90640ADDR = 0x33;

const int EEPROM_SIZE = 832;
const int RAM_SIZE = 832;
const uint16_t EEPROM_BASE_ADDR = 0x2400;
const uint16_t EEPROM_AlphaPtat_OCC_ADDR = 0x2410;
const uint16_t EEPROM_PIX_OFFSET_ADDR = 0x2411;
const uint16_t EEPROM_PIX_SENSITIVITY_AVERAGE = 0x2421;
const uint16_t EEPROM_GAIN_ADDR = 0x2430;
const uint16_t EEPROM_PTAT_25_ADDR = 0x2431;
const uint16_t EEPROM_KVPTAT_ADDR = 0x2432;
const uint16_t EEPROM_VDD_ADDR = 0x2433;
const uint16_t EEPROM_RESOLUTION_CAL_SCALE = 0x2438;
const uint16_t EEPROM_OFFSET_CP_ADDR = 0x243A;
const uint16_t EEPROM_KSTA_TGC_ADDR = 0x243C;
const uint16_t EEPROM_KSTO_1_2_ADDR = 0x243D;
const uint16_t EEPROM_KSTO_3_4_ADDR = 0x243E;
const uint16_t EEPROM_Step_CT4_CT3_KsToScale = 0x243F;

const uint16_t RAM_BASE_ADDR = 0x0400;

const uint16_t RAM_Ta_Vbe_ADDR = 0x0700;
const uint16_t RAM_CP_SubP0_ADDR = 0x0708;
const uint16_t RAM_CP_SubP1_ADDR = 0x0728;
const uint16_t RAM_GAIN_ADDR = 0x070A;
const uint16_t RAM_VDD_ADDR = 0x072A;
const uint16_t RAM_Ta_PTAT_ADDR = 0x0720;

const uint16_t STATUS_Register_ADDR = 0x8000;

double sqrt4(double x);
int pixelNumber(int i, int j);
int patternChess(int i, int j);

template<int height, int width>
struct TemperatureFrame
{
    double values[height * width];

    double& operator () (int i, int j)
    {
        return values[width * i + j];
    }

    const double& operator () (int i, int j) const
    {
        return values[width * i + j];
    }

};

using TemperatureFrameMLX90640 = TemperatureFrame<24, 32>;

struct MLX90640Measurment
{
    double ambient_temperature;
    TemperatureFrameMLX90640 flir_frame;
};

uint16_t buildUint16HiLo(const uint8_t* addr);

template <typename TI2CDriver>
class MLX90640Sensor
{
    const double emissivity = 1.0;
    const double Vdd0 = 3.3;
    const double Ta0 = 25.0;

    TI2CDriver &i2cDriver;

    uint8_t eepromData[832 * 2];
    uint8_t ramData[832 * 2];

    //This values are initialized by InitConstantsFromEEPROMBuffer
    int16_t eeprom_gain;
    int16_t eeprom_Kvdd;
    int16_t eeprom_Vdd_25;
    int8_t eeprom_TGC;
    double eeprom_KsTa;
    int32_t eeprom_iVptat25;
    double eeprom_dKTptat;
    int16_t eeprom_OffCPSP0;
    int16_t eeprom_OffCPSP1;
    int16_t eeprom_offsetAvg;
    uint16_t eeprom_OCCscaleRow;
    uint16_t eeprom_OCCscaleCol;
    uint16_t eeprom_OCCscaleRem;
    int16_t eeprom_Alphaptat;
    int16_t eeprom_KtaScale1;
    double eeprom_2_pow_KtaScale1;
    int16_t eeprom_KtaScale2;
    uint16_t eeprom_KvScale;
    double eeprom_2_pow_KvScale;
    uint16_t eeprom_alphaRef;
    double eeprom_dKsTo1;
    double eeprom_dKsTo2;
    double eeprom_dKsTo3;
    double eeprom_dKsTo4;
    double eeprom_dKV_PTAT;

public:
    MLX90640Sensor(TI2CDriver &i2cDriver) : i2cDriver(i2cDriver)
    {
        //ReadCalibrationCoefficinets();
        ReadEEPROM();
        InitConstantsFromEEPROMBuffer();
    }

    int getWidth() const { return 32; }
    int getHeight() const { return 24; }
    double readPixelData(int i, int j, double Kgain, double Ta, double Vdd);

    //Control methods
    void setReadingPattern(ReadingPattern reading_pattern);
    void setRefreshRate(RefreshRate refresh_rate);
    void TemperatureFrameMLX90640(
        MLX90640Measurment& measurment);

private:
    uint16_t GetRegisterFromBuffer(
        const uint8_t* buffer,
        uint16_t base_addr,
        uint16_t register_address) const;

    void ReadEEPROM();
    void InitConstantsFromEEPROMBuffer();

    void ReadRAMBuffer();

    double readKGain();
    double readVdd();
    double readTa(double Vdd);
    double readKsTa();

    double getPixelTemperatureFromRamBuffer(int i, int j, double Kgain, double Ta, double Vdd);

    tuple<double, double, double, double> getKsTo();
};

template <typename TI2CDriver>
uint16_t MLX90640Sensor<TI2CDriver>::GetRegisterFromBuffer(
    const uint8_t* buffer,
    uint16_t base_addr,
    uint16_t register_address) const
{
    uint16_t result = buildUint16HiLo(buffer + 2 * (register_address - base_addr));
    return result;
}

template <typename TI2CDriver>
void MLX90640Sensor<TI2CDriver>::ReadEEPROM()
{
    i2cDriver.ReadBuffer(EEPROM_BASE_ADDR, EEPROM_SIZE * 2, eepromData);
}


template <typename TI2CDriver>
void MLX90640Sensor<TI2CDriver>::ReadRAMBuffer()
{
    i2cDriver.ReadBuffer(RAM_BASE_ADDR, RAM_SIZE * 2, ramData);
}


template <typename TI2CDriver>
void MLX90640Sensor<TI2CDriver>::InitConstantsFromEEPROMBuffer()
{
    //Gain calibration
    eeprom_gain = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_GAIN_ADDR);

    //Vdd_25 and Kvdd 
    uint16_t eeprom_x2433 = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_VDD_ADDR);
    eeprom_Kvdd = eeprom_x2433 & 0xFF00;
    eeprom_Kvdd = eeprom_Kvdd >> 8;
    eeprom_Kvdd = eeprom_Kvdd > 127 ? (int16_t)eeprom_Kvdd - 256 : eeprom_Kvdd;
    eeprom_Kvdd *= 32;
    eeprom_Vdd_25 = ((eeprom_x2433 & 0x00FF) - 256) * 32 - 8192;

    //KsTa
    uint16_t uKsTaEE = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_KSTA_TGC_ADDR);

    uKsTaEE = (uKsTaEE & 0xFF00) >> 8;
    int16_t KsTaEE = uKsTaEE;
    if (KsTaEE > 127)
    {
        KsTaEE = KsTaEE - 256;
    }

    eeprom_KsTa = (double)KsTaEE / 8192.0;

    //eeprom_dKTptat
    uint16_t eeprom_KV_PTAT = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_KVPTAT_ADDR);
    uint16_t uKV_PTAT = (eeprom_KV_PTAT & 0xFC00) >> 10;
    int8_t iKV_PTAT;
    if (uKV_PTAT > 31)
    {
        iKV_PTAT = (int)uKV_PTAT - 64;
    }
    else
    {
        iKV_PTAT = (int)uKV_PTAT;
    }

    eeprom_dKV_PTAT = (double)iKV_PTAT / 4096.0;

    uint16_t uKTptat = eeprom_KV_PTAT & 0x03FF;
    int16_t iKTptat;
    if (uKTptat > 511)
    {
        iKTptat = (int16_t)uKTptat - 1024;
    }
    else
    {
        iKTptat = (int16_t)uKTptat;
    }

    eeprom_dKTptat = (double)iKTptat / 8.0;

    //eeprom_iVptat25
    uint16_t eeprom_PTAT_25 = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_PTAT_25_ADDR);
    if (eeprom_PTAT_25 > 32767)
    {
        eeprom_iVptat25 = (int32_t)eeprom_PTAT_25 - 65536;
    }
    else
    {
        eeprom_iVptat25 = (int32_t)eeprom_PTAT_25;
    }

    //Offsets
    uint16_t eeprom_Offset_CP = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_OFFSET_CP_ADDR);
    uint16_t uOffCPSP0 = eeprom_Offset_CP & 0x03FF;
    eeprom_OffCPSP0 = uOffCPSP0;

    if (eeprom_OffCPSP0 > 511)
    {
        eeprom_OffCPSP0 = eeprom_OffCPSP0 - 1024;
    }
    uint16_t uOffCPSP1d = (eeprom_Offset_CP & 0xFC00) >> 10;
    int16_t OffCPSP1d = uOffCPSP1d;
    if (OffCPSP1d > 31)
    {
        OffCPSP1d = OffCPSP1d - 64;
    }
    eeprom_OffCPSP1 = OffCPSP1d + eeprom_OffCPSP0;
    eeprom_offsetAvg = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_PIX_OFFSET_ADDR);

    uint16_t scaleVal = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_AlphaPtat_OCC_ADDR);
    eeprom_OCCscaleRow = (scaleVal & 0x0f00) >> 8;
    eeprom_OCCscaleCol = (scaleVal & 0x00F0) >> 4;
    eeprom_OCCscaleRem = (scaleVal & 0x000F);

    uint16_t AlphaPtatEE = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_AlphaPtat_OCC_ADDR) & 0xF000) / 4096;
    eeprom_Alphaptat = (AlphaPtatEE / 4) + 8;

    eeprom_KtaScale1 =
        ((GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_RESOLUTION_CAL_SCALE) & 0x00F0) >> 4) + 8;
    eeprom_2_pow_KtaScale1 = pow(2, eeprom_KtaScale1);

    eeprom_KtaScale2 =
        (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_RESOLUTION_CAL_SCALE) & 0x000F);
        
    eeprom_KvScale = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_RESOLUTION_CAL_SCALE) & 0x0F00) >> 8;
    eeprom_2_pow_KvScale = pow(2, eeprom_KvScale);


    uint16_t EE1 = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_KSTO_1_2_ADDR);
    uint16_t EE2 = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_KSTO_3_4_ADDR);

    uint16_t uKsTo1 = EE1 & 0x00FF;
    uint16_t uKsTo3 = EE2 & 0x00FF;
    uint16_t uKsTo2 = (EE1 & 0xFF00) >> 8;
    uint16_t uKsTo4 = (EE2 & 0xFF00) >> 8;

    int16_t KsTo1 = uKsTo1;
    int16_t KsTo3 = uKsTo3;
    int16_t KsTo2 = uKsTo2;
    int16_t KsTo4 = uKsTo4;

    if (KsTo1 > 127)
        KsTo1 = KsTo1 - 256;
    if (KsTo2 > 127)
        KsTo2 = KsTo2 - 256;
    if (KsTo3 > 127)
        KsTo3 = KsTo3 - 256;
    if (KsTo4 > 127)
        KsTo4 = KsTo4 - 256;

    uint16_t KsToScale = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_Step_CT4_CT3_KsToScale) & 0x000F) + 8;

    eeprom_dKsTo1 = (double)KsTo1 / pow(2, KsToScale);
    eeprom_dKsTo2 = (double)KsTo2 / pow(2, KsToScale);
    eeprom_dKsTo3 = (double)KsTo3 / pow(2, KsToScale);
    eeprom_dKsTo4 = (double)KsTo4 / pow(2, KsToScale);

    uint16_t uTGC = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_KSTA_TGC_ADDR);
    uTGC = uTGC & 0x00FF;
    if (uTGC > 127)
    {
        eeprom_TGC = (int)uTGC - 256;
    }
    else
    {
        eeprom_TGC = uTGC;
    }

    eeprom_alphaRef = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, EEPROM_PIX_SENSITIVITY_AVERAGE);
}

template <typename TI2CDriver>
double MLX90640Sensor<TI2CDriver>::readKGain()
{
    int16_t ram_gain = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAM_GAIN_ADDR);
    return (double)eeprom_gain / ram_gain;
}

template <typename TI2CDriver>
double MLX90640Sensor<TI2CDriver>::readVdd()
{
    int16_t VddRAM = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAM_VDD_ADDR);
    double Vdd = ((double)VddRAM - (double)eeprom_Vdd_25) / eeprom_Kvdd + Vdd0;
    return Vdd;
}

template <typename TI2CDriver>
double MLX90640Sensor<TI2CDriver>::readTa(double Vdd)
{
    int16_t Vptat = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAM_Ta_PTAT_ADDR);
    int16_t Vbe = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAM_Ta_Vbe_ADDR);
    double Vptatart = ((double)Vptat / (double)(Vptat * eeprom_Alphaptat + Vbe)) * 262144.0;
    double deltaV = Vdd - Vdd0;
    double Ta = (Vptatart / (1.0 + eeprom_dKV_PTAT * deltaV) - (double)eeprom_iVptat25) / eeprom_dKTptat + Ta0;

    return Ta;
}

template <typename TI2CDriver>
double MLX90640Sensor<TI2CDriver>::readPixelData(
    int i,
    int j,
    double Kgain,
    double Ta,
    double Vdd)
{
    uint16_t rowAdd = 0x2412 + (i >> 2);
    uint16_t colAdd = 0x2418 + (j >> 2);
    uint16_t rowMask = 0xF << (4 * (i % 4));
    uint16_t colMask = 0xF << (4 * (j % 4));
    uint16_t OffsetPixAdd = 0x243F + (i * 32) + j;
    uint16_t OffsetPixVal = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, OffsetPixAdd);
    uint16_t uOffsetPix = (OffsetPixVal & 0xFC00) / 1024;
    int16_t OffsetPix = uOffsetPix;
    if (OffsetPix > 31)
    {
        OffsetPix = OffsetPix - 64;
    }

    uint16_t uOCCRow = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, rowAdd) & rowMask) >> (4 * (i % 4));
    int16_t OCCRow = uOCCRow;
    if (OCCRow > 7)
    {
        OCCRow = OCCRow - 16;
    }

    uint16_t uOCCCol = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, colAdd) & colMask) >> (4 * (j % 4));
    int16_t OCCCol = uOCCCol;
    if (OCCCol > 7)
    {
        OCCCol = OCCCol - 16;
    }

    int16_t pixOffset =
        eeprom_offsetAvg +
        OCCRow * pow(2, eeprom_OCCscaleRow) +
        OCCCol * pow(2, eeprom_OCCscaleCol) +
        OffsetPix * pow(2, eeprom_OCCscaleRem);

    uint16_t uKtaEE = (OffsetPixVal & 0x000E) >> 1;
    int16_t KtaEE = uKtaEE > 3 ? uKtaEE - 7 : uKtaEE;

    bool colEven = ((j % 2) == 1); //O-based counting
    bool rowEven = ((i % 2) == 1);
    bool rowOdd = !rowEven;
    bool colOdd = !colEven;
    uint16_t KtaAvAddr = 0x2436 + (colEven ? 1 : 0);
    uint16_t KtaAvMask = 0xFF00 >> (8 * rowEven);

    //!!!Implementation defined, 2-complementary representation expected (which is true for most systems)
    uint16_t uKtaRC = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, KtaAvAddr) & KtaAvMask);
    uKtaRC >>= 8 * (rowOdd ? 1 : 0);

    int16_t KtaRC;
    KtaRC = uKtaRC;
    if (uKtaRC > 127)
    {
        KtaRC = uKtaRC - 256;
    }

    double Kta = (KtaRC + (KtaEE << eeprom_KtaScale2)) / eeprom_2_pow_KtaScale1;

    int8_t shiftNum = (rowOdd * 4) + (colOdd * 8);
    uint16_t KvMask = 0x000F << shiftNum;
    uint16_t uKv = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, 0x2434) & KvMask;
    uKv >>= shiftNum;
    int8_t Kv = uKv;
    if (Kv > 7)
    {
        Kv = Kv - 16;
    }

    double d_Kv = Kv / eeprom_2_pow_KvScale;

    uint16_t RAMaddr = 0x400 + (i * 32) + j;
    int16_t RAM = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAMaddr);

    double pixGain = RAM * Kgain;
    double pixOs = pixGain - pixOffset * (1 + Kta * (Ta - Ta0) * (1 + d_Kv * (Vdd - Vdd0)));

    return pixOs;
}

template <typename TI2CDriver>
double MLX90640Sensor<TI2CDriver>::getPixelTemperatureFromRamBuffer(
    int i,
    int j,
    double Kgain,
    double Ta,
    double Vdd)
{
    double pixOs = readPixelData(i, j, Kgain, Ta, Vdd);
    int16_t i_pixGainCPSP0 = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAM_CP_SubP0_ADDR);
    int16_t i_pixGainCPSP1 = GetRegisterFromBuffer(ramData, RAM_BASE_ADDR, RAM_CP_SubP1_ADDR);

    double pixGainCPSP0 = i_pixGainCPSP0 * Kgain;
    double pixGainCPSP1 = i_pixGainCPSP1 * Kgain;

    uint16_t KvtaCPEEVal = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, 0x243B);

    int16_t KtaCPEE = KvtaCPEEVal & 0x00FF;
    if (KtaCPEE > 127)
    {
        KtaCPEE = KtaCPEE - 256;
    }

    uint16_t KvCPEE = (KvtaCPEEVal & 0xFF00) >> 8;
    double KvCP = KvCPEE / eeprom_2_pow_KvScale;
    double KtaCP = KtaCPEE / eeprom_2_pow_KtaScale1;

    double b = (1.0 + KtaCP * (Ta - Ta0)) * (1 + KvCP * (Vdd - Vdd0));

    double pixOSCPSP0 = pixGainCPSP0 - eeprom_OffCPSP0 * b;
    double pixOSCPSP1 = pixGainCPSP1 - eeprom_OffCPSP1 * b;

    int pattern = patternChess(i, j);

    double VIREmcomp = pixOs / emissivity;
    double VIRcomp = VIREmcomp - eeprom_TGC * ((1 - pattern) * pixOSCPSP0 + pattern * pixOSCPSP1);

    uint16_t reg2439val = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, 0x2439);
    uint16_t reg2420val = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, 0x2420);

    uint8_t alphaScaleCP = ((reg2420val & 0xF000) >> 12) + 27;
    uint8_t CPP1P0ratio = (reg2439val & 0xFC00) >> 10;
    if (CPP1P0ratio > 31)
    {
        CPP1P0ratio = CPP1P0ratio - 64;
    }

    uint16_t alphaScale = ((reg2420val & 0xF000) >> 12) + 30;

    uint16_t rowAdd = 0x2422 + (i / 4);
    uint16_t colAdd = 0x2428 + (j / 4);
    uint16_t rowMask = 0xF << (4 * (i % 4));
    uint16_t colMask = 0xF << (4 * (j % 4));

    uint16_t uACCRow = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, rowAdd);
    uACCRow = uACCRow & rowMask;
    uACCRow >>= (4 * (i % 4));
    int16_t ACCRow = uACCRow;
    if (ACCRow > 7)
    {
        ACCRow = ACCRow - 16;
    }

    uint16_t uACCCol = GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, colAdd);
    uACCCol = uACCCol & colMask;
    uACCCol >>= (4 * (j % 4));
    int16_t ACCCol = uACCCol;
    if (ACCCol > 7)
    {
        ACCCol = ACCCol - 16;
    }

    uint16_t ACCScaleRow = (reg2420val & 0x0F00) >> 8;
    uint16_t ACCScaleCol = (reg2420val & 0x00F0) >> 4;
    uint16_t ACCScaleRem = (reg2420val & 0x000F);

    uint16_t alphaPixel = (GetRegisterFromBuffer(eepromData, EEPROM_BASE_ADDR, 0x2420 + pixelNumber(i, j)) &
                           0x03f0) >>
                          4;

    double alpha = double((eeprom_alphaRef + (ACCRow << ACCScaleRow) + (ACCCol << ACCScaleCol) + (alphaPixel << ACCScaleRem))) / double(pow(2, alphaScale));

    double alphaCPSP0 = (reg2439val & 0x03ff) / double(pow(2, alphaScaleCP));
    double alphaCPSP1 = alphaCPSP0 * (1.0 + CPP1P0ratio / 128.0);

    double alphacomp = alpha -
                       this->eeprom_TGC * ((1 - pattern) * alphaCPSP0 + pattern * alphaCPSP1) * (1 + this->eeprom_KsTa * (Ta - this->Ta0));

    double Tak4 = ::pow(Ta + 273.15, 4);
    double Trk4 = ::pow(Ta - 8 + 273.15, 4);
    double Tar = Trk4 - (Trk4 - Tak4) / this->emissivity;
    double Sx = eeprom_dKsTo2 * sqrt4(::pow(alphacomp, 3) * VIRcomp + ::pow(alphacomp, 4) * Tar);
    double sqrt4arg = (VIRcomp / (alphacomp * (1 - eeprom_dKsTo2 * 273.15) + Sx)) + Tar;

    double result = sqrt4(sqrt4arg) - 273.15;
    return result;
}

template <typename TI2CDriver>
void MLX90640Sensor<TI2CDriver>::TemperatureFrameMLX90640(
    MLX90640Measurment& measurment) 
{
    ReadRAMBuffer();

    double Kgain = readKGain();
    double Vdd = readVdd();
    double Ta = readTa(Vdd);

    measurment.ambient_temperature = Ta;
    for (int i = 0; i < getHeight(); ++i)
    {
        for (int j = 0; j < getWidth(); ++j)
        {
            measurment.flir_frame(i, j) = getPixelTemperatureFromRamBuffer(i, j, Kgain, Ta, Vdd);
        }
    }

}

} //namespace MLX90640