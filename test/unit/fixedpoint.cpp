#include <gtest/gtest.h>
#include <misc/simplevec2.h>

#define _USE_MATH_DEFINES
#include "math.h"

#define ASSERT_ALMOST_EQ(expected, actual, delta)                                                                                                              \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        auto assert_expected = (expected);                                                                                                                     \
        auto assert_actual = (actual);                                                                                                                         \
        auto assert_delta = (delta);                                                                                                                           \
        ASSERT_GT(assert_expected, assert_actual - assert_delta);                                                                                              \
        ASSERT_LT(assert_expected, assert_actual + assert_delta);                                                                                              \
    } while (false)

TEST(FixedPoint, TestConstruction)
{
    FixedPoint val;

    val = FixedPoint("1.23");
    ASSERT_EQ(val.intPart(), 1);
    ASSERT_EQ(val.fractionPart(), FixedPoint("0.23"));
    ASSERT_EQ(val.str(), "1.23");

    val = FixedPoint("1.023");
    ASSERT_EQ(val.intPart(), 1);
    ASSERT_EQ(val.fractionPart(), FixedPoint("0.023"));
    ASSERT_EQ(val.str(), "1.023");

    val = FixedPoint("981.00006");
    ASSERT_EQ(val.intPart(), 981);
    ASSERT_EQ(val.fractionPart(), FixedPoint("0.00006"));
    ASSERT_EQ(val.str(), "981.00006");

    val = FixedPoint("-1.023");
    ASSERT_EQ(val.intPart(), -1);
    ASSERT_EQ(val.fractionPart(), FixedPoint("-0.023"));
    ASSERT_EQ(val.str(), "-1.023");

    val = FixedPoint("-0.023");
    ASSERT_EQ(val.intPart(), 0);
    ASSERT_EQ(val.fractionPart(), FixedPoint("-0.023"));
    ASSERT_EQ(val.str(), "-0.023");

    val = FixedPoint::maxVal();
    ASSERT_EQ(val, FixedPoint(val.str()));

    val = FixedPoint::minVal();
    ASSERT_EQ(val, FixedPoint(val.str()));

    val = FixedPoint("3.1415926535897932384626433832795028841971693993751058");
    ASSERT_NEAR(M_PI, val.toDouble(), 0.000001);
}

TEST(FixedPoint, AddSubtract)
{
    FixedPoint val;

    val = FixedPoint("0.789") + FixedPoint("12.345");
    ASSERT_EQ(val.str(), "13.134");

    val = FixedPoint("0.5") + FixedPoint("1.6");
    ASSERT_EQ(val.str(), "2.1");

    val = FixedPoint("9.763345") - FixedPoint("72.00084542");
    ASSERT_EQ(val.str(), "-62.23750042");
}

TEST(FixedPoint, MultiplyDivide)
{
    FixedPoint val;

    val = FixedPoint("7.25") * FixedPoint("2.0");
    ASSERT_EQ(val.str(), "14.5");

    val = FixedPoint("1.0") / FixedPoint("100");
    ASSERT_EQ(val.str(), "0.01");

    val = FixedPoint("7") / FixedPoint("2");
    ASSERT_EQ(val.str(), "3.5");

    val = FixedPoint("1.0") / FixedPoint("0.5");
    ASSERT_EQ(val.str(), "2");
}

TEST(FixedPoint, sqrt)
{
    for (auto x : {"16", "200", "7.6345", "9.122938", "0.000123123"})
    {
        double doubleVal = 0;
        {
            std::stringstream ss(x);
            ss >> doubleVal;
        }

        auto fixedSquare = FixedPoint(x).sqrt();
        double floatSquare = std::sqrt(doubleVal);

        ASSERT_ALMOST_EQ(fixedSquare.toDouble(), floatSquare, 0.000001);
    }
}

TEST(FixedPoint, PlainIntOperators)
{
    FixedPoint val;

    val = FixedPoint("1.245") + int32_t(10);
    ASSERT_EQ(val, FixedPoint("11.245"));

    val = FixedPoint("1.0") / uint8_t(2);
    ASSERT_EQ(val, FixedPoint("0.5"));
}

TEST(FixedPoint, Rounding)
{
    ASSERT_EQ(1, FixedPoint("1.49").round());
    ASSERT_EQ(2, FixedPoint("1.50").round());
    ASSERT_EQ(-1, FixedPoint("-1.49").round());
    ASSERT_EQ(-2, FixedPoint("-1.50").round());
}

TEST(FixedPoint, VectorToDirection)
{
    ASSERT_EQ(Vec2Fix(1, 0).getDirection().getDirection8(), Misc::Direction8::south_east);
    ASSERT_EQ(Vec2Fix(-1, 0).getDirection().getDirection8(), Misc::Direction8::north_west);
    ASSERT_EQ(Vec2Fix(0, 1).getDirection().getDirection8(), Misc::Direction8::south_west);
    ASSERT_EQ(Vec2Fix(0, -1).getDirection().getDirection8(), Misc::Direction8::north_east);

    ASSERT_EQ(Vec2Fix(1, 1).getDirection().getDirection8(), Misc::Direction8::south);
    ASSERT_EQ(Vec2Fix(1, -1).getDirection().getDirection8(), Misc::Direction8::east);
    ASSERT_EQ(Vec2Fix(-1, -1).getDirection().getDirection8(), Misc::Direction8::north);
    ASSERT_EQ(Vec2Fix(-1, 1).getDirection().getDirection8(), Misc::Direction8::west);
}

