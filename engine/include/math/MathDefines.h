#pragma once

#include <cstdint>

#define UGP_FRONT    0
#define UGP_BACK     1
#define UGP_ON_PLANE 2
#define UGP_CLIPPED  3
#define UGP_CULLED   4
#define UGP_VISIBLE  5

constexpr float PI = 3.14159265358979323846f;
constexpr float TOW_PI = PI * 2;

#define GET_RADIANS(degree) (float)((degree) / 180.0f * PI)
#define GET_DEGREE(radians) (float)((radians) / PI * 180.0f)

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


