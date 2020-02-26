#include "console.h"
#include <script/luascript.h>

#include <filesystem/resolver.h>
#include <misc/fixedpoint.h>
#include <misc/stringops.h>

namespace FAGui
{
    static auto oldCout = std::cout.rdbuf();
#ifdef _WIN32
    static const char* nullFile = "nul";
#else
    static const char* nullFile = "/dev/null";
#endif
    static char consoleStdoutBuffer[BUFSIZ];
    std::unique_ptr<Console> Console::mInstance = nullptr;
    static std::stringstream consoleCout;

    Console::Console() : mBuffer({}), bufferLen(0), inputLen(0), mScript(Script::LuaScript::getInstance()), mCommandsPath("resources/commands/")
    {
        std::cout.rdbuf(consoleCout.rdbuf());
        freopen(nullFile, "a", stdout);
        setbuf(stdout, consoleStdoutBuffer);
    }

    Console::~Console()
    {
        std::cout.rdbuf(oldCout);
        fclose(stdout);
    }

    std::unique_ptr<Console>& Console::getInstance()
    {
        if (!mInstance)
        {
            mInstance = std::unique_ptr<Console>(new Console());
        }

        return mInstance;
    }

    void Console::appendStdOut()
    {
        if (auto size = strlen(consoleStdoutBuffer); size > 0)
        {
            mBuffer.append(consoleStdoutBuffer, size);
            bufferLen = mBuffer.length();
            std::memset(consoleStdoutBuffer, '\0', BUFSIZ);
        }

        if (consoleCout.tellp() > 0)
        {
            mBuffer += consoleCout.str();
            bufferLen = mBuffer.length();
            consoleCout.str({});
        }
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
            std::string msg = ">> ";
            msg.append(consoleStdoutBuffer, strlen(consoleStdoutBuffer));
            std::memset(consoleStdoutBuffer, '\0', BUFSIZ);
            std::cout << msg;
        }

        else
        {
            auto args = Misc::StringUtils::split(command, ' '); // TODO: parse args and pass them to the script
            filesystem::path scriptPath = filesystem::resolver().resolve(mCommandsPath / (args[0] + ".lua"));
            if (scriptPath.exists())
            {
                mScript->runScript(scriptPath.str());
                std::string msg = ">> ";
                msg.append(consoleStdoutBuffer, strlen(consoleStdoutBuffer));
                std::memset(consoleStdoutBuffer, '\0', BUFSIZ);
                std::cout << msg;
            }

            else
            {
                std::string msg = ">> Command '" + command + "' not found\n";
                std::cout << msg;
            }
        }
    }
}
