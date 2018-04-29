#pragma once
#include <cmath>
#include <cstdint>
#include <string>

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

    static FixedPoint fromRawValue(int64_t rawValue);

    int64_t rawValue() const { return mVal; }

    int64_t intPart() const;
    FixedPoint fractionPart() const;

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
    FixedPoint abs() const { return fromRawValue(std::abs(mVal)); }

private:
    // The scaling factor to use, as a power of 10.
    // Essentially the number of base 10 digits after the decimal point.
    // We're using a base 10 scaling factor becasue it makes things muccch easier to debug.
    // We could maybe switch to binary at some point in the future, if it ever becomes necessary.
    static constexpr int64_t scalingFactorPowerOf10 = 9;

    // must be equal to pow(10, scalingFactorPowerOf10).
    // mVal is just the intended value multiplid by scalingFactor.
    // So, eg with scale 1000, 1.23 would have value 1230.
    static constexpr int64_t scalingFactor = 1000000000;

    int64_t mVal = 0;
#ifndef NDEBUG
    double mDebugVal = 0;
#endif
};

template <typename T> FixedPoint operator+(T a, FixedPoint b) { return FixedPoint(a) + b; }
template <typename T> FixedPoint operator-(T a, FixedPoint b) { return FixedPoint(a) - b; }
template <typename T> FixedPoint operator*(T a, FixedPoint b) { return FixedPoint(a) * b; }
template <typename T> FixedPoint operator/(T a, FixedPoint b) { return FixedPoint(a) / b; }
template <typename T> bool operator==(T a, FixedPoint b) { return FixedPoint(a) == b; }
template <typename T> bool operator!=(T a, FixedPoint b) { return FixedPoint(a) != b; }
template <typename T> bool operator>(T a, FixedPoint b) { return FixedPoint(a) > b; }
template <typename T> bool operator<(T a, FixedPoint b) { return FixedPoint(a) < b; }
template <typename T> bool operator>=(T a, FixedPoint b) { return FixedPoint(a) >= b; }
template <typename T> bool operator<=(T a, FixedPoint b) { return FixedPoint(a) <= b; }