TEST(FixedPoint, sin)
{
    for (int deg = -360; deg <= 360; deg++)
    {
        double expected = sin(deg * M_PI / 180);
        double result = FixedPoint::sin_degrees(deg).toDouble();
        ASSERT_NEAR(expected, result, 0.002 * 2);
    }
}

TEST(FixedPoint, cos)
{
    for (int deg = -360; deg <= 360; deg++)
    {
        double expected = cos(deg * M_PI / 180);
        double result = FixedPoint::cos_degrees(deg).toDouble();
        ASSERT_NEAR(expected, result, 0.002 * 2);
    }
}

TEST(FixedPoint, atan2)
{
    for (int x = -10; x <= 10; x++)
    {
        for (int y = -10; y <= 10; y++)
        {
            double expected = atan2(y, x);
            double result = FixedPoint::atan2(y, x).toDouble();
            ASSERT_NEAR(expected, result, 0.002 * (2 * M_PI));
            result = FixedPoint::atan2_degrees(y, x).toDouble() * M_PI / 180;
            ASSERT_NEAR(expected, result, 0.002 * (2 * M_PI));
        }
    }
}

TEST(FixedPoint, muldivOptimisations)
{
    // Multiply/divide operations currently have optimisations to only use 128bit if 64bit will overflow.
    // NOTE: These are not black box tests, they require a bit of knowledge of the internal implementation.
    // If the implementation changes and/or these tests become problematic to maintain just delete them.
    const int64_t scalingFactor = 1000000000;

    const int64_t divLimit = INT64_MAX / scalingFactor;
    for (int64_t value = divLimit - 10; value <= divLimit + 10; value++)
        for (int64_t sign1 : {-1, 1})
            for (int64_t sign2 : {-1, 1})
                ASSERT_EQ(FixedPoint::fromRawValue(sign1 * sign2 * value / 10), FixedPoint::fromRawValue(sign1 * value) / FixedPoint(sign2 * 10));

    const int64_t mulLimit = 3037000500; // ~= sqrt(INT64_MAX) ~= 2^31.5
    for (int64_t value = mulLimit - 10; value <= mulLimit + 10; value++)
        for (int64_t sign1 : {-1, 1})
            for (int64_t sign2 : {-1, 1})
            {
                // Working will fit in an UNSIGNED 64 bit, but not signed.
                int64_t result = (uint64_t)mulLimit * value / scalingFactor;
                result *= sign1 * sign2;
                ASSERT_EQ(FixedPoint::fromRawValue(result), FixedPoint::fromRawValue(sign1 * value) * FixedPoint::fromRawValue(sign2 * mulLimit));
                ASSERT_EQ(FixedPoint::fromRawValue(result), FixedPoint::fromRawValue(sign1 * mulLimit) * FixedPoint::fromRawValue(sign2 * value));
            }
}

TEST(FixedPoint, ConstExprConstruct)
{
    constexpr FixedPoint one = FixedPoint("1");
    ASSERT_EQ(one.str(), "1");

    constexpr FixedPoint onePointFive = FixedPoint("1.5");
    ASSERT_EQ(onePointFive.str(), "1.5");

    constexpr FixedPoint zeroPointZeroZeroFive = FixedPoint("0.005");
    ASSERT_EQ(zeroPointZeroZeroFive.str(), "0.005");

    constexpr FixedPoint onePointZeroZeroFive = FixedPoint("1.005");
    ASSERT_EQ(onePointZeroZeroFive.str(), "1.005");

    constexpr FixedPoint minusOne = FixedPoint("-1");
    ASSERT_EQ(minusOne.str(), "-1");

    constexpr FixedPoint minusOnePointFive = FixedPoint("-1.5");
    ASSERT_EQ(minusOnePointFive.str(), "-1.5");

    constexpr FixedPoint minusZeroPointZeroZeroFive = FixedPoint("-0.005");
    ASSERT_EQ(minusZeroPointZeroZeroFive.str(), "-0.005");

    constexpr FixedPoint minusOnePointZeroZeroFive = FixedPoint("-1.005");
    ASSERT_EQ(minusOnePointZeroZeroFive.str(), "-1.005");
}

TEST(FixedPoint, FloorCeil)
{
    ASSERT_EQ(FixedPoint("1.5").floor(), 1);
    ASSERT_EQ(FixedPoint("1.5").ceil(), 2);

    ASSERT_EQ(FixedPoint(1).floor(), 1);
    ASSERT_EQ(FixedPoint(1).ceil(), 1);

    ASSERT_EQ(FixedPoint("0.5").floor(), 0);
    ASSERT_EQ(FixedPoint("0.5").ceil(), 1);

    ASSERT_EQ(FixedPoint(0).floor(), 0);
    ASSERT_EQ(FixedPoint(0).ceil(), 0);

    ASSERT_EQ((FixedPoint("-1.5")).floor(), -2);
    ASSERT_EQ((FixedPoint("-1.5")).ceil(), -1);

    ASSERT_EQ((FixedPoint(-1)).floor(), -1);
    ASSERT_EQ((FixedPoint(-1)).ceil(), -1);
}
