#ifndef FAIO_H
#define FAIO_H

#include <stdio.h>
#include <stdint.h>

#include <string>
#include <vector>

// The functions in this header are designed to behave roughly like the normal fopen, fread family.
// The difference is, if FAfopen is called on a file that doesn't exist, it will try to use StormLib
// to open it in the MPQ file DIABDAT.MPQ.

namespace FAIO
{
    // A FILE* like container for either a normal FILE*, or a StormLib HANDLE
    struct FAFile
    {
        private:
            union FAFileUnion
            {
                struct
                {
                    FILE* file;
                    std::string* filename;
                } plainFile;
                void* mpqFile; // This is a pointer to a StormLib HANDLE type, I jist didn't want to #include StormLib here
            } data;
        
            enum FAFileMode
            {
                PlainFile,
                MPQFile
            } mode;

            FAFile();

            friend FAFile* FAfopen(const std::string& filename);
            friend size_t FAfread(void * ptr, size_t size, size_t count, FAFile* stream);
            friend int FAfclose(FAFile* stream); 
            friend int FAfseek (FAFile* stream, size_t offset, int origin);
            friend size_t FAftell(FAFile* stream);
            friend size_t FAsize(FAFile* stream);
    };

    bool init(const std::string pathMPQ = "DIABDAT.MPQ", const std::string listFile = "");
    std::vector<std::string> listMpqFiles(const std::string& pattern);

    void quit();

    bool exists(const std::string& filename);
    FAFile* FAfopen(const std::string& filename);
    size_t FAfread(void * ptr, size_t size, size_t count, FAFile* stream);
    int FAfclose(FAFile* stream); 
    int FAfseek (FAFile* stream, size_t offset, int origin);
    size_t FAftell(FAFile* stream);
    size_t FAsize(FAFile* stream);

    uint32_t read32(FAFile* file);
    uint16_t read16(FAFile* file);
    uint8_t read8(FAFile* file);
    std::string readCString(FAFile* file, size_t ptr);
	std::string readCStringFromWin32Binary(FAFile* file, size_t ptr, size_t offset);
    std::string getMPQFileName();
}

#endif 
