#include <gtest/gtest.h>
#include <misc/fixedpoint.h>
#include <misc/vec2fix.h>
#include <script/luascript.h>

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

#define TEST_CONSTRUCTION(x, i, f)                                                                                                                             \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        lua->eval("val = FixedPoint.new(" x ");"                                                                                                               \
                  "intPart = val:intPart();"                                                                                                                   \
                  "fractionPart = val:fractionPart():str();"                                                                                                   \
                  "str = val:str()");                                                                                                                          \
        intPart = lua->get<int64_t>("intPart");                                                                                                                \
        fractionPart = lua->get<std::string>("fractionPart");                                                                                                  \
        str = lua->get<std::string>("str");                                                                                                                    \
        val = FixedPoint(x);                                                                                                                                   \
        ASSERT_EQ(intPart, i);                                                                                                                                 \
        ASSERT_EQ(intPart, val.intPart());                                                                                                                     \
        ASSERT_EQ(fractionPart, FixedPoint(f).str());                                                                                                          \
        ASSERT_EQ(val.fractionPart().str(), fractionPart);                                                                                                     \
        ASSERT_EQ(str, x);                                                                                                                                     \
        ASSERT_EQ(str, val.str());                                                                                                                             \
    } while (false)

static auto& lua = Script::LuaScript::getInstance();

TEST(FixedPointLua, TestLuaConstruction)
{
    FixedPoint val;

    lua->registerGlobalType<FixedPoint>();
    int64_t intPart;
    std::string fractionPart, str;

    TEST_CONSTRUCTION("1.23", 1, "0.23");

    TEST_CONSTRUCTION("1.023", 1, "0.023");

    TEST_CONSTRUCTION("981.00006", 981, "0.00006");

    TEST_CONSTRUCTION("-1.023", -1, "-0.023");

    TEST_CONSTRUCTION("-0.023", 0, "-0.023");

    val = FixedPoint::maxVal();
    lua->eval("val = FixedPoint.maxVal(); str = val:str()");
    str = lua->get<std::string>("str");
    ASSERT_EQ(val.str(), str);
    ASSERT_EQ(val, FixedPoint(str));

    val = FixedPoint::minVal();
    lua->eval("val = FixedPoint.minVal(); str = val:str()");
    str = lua->get<std::string>("str");
    ASSERT_EQ(val.str(), str);
    ASSERT_EQ(val, FixedPoint(val.str()));

    lua->eval("val = FixedPoint.new('3.1415926535897932384626433832795028841971693993751058'); toDouble = val:toDouble()");
    double toDouble = lua->get<double>("toDouble");
    ASSERT_NEAR(M_PI, toDouble, 0.000001);
}

