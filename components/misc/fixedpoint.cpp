#include "fixedpoint.h"
#include "assert.h"
#include "int128.h"
#include "stringops.h"
#include <iomanip>
#include <serial/loader.h>
#include <sstream>

#ifdef _MSC_VER
#include <intrin.h>
#endif

#ifndef NDEBUG
#define _USE_MATH_DEFINES
#include "math.h"
#include <iostream>
#endif

constexpr int64_t FixedPoint::scalingFactorPowerOf10;
constexpr int64_t FixedPoint::scalingFactor;
constexpr int64_t FixedPoint::maxStringLength;

FixedPoint FixedPoint::PI("3.14159265359");
FixedPoint FixedPoint::epsilon = fromRawValue(1);
FixedPoint FixedPoint::LN10("2.302585092994046");

static inline int64_t i64abs(int64_t i)
{
    return i >= 0 ? i : -i; // not using std::abs because of a libc++ bug https://github.com/Project-OSRM/osrm-backend/issues/1000
}

static inline int64_t muldiv(int64_t n1, int64_t n2, int64_t d)
{
    // This function is designed to return int64_t((int128_t(n1) * int128_t(n2)) / int128_t(d))

// 64-bit gcc or clang
#if defined(__GNUC__) && defined(__LP64__)
    // AMD64 instruction set actually allows some limited 128-bit arithmetic.
    // We can:
    //   - Multiply two 64-bit numbers to get a 128 bit result (imulq)
    //   - Divide a 128-bit number by a 64-bit number to get a 128-bit result (idivq)
    // This is enough to implement our muldiv.
    int64_t retval;
    asm("imulq %1\n"   // multiply %1 (n1) by rax (n2), stores 128-bit result in rdx:rax
        "idivq %3\n"   // divide rdx:rax by %3 (d), stores 128-bit result in rdx:rax
        : "=a"(retval) // retval will be stored in rax (which is good, since that's the register it needs to be in to return it from this function)
        : "r"(n1),
          "a"(n2), // n2 will be loaded into rax before starting the block
          "r"(d)
        : "rdx", "cc"); // rdx register will be used, and condition codes will be overwritten

    return retval;
#elif defined(_MSC_VER)
    // These msvc intrinsics do the same thing as the above assembly
    int64_t high;
    int64_t low = _mul128(n1, n2, &high);
    return _div128(high, low, d, nullptr);
#else
    // fall back to a naive implementation using the int128_t class from Abseil
    int64_t ret;
    // Optimisation : Only use 128bit if 64bit will overflow.
    if (n2 == 0 || i64abs(n1) <= INT64_MAX / i64abs(n2))
        ret = n1 * n2 / d;
    else
        ret = int64_t(absl::int128(n1) * n2 / d);
    return ret;
#endif
}

FixedPoint::FixedPoint(int64_t integerValue) { *this = fromRawValue(integerValue * FixedPoint::scalingFactor); }

static FixedPoint pow(FixedPoint x, size_t k)
{
    FixedPoint ret("1.0");

    while (k > 0)
    {
        if ((k % 2) == 1)
            ret *= x;

        k >>= 1;
        x *= x;
    }

    return ret;
}

std::optional<FixedPoint> FixedPoint::tryParseFromString(const char* str, char mode)
{
    std::stringstream ss;

    if (mode == 'i')
        ss << str;

    else if (mode == 'x')
    {
        const std::string aux(str);
        if (size_t pos = aux.find('.'); pos != std::string::npos)
        {
            const std::string beforePoint = aux.substr(0, pos);
            const std::string afterPoint = aux.substr(pos + 1);
            ss << strtol(beforePoint.c_str(), nullptr, 16) << ".";
            auto hexToInt = [](char c) -> FixedPoint {
                if (c >= '0' && c <= '9')
                    return c - '0';
                else if (c >= 'A' && c <= 'F')
                    return c - 'A' + 10;
                else if (c >= 'a' && c <= 'f')
                    return c - 'a' + 10;
                return 0;
            };

            FixedPoint fp("0.0");
            int64_t exp = 1;
            for (char c : afterPoint)
                fp += hexToInt(c) / pow("16", exp++);

            ss << fp.str().substr(2);
        }

        else
            ss << strtol(aux.c_str(), nullptr, 16);
    }

    else
    {
#ifndef NDEBUG
        std::cerr << "Invalid mode '" << mode << "'\n";
#endif
        return std::nullopt;
    }

    try
    {
        return FixedPoint(ss.str().c_str());
    }

    catch (std::runtime_error& err)
    {
#ifndef NDEBUG
        std::cerr << "FixedPoint error: " << err.what() << "\n";
#endif
        return std::nullopt;
    }
}

