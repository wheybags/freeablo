#pragma once
#include "fixedpoint_internal.h"
#include <cmath>
#include <cstdint>
#include <string>

template <typename T> constexpr typename std::remove_reference<T>::type makeprval(T&& t) { return t; }

#define is_constexpr(e) noexcept(makeprval(e))

class FixedPoint
{
public:
    FixedPoint() = default;
    FixedPoint(const std::string& str);
    FixedPoint(const char* str) : FixedPoint(std::string(str)) {}
    FixedPoint(int64_t integerValue);
    FixedPoint(uint64_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(int32_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(uint32_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(int16_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(uint16_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(int8_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(uint8_t integerValue) : FixedPoint(int64_t(integerValue)) {}
    FixedPoint(double) = delete;
    FixedPoint(float) = delete;

private:
    class RawConstructorTagType
    {
    };
#ifdef NDEBUG
    constexpr FixedPoint(int64_t rawValue, ConstructorTagType) : mVal(rawValue) {}
#else
    constexpr FixedPoint(int64_t rawValue, double debugVal, RawConstructorTagType) : mVal(rawValue), mDebugVal(debugVal) {}
#endif

public:
    static constexpr FixedPoint fromRawValue(int64_t rawValue)
    {
#ifdef NDEBUG
        return FixedPoint(rawValue, ConstructorTagType{});
#else
        return FixedPoint(rawValue, double(rawValue) / double(FixedPoint::scalingFactor), RawConstructorTagType{});
#endif
    }

#define MakeFixed1(IntPart) FixedPoint::fromRawValue(FIXED_POINT_INTPART_##IntPart* FixedPoint::scalingFactor)
#define MakeFixed2(IntPart, FractionPart)                                                                                                                      \
    FixedPoint::fromRawValue(FIXED_POINT_INTPART_##IntPart* FixedPoint::scalingFactor + FIXED_POINT_FRACTION_##FractionPart)

#define MakeFixedGetMacro(_1, _2, NAME, ...) NAME
#define MakeFixed(...) MakeFixedGetMacro(__VA_ARGS__, MakeFixed2, MakeFixed1, _)(__VA_ARGS__)

    int64_t rawValue() const { return mVal; }

    int64_t intPart() const;
    FixedPoint fractionPart() const;

    int64_t round() const;
    int64_t floor() const;
    int64_t ceil() const;

    double toDouble() const; /// NOT to be used in the game simulation. For testing/gui only
    std::string str() const;

    bool operator==(FixedPoint other) const { return mVal == other.mVal; }
    bool operator!=(FixedPoint other) const { return mVal != other.mVal; }
    bool operator>(FixedPoint other) const { return mVal > other.mVal; }
    bool operator<(FixedPoint other) const { return mVal < other.mVal; }
    bool operator>=(FixedPoint other) const { return mVal >= other.mVal; }
    bool operator<=(FixedPoint other) const { return mVal <= other.mVal; }

    FixedPoint operator+(FixedPoint other) const;
    FixedPoint operator-(FixedPoint other) const;
    constexpr FixedPoint operator-() const { return fromRawValue(-mVal); }

    FixedPoint operator*(FixedPoint other) const;
    FixedPoint operator/(FixedPoint other) const;

    FixedPoint& operator+=(FixedPoint other)
    {
        *this = *this + other;
        return *this;
    }
    FixedPoint& operator-=(FixedPoint other)
    {
        *this = *this - other;
        return *this;
    }
    FixedPoint& operator*=(FixedPoint other)
    {
        *this = *this * other;
        return *this;
    }
    FixedPoint& operator/=(FixedPoint other)
    {
        *this = *this / other;
        return *this;
    }

    FixedPoint sqrt() const;
    FixedPoint abs() const
    {
        // not using std::abs because of a libc++ bug https://github.com/Project-OSRM/osrm-backend/issues/1000
        return fromRawValue(mVal >= 0 ? mVal : -mVal);
    }

    static FixedPoint minVal() { return fromRawValue(INT64_MIN); }
    static FixedPoint maxVal() { return fromRawValue(INT64_MAX); }

    static FixedPoint atan2(FixedPoint y, FixedPoint x);
    static FixedPoint sin(FixedPoint rad);
    static FixedPoint cos(FixedPoint rad);
    static FixedPoint atan2_degrees(FixedPoint y, FixedPoint x);
    static FixedPoint sin_degrees(FixedPoint deg);
    static FixedPoint cos_degrees(FixedPoint deg);

    // The scaling factor to use, as a power of 10.
    // Essentially the number of base 10 digits after the decimal point.
    // We're using a base 10 scaling factor becasue it makes things muccch easier to debug.
    // We could maybe switch to binary at some point in the future, if it ever becomes necessary.
    static constexpr int64_t scalingFactorPowerOf10 = 9;

    // must be equal to pow(10, scalingFactorPowerOf10).
    // mVal is just the intended value multiplid by scalingFactor.
    // So, eg with scale 1000, 1.23 would have value 1230.
    static constexpr int64_t scalingFactor = 1000000000;

private:
    static FixedPoint PI;
    static FixedPoint epsilon;

    int64_t mVal = 0;
#ifndef NDEBUG
    double mDebugVal = 0;
#endif
};
