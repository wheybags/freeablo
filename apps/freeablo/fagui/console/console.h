#pragma once

#include <script/luascript.h>

#include <filesystem/path.h>
#include <memory>
#include <string>

namespace FAGui
{
    class Console
    {
        std::string mBuffer;
        int bufferLen;
        static constexpr size_t inputSize = 512;
        char mInput[inputSize];
        int inputLen;
        std::shared_ptr<Script::LuaScript> mScript;
        static std::shared_ptr<Console> mInstance;

    public:
        static std::shared_ptr<Console> getInstance();
        ~Console();

        char* getInput() { return mInput; };
        std::string& getBuffer() { return mBuffer; };
        char* getBufferPtr() const { return const_cast<char*>(mBuffer.c_str()); }
        int* getBufferLen() { return &bufferLen; }
        int* getInputLen() { return &inputLen; }
        constexpr size_t getInputSize() const { return inputSize; }

        void appendStdOut();
        void inputCommited();

    private:
        Console();
    };
}
