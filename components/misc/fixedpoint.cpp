#include "fixedpoint.h"
#include "assert.h"
#include "stringops.h"
#include <boost/multiprecision/cpp_int.hpp>
#include <sstream>

using uint128_t = boost::multiprecision::uint128_t;

static int64_t ipow(int64_t x, int64_t power)
{
    int64_t result = 1;
    for (int64_t i = 0; i < power; i++)
        result *= x;

    return result;
}

FixedPoint::FixedPoint(const std::string& str)
{
    auto split = Misc::StringUtils::split(str, '.');
    release_assert(split.size() <= 2);
    release_assert(split.size() > 0);

    int64_t integer = 0;
    {
        std::stringstream ss(split[0]);
        ss >> integer;
    }

    int64_t fractional = 0;
    int64_t fractionalDigits = 0;
    if (split.size() == 2)
    {
        fractionalDigits = int64_t(split[1].size()); // there could be leading zeros
        std::stringstream ss(split[1]);
        ss >> fractional;
    }

    int64_t sign = integer >= 0 ? 1 : -1;
    integer = std::abs(integer);

    int64_t tmpScalePow10 = fractionalDigits;
    int64_t tmpScale = ipow(10, tmpScalePow10);
    int64_t tmpFixed = (integer * tmpScale) + (fractional);

    int64_t scaleDifference = FixedPoint::scalingFactorPowerOf10 - tmpScalePow10;

    if (scaleDifference > 0)
        mVal = tmpFixed * ipow(10, scaleDifference);
    else
        mVal = tmpFixed / ipow(10, -scaleDifference);

    mVal *= sign;

#ifndef NDEBUG
    {
        std::stringstream ss(str);
        ss >> mDebugVal;
    }
#endif
}

FixedPoint::FixedPoint(int64_t integerValue) { *this = fromRawValue(integerValue * FixedPoint::scalingFactor); }

FixedPoint FixedPoint::fromRawValue(int64_t rawValue)
{
    FixedPoint retval;
    retval.mVal = rawValue;
#ifndef NDEBUG
    retval.mDebugVal = retval.toDouble();
#endif
    return retval;
}

int64_t FixedPoint::intPart() const { return mVal / FixedPoint::scalingFactor; }

FixedPoint FixedPoint::fractionPart() const
{
    int64_t intPart = this->intPart();
    int64_t rawVal = std::abs(mVal - (intPart * FixedPoint::scalingFactor));
    return fromRawValue(rawVal);
}

double FixedPoint::toDouble() const
{
    double val = mVal;
    double scale = FixedPoint::scalingFactor;

    return val / scale;
}

std::string FixedPoint::str() const
{
    std::stringstream ss;
    ss << intPart();

    std::string fractionalTempStr;
    fractionalTempStr.resize(FixedPoint::scalingFactorPowerOf10);

    int64_t fractionalTemp = fractionPart().mVal;
    for (int64_t i = 0; i < FixedPoint::scalingFactorPowerOf10; i++)
    {
        fractionalTempStr[size_t(FixedPoint::scalingFactorPowerOf10 - 1 - i)] = '0' + fractionalTemp % 10;
        fractionalTemp /= 10;
    }

    while (!fractionalTempStr.empty() && fractionalTempStr[fractionalTempStr.size() - 1] == '0')
        fractionalTempStr.pop_back();

    if (!fractionalTempStr.empty())
        ss << "." << fractionalTempStr;

    return ss.str();
}

FixedPoint FixedPoint::operator+(FixedPoint other) const
{
    FixedPoint retval = fromRawValue(mVal + other.mVal);
#ifndef NDEBUG
    retval.mDebugVal = mDebugVal + other.mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::operator-(FixedPoint other) const
{
    FixedPoint retval = fromRawValue(mVal - other.mVal);
#ifndef NDEBUG
    retval.mDebugVal = mDebugVal - other.mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::operator*(FixedPoint other) const
{
    int64_t sign = (mVal >= 0 ? 1 : -1) * (other.mVal >= 0 ? 1 : -1);

    uint128_t val1 = uint128_t(std::abs(mVal));
    uint128_t val2 = uint128_t(std::abs(other.mVal));
    uint128_t scale = FixedPoint::scalingFactor;

    uint128_t temp = (val1 * val2) / scale;

    int64_t val = sign * int64_t(temp);
    FixedPoint retval = fromRawValue(val);

#ifndef NDEBUG
    retval.mDebugVal = mDebugVal * other.mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::operator/(FixedPoint other) const
{
    int64_t sign = (mVal >= 0 ? 1 : -1) * (other.mVal >= 0 ? 1 : -1);

    uint128_t val1 = uint128_t(std::abs(mVal));
    uint128_t val2 = uint128_t(std::abs(other.mVal));
    uint128_t scale = FixedPoint::scalingFactor;

    uint128_t temp = (val1 * scale) / val2;

    int64_t val = sign * int64_t(temp);
    FixedPoint retval = fromRawValue(val);

#ifndef NDEBUG
    retval.mDebugVal = mDebugVal / other.mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::sqrt() const
{
    if (*this == 0)
        return 0;

    // basic newton raphson

    size_t iterationLimit = 10;

    FixedPoint x = *this;
    FixedPoint h;

    static FixedPoint epsilon("0.000001");

    size_t i = 0;
    do
    {
        h = ((x * x) - *this) / (2 * x);
        x = x - h;
        i++;
    } while (h.abs() >= epsilon && i < iterationLimit);

#ifndef NDEBUG
    double floatSqrt = std::sqrt(mDebugVal);
    (void)floatSqrt;
#endif

    return x;
}
