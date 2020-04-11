#pragma once

#include <misc/assert.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/Vector.h"

namespace Script
{
    class LuaScript
    {
        lua_State* mState;
        static std::shared_ptr<LuaScript> mInstance;

    public:
        ~LuaScript();

        template <typename T> void registerType();

        template <typename Func> void registerGlobalFunction(const std::string& functionName, Func&& func)
        {
            luabridge::getGlobalNamespace(mState).addFunction(functionName.c_str(), func);
        }

        template <typename T> void pushGlobalObject(T& obj, const char* varName)
        {
            registerType<T>();
            luabridge::push(mState, obj);
            lua_setglobal(mState, varName);
        }

        luabridge::LuaRef get(const std::string& variable)
        {
            return luabridge::getGlobal(mState, variable.c_str());
        }

        template <typename T> T get(const std::string& variable)
        {
            return get(variable).cast<T>();
        }

        void runScript(const std::string& path);
        void eval(const char* script);

        static std::shared_ptr<LuaScript> getInstance()
        {
            if (!mInstance)
                mInstance = std::shared_ptr<LuaScript>(new LuaScript());

            return mInstance;
        }

        operator lua_State*() { return mState; }

    private:
        LuaScript();
    };
}