void FixedPoint::save(Serial::Saver& saver) const { saver.save(mVal); }

void FixedPoint::load(Serial::Loader& loader) { *this = fromRawValue(loader.load<int64_t>()); }

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

int64_t FixedPoint::floor() const
{
    FixedPoint frac = fractionPart();
    int64_t i = intPart();

    if (frac != 0 && i < 0)
        return i - 1;

    return i;
}

int64_t FixedPoint::ceil() const
{
    FixedPoint frac = fractionPart();
    int64_t i = intPart();

    if (frac != 0 && i >= 0)
        return i + 1;

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

    ss << i64abs(intPart());

    int64_t fractional = i64abs(fractionPart().mVal);
    if (fractional > 0)
    {
        // LEFT pad with zeros to scalingFactorPowerOf10 chars, then strip zeros to the right.
        std::stringstream ssFractional;
        ssFractional << std::setfill('0') << std::setw(scalingFactorPowerOf10) << std::right << fractional;

        std::string fractionalStr = ssFractional.str();
        Misc::StringUtils::rstrip(fractionalStr, "0");

        ss << "." << fractionalStr;
    }

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
    int64_t val = muldiv(mVal, other.mVal, scalingFactor);
    FixedPoint retval = fromRawValue(val);

#ifndef NDEBUG
    retval.mDebugVal = mDebugVal * other.mDebugVal;
#endif
    return retval;
}

FixedPoint FixedPoint::operator/(FixedPoint other) const
{
    int64_t val = muldiv(mVal, scalingFactor, other.mVal);
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
FixedPoint FixedPoint::tan(FixedPoint rad) { return sin(rad) / cos(rad); }

FixedPoint FixedPoint::atan2_degrees(FixedPoint y, FixedPoint x)
{
    static const FixedPoint radToDeg = FixedPoint(180) / PI;
    return atan2(y, x) * radToDeg;
}
FixedPoint FixedPoint::sin_degrees(FixedPoint deg)
{
    static const FixedPoint degToRad = PI / 180;
    return sin(deg * degToRad);
}
FixedPoint FixedPoint::cos_degrees(FixedPoint deg) { return sin_degrees(deg + 90); }
FixedPoint FixedPoint::tan_degrees(FixedPoint deg) { return sin_degrees(deg) / cos_degrees(deg); }

FixedPoint FixedPoint::ln(FixedPoint x)
{
    int64_t n = 1;
    if (x >= 1)
        while ((x / 10).floor() > 10)
        {
            x /= 10;
            ++n;
        }
    else if (x > 0)
        while ((x * 10).intPart() < 1)
        {
            x *= 10;
            --n;
        }
    else
        return FixedPoint("0");

    FixedPoint y = (x - 1) / (x + 1);

    FixedPoint acc = 0;
    constexpr size_t iterationLimit = 50;

    for (uint64_t i = 0; i < iterationLimit; ++i)
    {
        uint64_t k = 2 * i + 1;
        FixedPoint num = pow(y, k);
        acc += num / k;
    }

    FixedPoint result = LN10 * (n - 1) + acc * 2;

#ifndef NDEBUG
    result.mDebugVal = log(x.mDebugVal);
#endif

    return result;
}

static constexpr uint64_t factorial(uint64_t x)
{
    uint64_t ret = 1;
    while (x > 1)
        ret *= x--;

    return ret;
}

FixedPoint FixedPoint::exp(FixedPoint x)
{
    FixedPoint result("1");
    result += x;

    constexpr size_t iterationLimit = 20;
    for (size_t i = 2; i < iterationLimit; ++i)
        result += pow(x, i) / factorial(i);

#ifndef NDEBUG
    result.mDebugVal = ::exp(x.mDebugVal);
#endif

    return result;
}
