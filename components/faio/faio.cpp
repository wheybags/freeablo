#include "faio.h"

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <StormLib.h>

namespace FAIO
{
    FAFile::FAFile(){}

    // StormLib needs paths with windows style \'s
    std::string getStormLibPath(const bfs::path& path)
    {
        std::string retval = "";
        
        for(bfs::path::iterator it = path.begin(); it != path.end(); ++it)
        {
            retval += it->string() + "\\";
        }
        
        retval = retval.substr(0, retval.size()-1);

        std::cout << retval << std::endl;
        
        return retval;
    }

    HANDLE diabdat = NULL;

    FAFile* FAfopen(const std::string& filename)
    {
        bfs::path path(filename);
        path.make_preferred();

        if(!bfs::exists(filename))
        {
            int nError = ERROR_SUCCESS;

            if(diabdat == NULL && !SFileOpenArchive("DIABDAT.MPQ", 0, 0, &diabdat))
                nError = GetLastError();

            if(nError != ERROR_SUCCESS)
            {
                std::cerr << "Failed to open DIABDAT.MPQ" << std::endl;
                return NULL;
            }
            
            std::string stormPath = getStormLibPath(path);

            if(!SFileHasFile(diabdat, stormPath.c_str()))
            {
                std::cerr << "File " << path << " not found" << std::endl;
                return NULL;
            }
            
            FAFile* file = new FAFile();
            file->data.mpqFile = malloc(sizeof(HANDLE));

            if(!SFileOpenFileEx(diabdat, stormPath.c_str(), 0, (HANDLE*)file->data.mpqFile))
            {
                std::cerr << "Failed to open " << filename << " in DIABDAT.MPQ";
                delete file;
                return NULL;
            }

            file->mode = FAFile::MPQFile;

            return file;
        }
        else
        {
            FILE* plainFile = fopen(filename.c_str(), "rb");
            if(plainFile == NULL)
                return NULL;

            FAFile* file = new FAFile();
            file->mode = FAFile::PlainFile; 
            file->data.plainFile.file = plainFile;
            file->data.plainFile.filename = new std::string(filename);

            return file;
        }
    }

    size_t FAfread(void * ptr, size_t size, size_t count, FAFile* stream)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return fread(ptr, size, count, stream->data.plainFile.file);
            
            case FAFile::MPQFile:
                DWORD dwBytes = 1;
                SFileReadFile(*((HANDLE*)stream->data.mpqFile), ptr, size*count, &dwBytes, NULL);

                return dwBytes;
        }
    }
    
    int FAfclose(FAFile* stream)
    {
        int retval = 0;

        switch(stream->mode)
        {
            case FAFile::PlainFile:
                delete stream->data.plainFile.filename;
                return fclose(stream->data.plainFile.file);

            case FAFile::MPQFile:
                int res = SFileCloseFile(*((HANDLE*)stream->data.mpqFile));
                free(stream->data.mpqFile);

                if(res != 0)
                    retval = EOF;
        }

        delete stream;

        return retval;
    }

    int FAfseek (FAFile* stream, size_t offset, int origin)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return fseek(stream->data.plainFile.file, offset, origin);
            
            case FAFile::MPQFile:
                DWORD moveMethod;
                
                switch(origin)
                {
                    case SEEK_SET:
                        moveMethod = FILE_BEGIN;
                        break;

                    case SEEK_CUR:
                        moveMethod = FILE_CURRENT;
                        break;

                    case SEEK_END:
                        moveMethod = FILE_END;
                        break;
                    
                    default:
                        return 1; // error, incorrect origin
                }

                SFileSetFilePointer(*((HANDLE*)stream->data.mpqFile), offset, NULL, moveMethod);
                int nError = ERROR_SUCCESS;
                nError = GetLastError();

                return nError != ERROR_SUCCESS;
        }
    }

    size_t FAsize(FAFile* stream)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return bfs::file_size(*(stream->data.plainFile.filename));

            case FAFile::MPQFile:
                return SFileGetFileSize(*((HANDLE*)stream->data.mpqFile), NULL);
        }
    }
}
