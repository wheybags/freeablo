#include "console.h"
#include "luascript.h"

#include <filesystem/resolver.h>
#include <misc/stringops.h>

namespace Script
{
    static std::string luaStdOut;
    std::unique_ptr<Console> Console::mInstance = nullptr;

    Console::Console() : mBuffer({}), bufferLen(0), inputLen(0), mScript(LuaScript::getInstance()), mCommandsPath("resources/commands/")
    {
        // Hack. Redirects the print output to the console.
        mScript->registerGlobalFunction("print", [](lua_State* state) -> int {
            int n = lua_gettop(state);
            if (n >= 1)
            {
                for (int i = n; i > 0; --i)
                {
                    const char* str = lua_tostring(state, -i);
                    luaStdOut += str;
                }
            }

            else
            {
                luaStdOut = "";
            }
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

    void Console::inputCommited()
    {
        std::string command = "";
        command.append(mInput, inputLen);
        mInput[inputLen] = '\n';
        ++inputLen;
        mBuffer.append(mInput, inputLen);
        bufferLen += inputLen;
        inputLen = 0;

        auto args = Misc::StringUtils::split(command, ' '); // TODO: parse args and pass them to the script
        filesystem::path scriptPath = filesystem::resolver().resolve(mCommandsPath / (args[0] + ".lua"));
        if (scriptPath.exists())
        {
            mScript->runScript(scriptPath.str());
            std::string msg = luaStdOut.empty() ? ">> nil\n" : (">> \"" + luaStdOut + "\"\n");
            mBuffer += msg;
            bufferLen += msg.length();
            luaStdOut = "";
        }

        else
        {
            std::string msg = ">> Command '" + command + "' not found\n";
            mBuffer += msg;
            bufferLen += msg.length();
            luaStdOut = "";
        }
    }
}
