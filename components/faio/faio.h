#ifndef FAIO_H
#define FAIO_H

#include <stdio.h>
#include <string>

#include <StormLib.h>

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
                HANDLE mpqFile;
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
            friend size_t FAsize(FAFile* stream);
    };
    
    FAFile* FAfopen(const std::string& filename);
    size_t FAfread(void * ptr, size_t size, size_t count, FAFile* stream);
    int FAfclose(FAFile* stream); 
    int FAfseek (FAFile* stream, size_t offset, int origin);
    size_t FAsize(FAFile* stream);

}

#endif 
