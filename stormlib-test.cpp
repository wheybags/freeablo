#include <StormLib.h>

#include <iostream>

#include <stdio.h>

//-----------------------------------------------------------------------------
// Extracts an archived file and saves it to the disk.
//
// Parameters :
//
//   char * szArchiveName  - Archive file name
//   char * szArchivedFile - Name/number of archived file.
//   char * szFileName     - Name of the target disk file.

static int ExtractFile(char * szArchiveName, char * szArchivedFile, char * szFileName)
{
    HANDLE hMpq   = NULL;          // Open archive handle
    HANDLE hFile  = NULL;          // Archived file handle
    HANDLE handle = NULL;          // Disk file handle
    int    nError = ERROR_SUCCESS; // Result value

    // Open an archive, e.g. "d2music.mpq"
    if(!SFileOpenArchive(szArchiveName, 0, 0, &hMpq))
        nError = GetLastError();

    if(nError != ERROR_SUCCESS)
    {
        std::cout << "failed opening mpq" << std::endl;
        return nError;
    }
    
    // Open a file in the archive, e.g. "data\global\music\Act1\tristram.wav"
    if(!SFileOpenFileEx(hMpq, szArchivedFile, 0, &hFile))
        nError = GetLastError();

    if(nError != ERROR_SUCCESS)
    {
        std::cout << "failed to open file in mpq" << std::endl;
        return nError;
    }

    FILE* output = fopen(szFileName, "w");

    // Read the file from the archive
    //if(nError == ERROR_SUCCESS)
    {
        char  szBuffer[256];
        DWORD dwBytes = 1;

        while(dwBytes > 0)
        {
            SFileReadFile(hFile, szBuffer, 256, &dwBytes, NULL);

            std::cout << dwBytes << std::endl;

            if(dwBytes > 0)
                fwrite(&szBuffer, 1, dwBytes, output);
        }
    }        
    
    fclose(output);
    SFileCloseFile(hFile);
    SFileCloseArchive(hMpq);

    return nError;
}

int main(int argc, char** argv)
{

    ExtractFile(argv[1], argv[2], argv[3]);

    return 0;
}
