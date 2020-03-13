#include "console.h"
#include <script/luascript.h>

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

    Console::Console() : mBuffer({}), bufferLen(0), inputLen(0), mScript({})
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
            mInstance = std::unique_ptr<Console>(new Console());

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

        mScript.eval(command.c_str());
        std::string msg = ">> ";
        msg.append(consoleStdoutBuffer, strlen(consoleStdoutBuffer));
        std::memset(consoleStdoutBuffer, '\0', BUFSIZ);
        std::cout << msg;
    }
}
