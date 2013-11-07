#ifndef FAIO_H
#define FAIO_H

#include <stdio.h>
#include <string>

namespace FAIO
{
    struct FAFile
    {
        union FAFileUnion
        {
            FILE* plainFile;
        } data;
    
        enum FAFileMode
        {
            PlainFile
        } mode;

    };
    
    FAFile* FAfopen(const std::string& filename, const std::string& mode);
    size_t FAfread(void * ptr, size_t size, size_t count, FAFile* stream);
    int FAfclose(FAFile* stream); 

}

#endif 
