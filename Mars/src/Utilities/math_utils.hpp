//
// Created by arlev on 21.12.2022.
//

#pragma once

#include <stdint.h>

constexpr float PI32 = 3.141592741f;
constexpr double PI64 = 3.141592653589793;

constexpr size_t KiloBytes(const size_t amount)
{
    return amount * 1024ULL;
}

constexpr size_t MegaBytes(const size_t amount)
{
    return amount * 1024ULL * 1024ULL;
}

constexpr size_t GigaBytes(const size_t amount)
{
    return amount * 1024ULL * 1024ULL * 1024ULL;
}

constexpr uint32_t BIT(uint32_t value)
{
    return 1 << value;
}

template<typename T, int N>
constexpr size_t arraysize(const T(&array)[N])
{
    return N;
}

template<typename T, int N>
constexpr uint32_t arraysize32(const T(&array)[N])
{
    return N;
}

constexpr float GetRadians(float angle)
{
    constexpr float factor = PI32 / 180.0f;
    return angle * factor;
}

constexpr double GetRadians(double angle)
{
    constexpr double factor = PI64 / 180.0;
    return angle * factor;
}

constexpr float GetAngle(float rad)
{
    constexpr float factor = 180.0f / PI32;
    return rad * factor;
}

constexpr double GetAngle(double rad)
{
    constexpr double factor = 180.0 / PI64;
    return rad * factor;
}

template<typename T>
constexpr T clamp(T value, T min, T max)
{
    const T result = value < min ? min : value;
    return result > max ? max : result;
}

template<typename T>
constexpr T min(T a, T b)
{
    return a < b ? a : b;
}

template<typename T>
constexpr T max(T a, T b)
{
    return a > b ? a : b;
}

constexpr uint32_t MakeVersionU32(uint32_t major, uint32_t minor, uint32_t patch)
{
    return ((major << 22) | (minor << 12) | patch);
}

constexpr uint32_t GetVersionU32Major(uint32_t version)
{
    return version >> 22;
}

constexpr uint32_t GetVersionU32Minor(uint32_t version)
{
    return (version >> 12) & 0b1111111111;
}

constexpr uint32_t GetVersionU32Patch(uint32_t version)
{
    return version & 0b111111111111;
}

inline uint32_t GetMipLevels(uint32_t width, uint32_t height)
{
    return uint32_t(std::floor(std::log2(max(width, height)))) + 1;
}