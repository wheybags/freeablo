#include "faio.h"

#include <iostream>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace bfs = boost::filesystem;


// We don't want warnings from StormLibs headers
#include <misc/disablewarn.h>
    #include <StormLib.h>
#include <misc/enablewarn.h>

namespace FAIO
{
    FAFile::FAFile(){}

    const std::string DIABDAT_MPQ = "DIABDAT.MPQ";
    std::mutex m;

    // StormLib needs paths with windows style \'s
    std::string getStormLibPath(const bfs::path& path)
    {
        std::string retval = "";

        for(bfs::path::iterator it = path.begin(); it != path.end(); ++it)
        {
            retval += it->string() + "\\";
        }

        retval = retval.substr(0, retval.size()-1);

        return retval;
    }

    HANDLE diabdat = NULL;

    bool init(const std::string pathMPQ, const std::string listFile)
    {
        if(pathMPQ.empty())
        {
            std::cout << "skipping stormlib init - won't be able to read files in MPQ archives" << std::endl;
            return true;
        }

		const bool success = SFileOpenArchive(pathMPQ.c_str(), 0, STREAM_FLAG_READ_ONLY, &diabdat);

        if (!success)
        {
			std::cerr << "Failed to open " << pathMPQ.c_str() << " with error " << GetLastError() << std::endl;
        }

        if(!listFile.empty())
            SFileAddListFile(diabdat, listFile.c_str());

        return success;
    }

    std::vector<std::string> listMpqFiles(const std::string& pattern)
    {
        SFILE_FIND_DATA findFileData;
        HANDLE findHandle = SFileFindFirstFile(diabdat, pattern.c_str(), &findFileData, NULL);

        std::vector<std::string> results;

        results.push_back(findFileData.cFileName);

        while (SFileFindNextFile(findHandle, &findFileData))
        {
            results.push_back(findFileData.cFileName);
        }

        SFileFindClose(findHandle);

        return results;
    }

    void quit()
    {
        if (NULL != diabdat)
        {
            SFileCloseArchive(diabdat);
        }
    }

    bool exists(const std::string& filename)
    {
        bfs::path path(filename);
        path.make_preferred();

        if(bfs::exists(filename))
            return true;

        std::lock_guard<std::mutex> lock(m);
        std::string stormPath = getStormLibPath(path);

        return SFileHasFile(diabdat, stormPath.c_str());
    }

    FAFile* FAfopen(const std::string& filename)
    {
        bfs::path path(filename);
        path.make_preferred();

        if(!bfs::exists(filename))
        {
            std::lock_guard<std::mutex> lock(m);
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
                std::cerr << "Failed to open " << filename << " in " << DIABDAT_MPQ;
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
            {
                std::lock_guard<std::mutex> lock(m);

                DWORD dwBytes = 1;
                if(!SFileReadFile(*((HANDLE*)stream->data.mpqFile), ptr, size*count, &dwBytes, NULL))
                {
                    int errorCode = GetLastError();

                    // if the error code is ERROR_HANDLE_EOF, it's not really an error,
                    // we just requested a read that goes over the end of the file.
                    // The normal fread behaviour in this case is to truncate the read to fit within
                    // the file, and return the actual number of bytes read, which we do,
                    // so there is no need to print an error message.
                    if(errorCode != ERROR_HANDLE_EOF)
                        std::cout << "Error reading from file, error code: " << errorCode << std::endl;
                }

                return dwBytes;
            }
        }
        return 0;
    }

    int FAfclose(FAFile* stream)
    {
        int retval = 0;

        switch(stream->mode)
        {
            case FAFile::PlainFile:
            {
                delete stream->data.plainFile.filename;
                retval = fclose(stream->data.plainFile.file);
                break;
            }

            case FAFile::MPQFile:
            {
                std::lock_guard<std::mutex> lock(m);

                int res = SFileCloseFile(*((HANDLE*)stream->data.mpqFile));
                free(stream->data.mpqFile);

                if(res != 0)
                    retval = EOF;

                break;
            }
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
            {
                std::lock_guard<std::mutex> lock(m);
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

        return 0;
    }

    size_t FAftell(FAFile* stream)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return ftell(stream->data.plainFile.file);

            case FAFile::MPQFile:
            {
                std::lock_guard<std::mutex> lock(m);
                return SFileSetFilePointer(*((HANDLE*)stream->data.mpqFile), 0, NULL, FILE_CURRENT);
            }

            default:
                return 0;
        }
    }

    size_t FAsize(FAFile* stream)
    {
        switch(stream->mode)
        {
            case FAFile::PlainFile:
                return static_cast<size_t> (bfs::file_size(*(stream->data.plainFile.filename)));

            case FAFile::MPQFile:
            {
                std::lock_guard<std::mutex> lock(m);
                return SFileGetFileSize(*((HANDLE*)stream->data.mpqFile), NULL);
            }
        }

        return 0;
    }

    uint32_t read32(FAFile* file)
    {
        uint32_t tmp;
        FAfread(&tmp, 4, 1, file);
        return tmp;
    }

    uint16_t read16(FAFile* file)
    {
        uint16_t tmp;
        FAfread(&tmp, 2, 1, file);
        return tmp;
    }

    uint8_t read8(FAFile* file)
    {
        uint8_t tmp;
        FAfread(&tmp, 1, 1, file);
        return tmp;
    }

    std::string readCString(FAFile* file, size_t ptr)
    {
        std::string retval = "";

        if(ptr)
        {
            FAfseek(file, ptr, SEEK_SET);
            char c = 0;

			size_t bytesRead = FAfread(&c, 1, 1, file);

            while(c != '\0' && bytesRead)
            {
                retval += c;
                bytesRead = FAfread(&c, 1, 1, file);
            }
        }

        return retval;
    }

	std::string readCStringFromWin32Binary(FAFile* file, size_t ptr, size_t offset)
	{
		if (ptr)
			return readCString(file, ptr - offset);

		return "";
	}

    std::string getMPQFileName()
    {
		bfs::directory_iterator end;
		for(bfs::directory_iterator entry(".") ; entry != end; entry++)
		{
			if (!bfs::is_directory(*entry))
			{
				if(boost::iequals(entry->path().leaf().generic_string(), DIABDAT_MPQ))
				{
					return entry->path().leaf().generic_string();
				}
			}
		}

		std::cout << "Failed to find " << DIABDAT_MPQ << " in current directory" << std::endl;
		return "";
    }
}
