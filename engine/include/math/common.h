#pragma once

#include <cstdint>


constexpr float PI = 3.14159265358979323846f;
constexpr float TOW_PI = PI * 2;

inline float sat(float a) {
    if (a < 0.0f)
    {
        return 0.0f;
    }

    if (a > 1.0f)
    {
        return 1.0f;
    }
    return a;
}

inline float Clamp(float v, float a, float b)
{
    if (v < a)
    {
        return a;
    }

    if (v > b)
    {
        return b;
    }

    return v;
}
