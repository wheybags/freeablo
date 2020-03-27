#pragma once
#include "faio.h"

namespace FAIO
{

    /// this is just a RAII wrapper class for faio api functions.
    class FAFileObject
    {

    public:
        explicit FAFileObject(const std::string& pathFile);
        FAFileObject(const FAFileObject&) = delete;
        ~FAFileObject();

    protected:
        FAIO::FAFile* faFile;

    public:
        // wrapper api for faio
        bool isValid();
        bool exists(const std::string& filename);
        size_t FAfread(void* ptr, size_t size, size_t count);
        int FAfseek(size_t offset, int origin);
        size_t FAftell();
        size_t FAsize();

        std::vector<uint8_t> readAll();

        uint32_t read32();
        uint16_t read16();
        uint8_t read8();
        std::string readCString(size_t ptr);
        std::string readCStringFromWin32Binary(size_t ptr, size_t offset);

        static void quit();
    };
}
