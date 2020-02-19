#pragma once

#include <misc/assert.h>

#include <iostream>
#include <string>
#include <vector>

#include <lua.hpp>

namespace Script
{
    class LuaScript
    {
        lua_State* mState;
        int mLevel;

    public:
        LuaScript();
        ~LuaScript();

        void printError(const std::string& variable, const std::string& reason);

        template <typename T> void registerType(const std::string& metatable_name);

        template <typename T> T get(const std::string& variable)
        {
            release_assert(mState);

            T result;
            if (luaGetToStack(variable))
            {
                result = luaGet<T>(variable);
            }

            else
            {
                result = luaGetDefault<T>();
            }

            lua_pop(mState, mLevel + 1);
            return result;
        }

        template <typename T> std::vector<T> getVector(const std::string& variable)
        {
            std::vector<T> ret;
            lua_getglobal(mState, variable.c_str());
            if (lua_isnil(mState, -1))
            {
                return {};
            }

            lua_pushnil(mState);

            while (lua_next(mState, -2))
            {
                T aux;
                if constexpr (std::is_same<T, std::string>::value)
                {
                    ret.push_back(lua_tostring(mState, -1));
                }

                else
                {
                    ret.push_back(static_cast<T>(lua_tonumber(mState, -1)));
                }

                lua_pop(mState, 1);
            }

            clean();
            return ret;
        }

        void runScript(const std::string& path);

    private:
        bool luaGetToStack(const std::string& variable);

        void clean();

        template <typename T> T luaGet(const std::string& variable = "")
        {
            release_assert(lua_isnumber(mState, -1) && "Not a number");

            return static_cast<T>(lua_tonumber(mState, -1));
        };

        template <typename T> T luaGetDefault() { return {}; }
    };
}
