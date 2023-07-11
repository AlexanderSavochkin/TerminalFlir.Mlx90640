#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include "../mlx90640/mlx90640.h"
#include "termutils.h"


using std::cerr;
using std::endl;

using std::string;
using std::unordered_map;
using MLX90640::MLX90640Measurment;

struct RGB
{
    unsigned int r;
    unsigned int g;
    unsigned int b;
};

class Palette {
protected:
    Palette() {};
public:
    virtual const string& getName() const = 0;

    /*expects normalized temperature between 0 and 1*/
    virtual RGB getRGBColor(
        double relative_temperature) const = 0;
};

class WhiteHotPalette : public Palette  {
    const string name = "WHOT"; 
public:
    const string& getName() const {
        return name;
    };

    /*expects normalized temperature between 0 and 1*/
    RGB getRGBColor(double relative_temperature) const;
};

class IronPalette : public Palette  {
    const string name = "Iron";
public:
    const string& getName() const {
        return name;
    };

    /*expects normalized temperature between 0 and 1*/
    RGB getRGBColor(double relative_temperature) const;
};


class PaletteFactory
{
    static const WhiteHotPalette whiteHotPalette;
    static const IronPalette ironPalette;

    static const unordered_map<string, const Palette*> palettes;
public:
    static const Palette& getPalette(const string& name);
};

class RawTerminal
{
    uint8_t first_row;
    uint8_t first_col;
    uint8_t rows;
    uint8_t cols;
public:
    RawTerminal() : first_row(0), first_col(0), rows(24), cols(32)  {
        terminal_enable_raw_mode();
        clear_screen();
    }

    ~RawTerminal() {
        terminal_disable_raw_mode();
    }

    void SetOrigin() {
    }

    void draw_image(
        const MLX90640Measurment& measurment,
        const Palette& palette,
        double lowtemp,
        double hitemp);
};