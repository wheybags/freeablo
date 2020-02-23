#include "fixedpoint.h"
#include "assert.h"
#include "int128.h"
#include "stringops.h"
#include <iomanip>
#include <script/luascript.h>
#include <sstream>

#ifndef NDEBUG
#define _USE_MATH_DEFINES
#include "math.h"
#endif

namespace Script
{
    template <> void LuaScript::registerGlobalType<FixedPoint>()
    {
        luaL_newmetatable(mState, "FixedPoint");
        auto constructor = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            if (lua_isstring(state, -2))
            {
                std::string str = lua_tostring(state, -2);
                *self = new FixedPoint(str);
            }

            else if (lua_isinteger(state, -2))
            {
                int64_t integer = lua_tointeger(state, -2);
                *self = new FixedPoint(integer);
            }

            luaL_setmetatable(state, "FixedPoint");

            return 1;
        };

        auto destructor = [](lua_State* state) -> int {
            delete *static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            return 0;
        };

        auto fromRawValue = [](lua_State* state) -> int {
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            int64_t val = luaL_checkinteger(state, 1);
            *ret = new FixedPoint();
            **ret = FixedPoint::fromRawValue(val);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto rawValue = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushinteger(state, (*self)->rawValue());
            return 1;
        };

        auto intPart = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushinteger(state, (*self)->intPart());
            return 1;
        };

        auto fractionPart = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = (*self)->fractionPart();
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto round = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushinteger(state, (*self)->round());
            return 1;
        };

        auto floor = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushinteger(state, (*self)->floor());
            return 1;
        };

        auto ceil = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushinteger(state, (*self)->ceil());
            return 1;
        };

        auto str = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushstring(state, (*self)->str().c_str());
            return 1;
        };

        /*auto toDouble = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            lua_pushnumber(state, (*self)->toDouble());
            return 1;
            };*/

        auto maxVal = [](lua_State* state) -> int {
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::maxVal();
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto minVal = [](lua_State* state) -> int {
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::minVal();
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto sqrt = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = (*self)->sqrt();
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto abs = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = (*self)->abs();
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto atan2 = [](lua_State* state) -> int {
            FixedPoint** y = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** x = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::atan2(**y, **x);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto sin = [](lua_State* state) -> int {
            FixedPoint** rad = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::sin(**rad);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto cos = [](lua_State* state) -> int {
            FixedPoint** rad = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::cos(**rad);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto atan2_degrees = [](lua_State* state) -> int {
            FixedPoint** y = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** x = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::atan2_degrees(**y, **x);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto sin_degrees = [](lua_State* state) -> int {
            FixedPoint** deg = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::sin_degrees(**deg);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto cos_degrees = [](lua_State* state) -> int {
            FixedPoint** deg = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = FixedPoint::cos_degrees(**deg);
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto add = [](lua_State* state) -> int {
            int64_t intOp = 0;
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            if (lua_isinteger(state, -2))
            {
                intOp = lua_tointeger(state, -2);
                **ret = (*self)->operator+(intOp);
            }

            else
            {
                FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
                **ret = (*self)->operator+(**other);
            }

            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto sub = [](lua_State* state) -> int {
            int64_t intOp = 0;
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            if (lua_isinteger(state, -2))
            {
                intOp = lua_tointeger(state, -2);
                **ret = (*self)->operator-(intOp);
            }

            else
            {
                FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
                **ret = (*self)->operator-(**other);
            }

            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto mul = [](lua_State* state) -> int {
            int64_t intOp = 0;
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            if (lua_isinteger(state, -2))
            {
                intOp = lua_tointeger(state, -2);
                **ret = (*self)->operator*(intOp);
            }

            else
            {
                FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
                **ret = (*self)->operator*(**other);
            }

            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto div = [](lua_State* state) -> int {
            int64_t intOp = 0;
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            if (lua_isinteger(state, -2))
            {
                intOp = lua_tointeger(state, -2);
                **ret = (*self)->operator/(intOp);
            }

            else
            {
                FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
                **ret = (*self)->operator/(**other);
            }

            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto unm = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** ret = static_cast<FixedPoint**>(lua_newuserdata(state, sizeof(FixedPoint)));
            *ret = new FixedPoint();
            **ret = (*self)->operator-();
            luaL_setmetatable(state, "FixedPoint");
            return 1;
        };

        auto eq = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
            lua_pushboolean(state, **self == **other);
            return 1;
        };

        auto lt = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
            lua_pushboolean(state, (*self)->operator<(**other));
            return 1;
        };

        auto le = [](lua_State* state) -> int {
            FixedPoint** self = static_cast<FixedPoint**>(luaL_checkudata(state, 1, "FixedPoint"));
            FixedPoint** other = static_cast<FixedPoint**>(luaL_checkudata(state, 2, "FixedPoint"));
            lua_pushboolean(state, (*self)->operator<=(**other));
            return 1;
        };

        lua_newtable(mState);

        // constructor and static functions
        lua_pushcfunction(mState, constructor);
        lua_setfield(mState, -2, "new");
        lua_pushcfunction(mState, fromRawValue);
        lua_setfield(mState, -2, "fromRawValue");
        lua_pushcfunction(mState, maxVal);
        lua_setfield(mState, -2, "maxVal");
        lua_pushcfunction(mState, minVal);
        lua_setfield(mState, -2, "minVal");
        lua_pushcfunction(mState, atan2);
        lua_setfield(mState, -2, "atan2");
        lua_pushcfunction(mState, sin);
        lua_setfield(mState, -2, "sin");
        lua_pushcfunction(mState, cos);
        lua_setfield(mState, -2, "cos");
        lua_pushcfunction(mState, atan2_degrees);
        lua_setfield(mState, -2, "atan2_degrees");
        lua_pushcfunction(mState, sin_degrees);
        lua_setfield(mState, -2, "sin_degrees");
        lua_pushcfunction(mState, cos_degrees);
        lua_setfield(mState, -2, "cos_degrees");
        lua_setglobal(mState, "FixedPoint");

        // member functions
        lua_pushvalue(mState, -1);
        lua_setfield(mState, -2, "__index");
        lua_pushcfunction(mState, rawValue);
        lua_setfield(mState, -2, "rawValue");
        lua_pushcfunction(mState, intPart);
        lua_setfield(mState, -2, "intPart");
        lua_pushcfunction(mState, fractionPart);
        lua_setfield(mState, -2, "fractionPart");
        lua_pushcfunction(mState, str);
        lua_setfield(mState, -2, "str");
        // lua_pushcfunction(mState, toDouble);
        // lua_setfield(mState, -2, "toDouble");
        lua_pushcfunction(mState, round);
        lua_setfield(mState, -2, "round");
        lua_pushcfunction(mState, floor);
        lua_setfield(mState, -2, "floor");
        lua_pushcfunction(mState, ceil);
        lua_setfield(mState, -2, "ceil");
        lua_pushcfunction(mState, sqrt);
        lua_setfield(mState, -2, "sqrt");
        lua_pushcfunction(mState, abs);
        lua_setfield(mState, -2, "abs");

        // operators
        lua_pushcfunction(mState, add);
        lua_setfield(mState, -2, "__add");
        lua_pushcfunction(mState, sub);
        lua_setfield(mState, -2, "__sub");
        lua_pushcfunction(mState, mul);
        lua_setfield(mState, -2, "__mul");
        lua_pushcfunction(mState, div);
        lua_setfield(mState, -2, "__div");
        lua_pushcfunction(mState, unm); // unary - (negation)
        lua_setfield(mState, -2, "__unm");
        lua_pushcfunction(mState, eq);
        lua_setfield(mState, -2, "__eq");
        lua_pushcfunction(mState, lt);
        lua_setfield(mState, -2, "__lt");
        lua_pushcfunction(mState, le);
        lua_setfield(mState, -2, "__le");

        lua_pushcfunction(mState, destructor);
        lua_setfield(mState, -2, "__gc");
    }
}

constexpr int64_t FixedPoint::scalingFactorPowerOf10;
constexpr int64_t FixedPoint::scalingFactor;

FixedPoint FixedPoint::PI("3.14159265359");
FixedPoint FixedPoint::epsilon = fromRawValue(1);

static inline int64_t i64abs(int64_t i)
{
    return i >= 0 ? i : -i; // not using std::abs because of a libc++ bug https://github.com/Project-OSRM/osrm-backend/issues/1000
}

static inline int64_t muldiv(int64_t n1, int64_t n2, int64_t d)
{
    int64_t ret;
    // Optimisation : Only use 128bit if 64bit will overflow.
    if (n2 == 0 || i64abs(n1) <= INT64_MAX / i64abs(n2))
        ret = n1 * n2 / d;
    else
        ret = int64_t(absl::int128(n1) * n2 / d);
    return ret;
}

FixedPoint::FixedPoint(int64_t integerValue) { *this = fromRawValue(integerValue * FixedPoint::scalingFactor); }

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
