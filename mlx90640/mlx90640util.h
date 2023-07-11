#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "mlx90640.h"

namespace MLX90640Util {

using std::string;
using std::unordered_map;
using std::hash;
using MLX90640::ReadingPattern;
using MLX90640::AdcResolution;
using MLX90640::RefreshRate;

template <typename E> 
struct ehash
{
    size_t operator()(const E& v) const
    {
        return hash<int>()(static_cast<int>(v));
    }
};

extern unordered_map<ReadingPattern, string, ehash<ReadingPattern>> ReadingPatternStrings;
extern unordered_map<AdcResolution, string, ehash<AdcResolution>> AdcResolutionStrings;
extern unordered_map<RefreshRate, string, ehash<RefreshRate>> RfreshRateStrings;

}