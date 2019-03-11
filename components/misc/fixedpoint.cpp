#include "fixedpoint.h"
#include "assert.h"
#include "stringops.h"
#include <boost/multiprecision/cpp_int.hpp>
#include <sstream>

#ifndef NDEBUG
#define _USE_MATH_DEFINES
#include "math.h"
#endif

using int128_t = boost::multiprecision::int128_t;

constexpr int64_t FixedPoint::scalingFactorPowerOf10;
constexpr int64_t FixedPoint::scalingFactor;

FixedPoint FixedPoint::PI("3.14159265359");
FixedPoint FixedPoint::epsilon = fromRawValue(1);

static int64_t i64abs(int64_t i)
{
    return i >= 0 ? i : -i; // not using std::abs because of a libc++ bug https://github.com/Project-OSRM/osrm-backend/issues/1000
}

FixedPoint::FixedPoint(const std::string& str)
{
    auto split = Misc::StringUtils::split(str, '.');
    release_assert(split.size() <= 2);
    release_assert(split.size() > 0);

    int64_t sign = split[0][0] == '-' ? -1 : 1;

    int64_t integer = 0;
    {
        std::stringstream ss(split[0]);
        ss >> integer;
        integer = i64abs(integer);
    }

    int64_t fractional = 0;
    if (split.size() == 2)
    {
        // Truncate and RIGHT pad with zeros to scalingFactorPowerOf10 chars.
        // This is to handle leading zeros and any number of significant figures.
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(scalingFactorPowerOf10) << std::left << split[1].substr(0, scalingFactorPowerOf10);
        ss >> fractional;
    }

    mVal = integer * scalingFactor + fractional;
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

int64_t FixedPoint::round() const
{
    FixedPoint frac = fractionPart();
    int64_t i = intPart();
    if (frac >= FixedPoint("0.5"))
        i++;
    else if (frac <= FixedPoint("-0.5"))
        i--;
    return i;
}

FixedPoint FixedPoint::fractionPart() const
{
    int64_t sign = (mVal >= 0 ? 1 : -1);
    int64_t intPart = this->intPart();
    int64_t temp = i64abs(mVal - (intPart * FixedPoint::scalingFactor));
    int64_t rawVal = sign * temp;
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

    if (*this < 0)
        ss << "-";

    ss << i64abs(this->intPart());

    std::string fractionalTempStr;
    fractionalTempStr.resize(FixedPoint::scalingFactorPowerOf10);

    int64_t fractionalTemp = fractionPart().abs().mVal;
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

FixedPoint FixedPoint::operator-() const
{
    FixedPoint retval = fromRawValue(-mVal);
#ifndef NDEBUG
    retval.mDebugVal = -mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::operator*(FixedPoint other) const
{
    int128_t val1 = int128_t(mVal);
    int128_t val2 = int128_t(other.mVal);
    int128_t scale = FixedPoint::scalingFactor;

    int128_t temp = (val1 * val2) / scale;

    int64_t val = int64_t(temp);
    FixedPoint retval = fromRawValue(val);

#ifndef NDEBUG
    retval.mDebugVal = mDebugVal * other.mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::operator/(FixedPoint other) const
{
    int128_t val1 = int128_t(mVal);
    int128_t val2 = int128_t(other.mVal);
    int128_t scale = FixedPoint::scalingFactor;

    int128_t temp = (val1 * scale) / val2;

    int64_t val = int64_t(temp);
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

    static const FixedPoint convergenceDiff("0.000001");

    size_t i = 0;
    do
    {
        h = ((x * x) - *this) / (FixedPoint(2) * x);
        x = x - h;
        i++;
    } while (h.abs() >= convergenceDiff && i < iterationLimit);

#ifndef NDEBUG
    x.mDebugVal = std::sqrt(mDebugVal);
#endif

    return x;
}

FixedPoint FixedPoint::atan2(FixedPoint y, FixedPoint x)
{
    // https://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization/
    static const FixedPoint QTR_PI = PI / 4;
    static const FixedPoint THREE_QTR_PI = PI * 3 / 4;
    static const FixedPoint COEFF_1("0.9817");
    static const FixedPoint COEFF_3("0.1963");
    FixedPoint absY = y.abs() + epsilon; // kludge to prevent 0/0 condition
    FixedPoint r, angle;

    if (x == 0 && y == 0)
        // Domain error
        return 0;

    if (x >= 0)
    {
        r = (x - absY) / (x + absY);
        angle = QTR_PI;
    }
    else
    {
        r = (x + absY) / (absY - x);
        angle = THREE_QTR_PI;
    }
    angle += (COEFF_3 * r * r - COEFF_1) * r;

    FixedPoint result = (y < 0) ? -angle : angle; // negate if in quad III or IV

#ifndef NDEBUG
    result.mDebugVal = ::atan2(y.mDebugVal, x.mDebugVal);
#endif

    return result;
}

FixedPoint FixedPoint::sin(FixedPoint rad)
{
    // Chebyshev polynomial approximation (5th order).
    // sin(x) ~= 0.99997860 * x - 0.16649840 * x^3 + 0.00799232 * x^5;
    // Used for -PI/2 -> PI/2 and symmetry used for the rest.
    static const FixedPoint TWO_PI = PI * 2;
    static const FixedPoint HALF_PI = PI / 2;
    static const FixedPoint THREE_HALF_PI = PI * 3 / 2;
    static const FixedPoint COEFF_1("0.99997860");
    static const FixedPoint COEFF_3("0.16649840");
    static const FixedPoint COEFF_5("0.00799232");

    // Normalise 0 -> 2PI.
    while (rad >= TWO_PI)
        rad -= TWO_PI;
    while (rad < 0)
        rad += TWO_PI;

    // Reduce to -PI/2 -> PI/2.
    FixedPoint x;
    if (rad < PI)
        if (rad < HALF_PI)
            x = rad;
        else
            x = PI - rad;
    else if (rad < THREE_HALF_PI)
        x = rad - PI;
    else
        x = TWO_PI - rad;

    FixedPoint xSquared = x * x;
    FixedPoint result = (COEFF_1 + (-COEFF_3 + COEFF_5 * xSquared) * xSquared) * x;

    result = (rad > PI) ? -result : result;

#ifndef NDEBUG
    result.mDebugVal = ::sin(rad.mDebugVal);
#endif

    return result;
}

FixedPoint FixedPoint::cos(FixedPoint rad) { return sin(rad + PI / 2); }
FixedPoint FixedPoint::atan2_degrees(FixedPoint y, FixedPoint x) { return atan2(x, y) * 180 / PI; }
FixedPoint FixedPoint::sin_degrees(FixedPoint deg) { return sin(deg * PI / 180); }
FixedPoint FixedPoint::cos_degrees(FixedPoint deg) { return sin_degrees(deg + 90); }
