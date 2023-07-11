#pragma once

#include <vector>
#include <string>

#include "color.h"

using std::vector;
using std::string;

class Palette {
public:
    virtual RGB MapValueToColor(double value, double min_value, double max_value) = 0;
    virtual ~Palette() {};
};
