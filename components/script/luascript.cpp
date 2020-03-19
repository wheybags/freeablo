#include "luascript.h"

#include <type_traits>
#include <vector>

namespace Script
{
    std::shared_ptr<LuaScript> LuaScript::mInstance = nullptr;

    LuaScript::LuaScript() : mState(luaL_newstate()) { luaL_openlibs(mState); }

    LuaScript::~LuaScript()
    {
        if (mState)
            lua_close(mState);
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

    template <> float LuaScript::get<float>(const std::string &variable) = delete;
    template <> double LuaScript::get<double>(const std::string &variable) = delete;
}
