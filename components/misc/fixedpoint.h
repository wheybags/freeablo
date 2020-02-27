#pragma once
#include <cmath>
#include <cstdint>
#include <misc/assert.h>
#include <stdexcept>
#include <string>

namespace Serial
{
    class Saver;
    class Loader;
}

class FixedPoint
{
public:
    FixedPoint() = default;

    constexpr FixedPoint(const char* input)
    {
        class Helpers
        {
        public:
            static constexpr bool isDigit(char c) { return c <= '9' && c >= '0'; }

            static constexpr int64_t stoi(const char* str)
            {
                int64_t value = 0;

                while (*str)
                {
                    if (isDigit(*str))
                        value = value * 10 + (int64_t(*str) - int64_t('0'));
                    else
                        throw std::runtime_error("not a digit");

                    str++;
                }

                return value;
            }

            static constexpr size_t strlen(const char* str)
            {
                size_t result = 0;
                while (*str)
                {
                    result++;
                    str++;
                }
                return result;
            }

            // copies in to out, and stops if it sees NUL or '.'
            static constexpr const char* copyIntString(const char* in, char* out, size_t outSize, bool writeNul)
            {
                size_t outIndex = 0;

                while (true)
                {
                    if (*in == '\0' || *in == '.')
                        break;

                    if (!isDigit(*in))
                        throw std::runtime_error("not a digit");

                    out[outIndex] = *in;

                    in++;

                    // always leave room for a NUL terminator
                    outIndex = outIndex + 1;
                    if (outIndex > outSize - 2)
                        outIndex = outSize - 2;
                }

                if (writeNul)
                    out[outIndex] = '\0';

                return in;
            }
        };

        const char* inputPtr = input;

        int64_t sign = *inputPtr == '-' ? -1 : 1;
        if (sign == -1)
            inputPtr++;

        constexpr size_t beforeDecimalBufferSize = FixedPoint::scalingFactorPowerOf10 * 2;
        char beforeDecimal[beforeDecimalBufferSize] = {};

        constexpr size_t afterDecimalBufferSize = FixedPoint::scalingFactorPowerOf10 + 1;
        char afterDecimal[afterDecimalBufferSize] = {};

        // Zero pad before we copy in
        for (size_t i = 0; i < afterDecimalBufferSize - 1; i++)
            afterDecimal[i] = '0';
        afterDecimal[afterDecimalBufferSize - 1] = '\0';

        inputPtr = Helpers::copyIntString(inputPtr, beforeDecimal, beforeDecimalBufferSize, true);
        if (*inputPtr == '.')
        {
            inputPtr++;
            Helpers::copyIntString(inputPtr, afterDecimal, afterDecimalBufferSize, false);
        }

        // At this point, afterDecimal contains the string of digits after the decimal point,
        // truncated and RIGHT padded with zeros to scalingFactorPowerOf10 chars.
        // This is to handle leading zeros and any number of significant figures.

        mVal = sign * (Helpers::stoi(beforeDecimal) * FixedPoint::scalingFactor + Helpers::stoi(afterDecimal));

#ifndef NDEBUG
        mDebugVal = double(mVal) / double(FixedPoint::scalingFactor);
#endif
    }
    FixedPoint(const std::string& str) : FixedPoint(str.c_str()) {}

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

    void save(Serial::Saver& saver) const;
    void load(Serial::Loader& loader);

private:
    class RawConstructorTagType
    {
    };
#ifdef NDEBUG
    constexpr FixedPoint(int64_t rawValue, RawConstructorTagType) : mVal(rawValue) {}
#else
    constexpr FixedPoint(int64_t rawValue, double debugVal, RawConstructorTagType) : mVal(rawValue), mDebugVal(debugVal) {}
#endif

public:
    static constexpr FixedPoint fromRawValue(int64_t rawValue)
    {
#ifdef NDEBUG
        return FixedPoint(rawValue, RawConstructorTagType{});
#else
        return FixedPoint(rawValue, double(rawValue) / double(FixedPoint::scalingFactor), RawConstructorTagType{});
#endif
    }

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

    static FixedPoint invalid() { return FixedPoint::fromRawValue(std::numeric_limits<uint64_t>::max()); }
    bool isInvalid() const { return *this == invalid(); }

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
