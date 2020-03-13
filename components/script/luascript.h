#pragma once

#include <misc/assert.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

namespace Script
{
    class LuaScript
    {
        lua_State* mState;

    public:
        LuaScript();
        ~LuaScript();

        template <typename T> void registerGlobalType();

        template <typename Func> void registerGlobalFunction(const std::string& functionName, Func&& func)
        {
            lua_pushcfunction(mState, func);
            lua_setglobal(mState, functionName.c_str());
        }

        template <typename T> void pushObject(T& obj, const char* varName)
        {
            static bool registered = false;
            if (!registered)
            {
                registerGlobalType<T>();
                registered = true;
            }

            T** udata = reinterpret_cast<T**>(lua_newuserdata(mState, sizeof(T*)));
            (*udata) = &obj;
            const std::string typeName = typeid(T).name();
            luaL_setmetatable(mState, typeName.substr(1).c_str());
            lua_setglobal(mState, varName);
        }

        template <typename T> T get(const std::string& variable)
        {
            release_assert(mState);
            int level;

            T result;
            if (luaGetToStack(variable, level))
                result = luaGet<T>(variable);

            else
                result = luaGetDefault<T>();

            lua_pop(mState, level + 1);
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
        bool luaGetToStack(const std::string& variable, int& level);

        void clean();

        void printError(const std::string& variable, const std::string& reason);

        template <typename T> T luaGet(const std::string& variable = "");

        template <typename T> T luaGetDefault() { return {}; }

        template <typename Func> void pushFunction(Func&& f, const char* funcName)
        {
            lua_pushcfunction(mState, f);
            lua_setfield(mState, -2, funcName);
        }
    };
}
