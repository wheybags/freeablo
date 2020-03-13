#include "luascript.h"

#include <type_traits>
#include <vector>

namespace Script
{
    LuaScript::LuaScript() : mState(luaL_newstate()) { luaL_openlibs(mState); }

    LuaScript::~LuaScript()
    {
        if (mState)
            lua_close(mState);
    }

    void LuaScript::printError(const std::string& variable, const std::string& reason)
    {
#ifdef NDEBUG
        (void)variable;
        (void)reason;
#else
        std::cerr << "Can't get variable " << variable << ".\nReason: " << reason << "\n";
#endif
    }

    template <> std::string LuaScript::luaGetDefault<std::string>() { return "null"; }

    template <> bool LuaScript::luaGet<bool>(const std::string& variable)
    {
        (void)variable;
        return static_cast<bool>(lua_toboolean(mState, -1));
    }
    template <> int LuaScript::luaGet<int>(const std::string& variable)
    {
        (void)variable;
        return static_cast<int>(lua_tointeger(mState, -1));
    }
    template <> int64_t LuaScript::luaGet<int64_t>(const std::string& variable)
    {
        (void)variable;
        return static_cast<int64_t>(lua_tointeger(mState, -1));
    }

    template <> FixedPoint LuaScript::luaGet<FixedPoint>(const std::string& variable)
    {
        (void)variable;
        return static_cast<FixedPoint>(luaL_checknumber(mState, 1));
    }

    template <> std::string LuaScript::luaGet<std::string>(const std::string& variable)
    {
        std::string ret = "null";

        release_assert(lua_isstring(mState, -1));

        if (lua_isstring(mState, -1))
            ret = std::string(lua_tostring(mState, -1));
        else
            printError(variable, "Not a string");

        return ret;
    }

    bool LuaScript::luaGetToStack(const std::string& variable, int& level)
    {
        level = 0;
        std::string var{};

        for (char c : variable)
        {
            if (c == '.')
            {
                if (level == 0)
                {
                    lua_getglobal(mState, var.c_str());
                }

                else
                {
                    lua_getfield(mState, -1, var.c_str());
                }

                if (lua_isnil(mState, -1))
                {
                    printError(variable, var + " is not defined");
                    return false;
                }

                else
                {
                    var = "";
                    ++level;
                }
            }

            else
            {
                var += c;
            }
        }

        if (level == 0)
        {
            lua_getglobal(mState, var.c_str());
        }

        else
        {
            lua_getfield(mState, -1, var.c_str());
        }

        if (lua_isnil(mState, -1))
        {
            printError(variable, var + " is not defined");
            return false;
        }

        return true;
    }

    void LuaScript::clean()
    {
        int n = lua_gettop(mState);
        lua_pop(mState, n);
    }

    void LuaScript::runScript(const std::string& path)
    {
        lua_settop(mState, 0);

        if (luaL_dofile(mState, path.c_str()))
        {
            std::cerr << "Lua error: " << lua_tostring(mState, -1) << "\n";
            lua_pop(mState, 1);
            exit(1);
        }
    }

    void LuaScript::eval(const char* script)
    {
        lua_settop(mState, 0);

        if (luaL_dostring(mState, script))
        {
            std::cout << "Lua error: " << lua_tostring(mState, -1) << "\n";
            lua_pop(mState, 1);
        }
    }
}
