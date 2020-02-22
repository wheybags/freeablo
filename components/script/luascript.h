#pragma once

#include <misc/assert.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

namespace Script
{
    class LuaScript
    {
        lua_State* mState;
        int mLevel;
        static std::unique_ptr<LuaScript> mInstance;

    public:
        ~LuaScript();

        static std::unique_ptr<LuaScript>& getInstance();

        void printError(const std::string& variable, const std::string& reason);

        template <typename T> void registerGlobalType();

        template <typename Func> void registerGlobalFunction(const std::string& functionName, Func func)
        {
            lua_pushcfunction(mState, func);
            lua_setglobal(mState, functionName.c_str());
        }

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
        void eval(const char* script);

    private:
        LuaScript();

        bool luaGetToStack(const std::string& variable);

        void clean();

        template <typename T> T luaGet(const std::string& variable = "");

        template <typename T> T luaGetDefault() { return {}; }
    };
}
