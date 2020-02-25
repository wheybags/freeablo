#include "console.h"
#include <script/luascript.h>

#include <filesystem/resolver.h>
#include <misc/fixedpoint.h>
#include <misc/stringops.h>

namespace FAGui
{
    static std::string luaStdOut;
    std::unique_ptr<Console> Console::mInstance = nullptr;

    Console::Console() : mBuffer({}), bufferLen(0), inputLen(0), mScript(Script::LuaScript::getInstance()), mCommandsPath("resources/commands/")
    {
        // Hack. Redirects the print output to the console.
        mScript->registerGlobalFunction("print", [](lua_State* state) -> int {
            int n = lua_gettop(state);
            luaStdOut += "\"";
            if (n >= 1)
            {
                for (int i = n; i > 0; --i)
                {
                    if (lua_isboolean(state, -i))
                    {
                        luaStdOut += lua_toboolean(state, -i) != 0 ? "true" : "false";
                    }

                    else if (lua_isnumber(state, -i))
                    {
                        luaStdOut += luaL_checknumber(state, i).str();
                    }

                    else
                    {
                        const char* str = luaL_checkstring(state, i);
                        luaStdOut += str;
                    }
                }
            }

            luaStdOut += "\"\n";
            return 0;
        });
    }

    std::unique_ptr<Console>& Console::getInstance()
    {
        if (!mInstance)
        {
            mInstance = std::unique_ptr<Console>(new Console());
        }

        return mInstance;
    }

    void Console::appendStdOut(const std::string& msg)
    {
        mBuffer += msg;
        bufferLen += msg.length();
        luaStdOut = "";
    }

    void Console::inputCommited()
    {
        std::string command = "";
        command.append(mInput, inputLen);
        mInput[inputLen] = '\n';
        ++inputLen;
        mBuffer.append(mInput, inputLen);
        bufferLen += inputLen;
        inputLen = 0;

        if (Misc::StringUtils::ciStartsWith(command, "eval"))
        {
            std::string arg = command.substr(4, command.length() - 4);
            mScript->eval(arg.c_str());
            std::string msg = ">> " + luaStdOut;
            appendStdOut(msg);
        }

        else
        {
            auto args = Misc::StringUtils::split(command, ' '); // TODO: parse args and pass them to the script
            filesystem::path scriptPath = filesystem::resolver().resolve(mCommandsPath / (args[0] + ".lua"));
            if (scriptPath.exists())
            {
                mScript->runScript(scriptPath.str());
                std::string msg = luaStdOut.empty() ? ">> nil\n" : (">> " + luaStdOut);
                appendStdOut(msg);
            }

            else
            {
                std::string msg = ">> Command '" + command + "' not found\n";
                appendStdOut(msg);
            }
        }
    }
}