TEST(FixedPointLua, AddSubtract)
{
    FixedPoint val;
    std::string str;

    lua->eval("val = FixedPoint.new('0.789') + FixedPoint.new('12.345'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("0.789") + FixedPoint("12.345");
    ASSERT_EQ(str, "13.134");
    ASSERT_EQ(val.str(), str);

    lua->eval("val = FixedPoint.new('0.5') + FixedPoint.new('1.6'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("0.5") + FixedPoint("1.6");
    ASSERT_EQ(str, "2.1");
    ASSERT_EQ(val.str(), str);

    lua->eval("val = FixedPoint.new('9.763345') - FixedPoint.new('72.00084542'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("9.763345") - FixedPoint("72.00084542");
    ASSERT_EQ(str, "-62.23750042");
    ASSERT_EQ(str, val.str());
}

TEST(FixedPointLua, MultiplyDivide)
{
    FixedPoint val;
    std::string str;

    lua->eval("val = FixedPoint.new('7.25') * FixedPoint.new('2.0'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("7.25") * FixedPoint("2.0");
    ASSERT_EQ(str, "14.5");
    ASSERT_EQ(str, val.str());

    lua->eval("val = FixedPoint.new('1.0') / FixedPoint.new('100'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("1.0") / FixedPoint("100");
    ASSERT_EQ(str, "0.01");
    ASSERT_EQ(str, val.str());

    lua->eval("val = FixedPoint.new('7') / FixedPoint.new('2'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("7") / FixedPoint("2");
    ASSERT_EQ(str, "3.5");
    ASSERT_EQ(str, val.str());

    lua->eval("val = FixedPoint.new('1.0') / FixedPoint.new('0.5'); str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("1.0") / FixedPoint("0.5");
    ASSERT_EQ(str, "2");
    ASSERT_EQ(str, val.str());
}

TEST(FixedPointLua, sqrt)
{
    for (auto x : {"16", "200", "7.6345", "9.122938", "0.000123123"})
    {
        double doubleVal = 0;
        {
            std::stringstream ss(x);
            ss >> doubleVal;
        }

        double fixedSquare = 0.0;
        std::string script = "val = FixedPoint.new(\"";
        script += x;
        script += "\"):sqrt(); fixedSquare = val:toDouble()";
        lua->eval(script.c_str());
        fixedSquare = lua->get<double>("fixedSquare");
        double floatSquare = std::sqrt(doubleVal);

        ASSERT_ALMOST_EQ(fixedSquare, floatSquare, 0.000001);
    }
}

TEST(FixedPointLua, PlainIntOperators)
{
    FixedPoint val;
    std::string str;

    lua->eval("val = FixedPoint.new('1.245') + 10; str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("1.245") + int32_t(10);
    ASSERT_EQ(str, FixedPoint("11.245").str());
    ASSERT_EQ(val.str(), str);

    lua->eval("val = FixedPoint.new('1.0') / 2; str = val:str()");
    str = lua->get<std::string>("str");
    val = FixedPoint("1.0") / uint8_t(2);
    ASSERT_EQ(str, FixedPoint("0.5").str());
    ASSERT_EQ(val.str(), str);
}

TEST(FixedPointLua, Rounding)
{
    int64_t round = 0;

    lua->eval("round = FixedPoint.new('1.49'):round()");
    round = lua->get<int64_t>("round");
    ASSERT_EQ(1, round);

    lua->eval("round = FixedPoint.new('1.50'):round()");
    round = lua->get<int64_t>("round");
    ASSERT_EQ(2, FixedPoint("1.50").round());

    lua->eval("round = FixedPoint.new('-1.49'):round()");
    round = lua->get<int64_t>("round");
    ASSERT_EQ(-1, round);

    lua->eval("round = FixedPoint.new('-1.50'):round()");
    round = lua->get<int64_t>("round");
    ASSERT_EQ(-2, round);
}

TEST(FixedPointLua, sin)
{
    for (int deg = -360; deg <= 360; deg++)
    {
        std::stringstream ss;
        ss << deg;
        double expected = sin(deg * M_PI / 180);
        std::string script = "result = FixedPoint.sin_degrees(FixedPoint.new(" + ss.str() + ")):toDouble()";
        lua->eval(script.c_str());
        double result = lua->get<double>("result");
        ASSERT_NEAR(expected, result, 0.002 * 2);
    }
}

TEST(FixedPointLua, cos)
{
    for (int deg = -360; deg <= 360; deg++)
    {
        std::stringstream ss;
        ss << deg;
        double expected = cos(deg * M_PI / 180);
        std::string script = "result = FixedPoint.cos_degrees(FixedPoint.new(" + ss.str() + ")):toDouble()";
        lua->eval(script.c_str());
        double result = lua->get<double>("result");
        ASSERT_NEAR(expected, result, 0.002 * 2);
    }
}

TEST(FixedPointLua, atan2)
{
    for (int x = -10; x <= 10; x++)
    {
        std::stringstream ssX;
        ssX << x;
        for (int y = -10; y <= 10; y++)
        {
            std::stringstream ssY;
            ssY << y;
            std::string script = "result = FixedPoint.atan2(FixedPoint.new(" + ssY.str() + "), FixedPoint.new(" + ssX.str() + ")):toDouble()";
            lua->eval(script.c_str());
            double result = lua->get<double>("result");
            double expected = atan2(y, x);
            ASSERT_NEAR(expected, result, 0.002 * (2 * M_PI));

            script = "result = FixedPoint.atan2_degrees(FixedPoint.new(" + ssY.str() + "), FixedPoint.new(" + ssX.str() + ")):toDouble()";
            lua->eval(script.c_str());
            result = lua->get<double>("result") * M_PI / 180;
            ASSERT_NEAR(expected, result, 0.002 * (2 * M_PI));
        }
    }
}

TEST(FixedPointLua, FloorCeil)
{
    int64_t floor = 0;
    int64_t ceil = 0;

    lua->eval("val = FixedPoint.new('1.5'); floor = val:floor(); ceil = val:ceil()");
    floor = lua->get<int64_t>("floor");
    ceil = lua->get<int64_t>("ceil");
    ASSERT_EQ(floor, 1);
    ASSERT_EQ(ceil, 2);

    lua->eval("val = FixedPoint.new(1); floor = val:floor(); ceil = val:ceil()");
    floor = lua->get<int64_t>("floor");
    ceil = lua->get<int64_t>("ceil");
    ASSERT_EQ(floor, 1);
    ASSERT_EQ(ceil, 1);

    lua->eval("val = FixedPoint.new('0.5'); floor = val:floor(); ceil = val:ceil()");
    floor = lua->get<int64_t>("floor");
    ceil = lua->get<int64_t>("ceil");
    ASSERT_EQ(floor, 0);
    ASSERT_EQ(ceil, 1);

    lua->eval("val = FixedPoint.new(0); floor = val:floor(); ceil = val:ceil()");
    floor = lua->get<int64_t>("floor");
    ceil = lua->get<int64_t>("ceil");
    ASSERT_EQ(floor, 0);
    ASSERT_EQ(ceil, 0);

    lua->eval("val = FixedPoint.new('-1.5'); floor = val:floor(); ceil = val:ceil()");
    floor = lua->get<int64_t>("floor");
    ceil = lua->get<int64_t>("ceil");
    ASSERT_EQ(floor, -2);
    ASSERT_EQ(ceil, -1);

    lua->eval("val = FixedPoint.new('-1'); floor = val:floor(); ceil = val:ceil()");
    floor = lua->get<int64_t>("floor");
    ceil = lua->get<int64_t>("ceil");
    ASSERT_EQ(floor, -1);
    ASSERT_EQ(ceil, -1);
}
