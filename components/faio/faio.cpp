#include "faio.h"

namespace FAIO
{

    FAFile* FAfopen(const std::string& filename, const std::string& mode)
    {
        FILE* plainFile = fopen(filename.c_str(), mode.c_str());
        if(plainFile == NULL)
            return NULL;

        FAFile* file = new FAFile();
        file->mode = FAFile::PlainFile; 
        file->data.plainFile = plainFile;

        return file;
    }

    size_t FAfread(void * ptr, size_t size, size_t count, FAFile* stream)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return fread(ptr, size, count, stream->data.plainFile);
        }
    }
    
    int FAfclose(FAFile* stream)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return fclose(stream->data.plainFile);
        }
    }
}
