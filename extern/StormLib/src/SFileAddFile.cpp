/*****************************************************************************/
/* SFileAddFile.cpp                       Copyright (c) Ladislav Zezula 2010 */
/*---------------------------------------------------------------------------*/
/* MPQ Editing functions                                                     */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 27.03.10  1.00  Lad  Splitted from SFileCreateArchiveEx.cpp               */
/* 21.04.13  1.01  Dea  AddFile callback now part of TMPQArchive             */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

//-----------------------------------------------------------------------------
// Local variables

// Mask for lossy compressions
#define MPQ_LOSSY_COMPRESSION_MASK (MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_ADPCM_STEREO | MPQ_COMPRESSION_HUFFMANN)

// Data compression for SFileAddFile
// Kept here for compatibility with code that was created with StormLib version < 6.50
static DWORD DefaultDataCompression = MPQ_COMPRESSION_PKWARE;

//-----------------------------------------------------------------------------
// WAVE verification

#define FILE_SIGNATURE_RIFF     0x46464952
#define FILE_SIGNATURE_WAVE     0x45564157
#define FILE_SIGNATURE_FMT      0x20746D66
#define AUDIO_FORMAT_PCM                 1

typedef struct _WAVE_FILE_HEADER
{
    DWORD dwChunkId;                        // 0x52494646 ("RIFF")
    DWORD dwChunkSize;                      // Size of that chunk, in bytes
    DWORD dwFormat;                         // Must be 0x57415645 ("WAVE")
   
    // Format sub-chunk 
    DWORD dwSubChunk1Id;                    // 0x666d7420 ("fmt ")
    DWORD dwSubChunk1Size;                  // 0x16 for PCM
    USHORT wAudioFormat;                    // 1 = PCM. Other value means some sort of compression
    USHORT wChannels;                       // Number of channels
    DWORD dwSampleRate;                     // 8000, 44100, etc.
    DWORD dwBytesRate;                      // SampleRate * NumChannels * BitsPerSample/8
    USHORT wBlockAlign;                     // NumChannels * BitsPerSample/8
    USHORT wBitsPerSample;                  // 8 bits = 8, 16 bits = 16, etc.

    // Followed by "data" sub-chunk (we don't care)
} WAVE_FILE_HEADER, *PWAVE_FILE_HEADER;

static bool IsWaveFile_16BitsPerAdpcmSample(
    LPBYTE pbFileData,
    DWORD cbFileData,
    LPDWORD pdwChannels)
{
    PWAVE_FILE_HEADER pWaveHdr = (PWAVE_FILE_HEADER)pbFileData;

    // The amount of file data must be at least size of WAVE header
    if(cbFileData > sizeof(WAVE_FILE_HEADER))
    {
        // Check for the RIFF header
        if(pWaveHdr->dwChunkId == FILE_SIGNATURE_RIFF && pWaveHdr->dwFormat == FILE_SIGNATURE_WAVE)
        {
            // Check for ADPCM format
            if(pWaveHdr->dwSubChunk1Id == FILE_SIGNATURE_FMT && pWaveHdr->wAudioFormat == AUDIO_FORMAT_PCM)
            {
                // Now the number of bits per sample must be at least 16.
                // If not, the WAVE file gets corrupted by the ADPCM compression
                if(pWaveHdr->wBitsPerSample >= 0x10)
                {
                    *pdwChannels = pWaveHdr->wChannels;
                    return true;
                }
            }
        }
    }

    return false;
}

static int FillWritableHandle(
    TMPQArchive * ha,
    TMPQFile * hf,
    ULONGLONG FileTime,
    DWORD dwFileSize,
    DWORD dwFlags)
{
    TFileEntry * pFileEntry = hf->pFileEntry;

    // Initialize the hash entry for the file
    hf->RawFilePos = ha->MpqPos + hf->MpqFilePos;
    hf->dwDataSize = dwFileSize;

    // Initialize the block table entry for the file
    pFileEntry->ByteOffset = hf->MpqFilePos;
    pFileEntry->dwFileSize = dwFileSize;
    pFileEntry->dwCmpSize = 0;
    pFileEntry->dwFlags  = dwFlags | MPQ_FILE_EXISTS;

    // Initialize the file time, CRC32 and MD5
    assert(sizeof(hf->hctx) >= sizeof(hash_state));
    memset(pFileEntry->md5, 0, MD5_DIGEST_SIZE);
    md5_init((hash_state *)hf->hctx);
    pFileEntry->dwCrc32 = crc32(0, Z_NULL, 0);

    // If the caller gave us a file time, use it.
    pFileEntry->FileTime = FileTime;

    // Mark the archive as modified
    ha->dwFlags |= MPQ_FLAG_CHANGED;

    // Call the callback, if needed
    if(ha->pfnAddFileCB != NULL)
        ha->pfnAddFileCB(ha->pvAddFileUserData, 0, hf->dwDataSize, false);
    hf->nAddFileError = ERROR_SUCCESS;

    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// MPQ write data functions

static int WriteDataToMpqFile(
    TMPQArchive * ha,
    TMPQFile * hf,
    LPBYTE pbFileData,
    DWORD dwDataSize,
    DWORD dwCompression)
{
    TFileEntry * pFileEntry = hf->pFileEntry;
    ULONGLONG ByteOffset;
    LPBYTE pbCompressed = NULL;             // Compressed (target) data
    LPBYTE pbToWrite = hf->pbFileSector;    // Data to write to the file
    int nCompressionLevel;                  // ADPCM compression level (only used for wave files)
    int nError = ERROR_SUCCESS;

    // Make sure that the caller won't overrun the previously initiated file size
    assert(hf->dwFilePos + dwDataSize <= pFileEntry->dwFileSize);
    assert(hf->dwSectorCount != 0);
    assert(hf->pbFileSector != NULL);
    if((hf->dwFilePos + dwDataSize) > pFileEntry->dwFileSize)
        return ERROR_DISK_FULL;

    // Now write all data to the file sector buffer
    if(nError == ERROR_SUCCESS)
    {
        DWORD dwBytesInSector = hf->dwFilePos % hf->dwSectorSize;
        DWORD dwSectorIndex = hf->dwFilePos / hf->dwSectorSize;
        DWORD dwBytesToCopy;

        // Process all data. 
        while(dwDataSize != 0)
        {
            dwBytesToCopy = dwDataSize;
                
            // Check for sector overflow
            if(dwBytesToCopy > (hf->dwSectorSize - dwBytesInSector))
                dwBytesToCopy = (hf->dwSectorSize - dwBytesInSector);

            // Copy the data to the file sector
            memcpy(hf->pbFileSector + dwBytesInSector, pbFileData, dwBytesToCopy);
            dwBytesInSector += dwBytesToCopy;
            pbFileData += dwBytesToCopy;
            dwDataSize -= dwBytesToCopy;

            // Update the file position
            hf->dwFilePos += dwBytesToCopy;

            // If the current sector is full, or if the file is already full,
            // then write the data to the MPQ
            if(dwBytesInSector >= hf->dwSectorSize || hf->dwFilePos >= pFileEntry->dwFileSize)
            {
                // Set the position in the file
                ByteOffset = hf->RawFilePos + pFileEntry->dwCmpSize;

                // Update CRC32 and MD5 of the file
                md5_process((hash_state *)hf->hctx, hf->pbFileSector, dwBytesInSector);
                hf->dwCrc32 = crc32(hf->dwCrc32, hf->pbFileSector, dwBytesInSector);

                // Compress the file sector, if needed
                if(pFileEntry->dwFlags & MPQ_FILE_COMPRESS_MASK)
                {
                    int nOutBuffer = (int)dwBytesInSector;
                    int nInBuffer = (int)dwBytesInSector;

                    // If the file is compressed, allocate buffer for the compressed data.
                    // Note that we allocate buffer that is a bit longer than sector size,
                    // for case if the compression method performs a buffer overrun
                    if(pbCompressed == NULL)
                    {
                        pbToWrite = pbCompressed = STORM_ALLOC(BYTE, hf->dwSectorSize + 0x100);
                        if(pbCompressed == NULL)
                        {
                            nError = ERROR_NOT_ENOUGH_MEMORY;
                            break;
                        }
                    }

                    //
                    // Note that both SCompImplode and SCompCompress copy data as-is,
                    // if they are unable to compress the data.
                    //

                    if(pFileEntry->dwFlags & MPQ_FILE_IMPLODE)
                    {
                        SCompImplode(pbCompressed, &nOutBuffer, hf->pbFileSector, nInBuffer);
                    }

                    if(pFileEntry->dwFlags & MPQ_FILE_COMPRESS)
                    {
                        // If this is the first sector, we need to override the given compression
                        // by the first sector compression. This is because the entire sector must
                        // be compressed by the same compression.
                        //
                        // Test case:                        
                        //
                        // WRITE_FILE(hFile, pvBuffer, 0x10, MPQ_COMPRESSION_PKWARE)       // Write 0x10 bytes (sector 0)
                        // WRITE_FILE(hFile, pvBuffer, 0x10, MPQ_COMPRESSION_ADPCM_MONO)   // Write 0x10 bytes (still sector 0)
                        // WRITE_FILE(hFile, pvBuffer, 0x10, MPQ_COMPRESSION_ADPCM_MONO)   // Write 0x10 bytes (still sector 0)
                        // WRITE_FILE(hFile, pvBuffer, 0x10, MPQ_COMPRESSION_ADPCM_MONO)   // Write 0x10 bytes (still sector 0)
                        dwCompression = (dwSectorIndex == 0) ? hf->dwCompression0 : dwCompression;

                        // If the caller wants ADPCM compression, we will set wave compression level to 4,
                        // which corresponds to medium quality
                        nCompressionLevel = (dwCompression & MPQ_LOSSY_COMPRESSION_MASK) ? 4 : -1;
                        SCompCompress(pbCompressed, &nOutBuffer, hf->pbFileSector, nInBuffer, (unsigned)dwCompression, 0, nCompressionLevel);
                    }

                    // Update sector positions
                    dwBytesInSector = nOutBuffer;
                    if(hf->SectorOffsets != NULL)
                        hf->SectorOffsets[dwSectorIndex+1] = hf->SectorOffsets[dwSectorIndex] + dwBytesInSector;

                    // We have to calculate sector CRC, if enabled
                    if(hf->SectorChksums != NULL)
                        hf->SectorChksums[dwSectorIndex] = adler32(0, pbCompressed, nOutBuffer);
                }                 

                // Encrypt the sector, if necessary
                if(pFileEntry->dwFlags & MPQ_FILE_ENCRYPTED)
                {
                    BSWAP_ARRAY32_UNSIGNED(pbToWrite, dwBytesInSector);
                    EncryptMpqBlock(pbToWrite, dwBytesInSector, hf->dwFileKey + dwSectorIndex);
                    BSWAP_ARRAY32_UNSIGNED(pbToWrite, dwBytesInSector);
                }

                // Write the file sector
                if(!FileStream_Write(ha->pStream, &ByteOffset, pbToWrite, dwBytesInSector))
                {
                    nError = GetLastError();
                    break;
                }

                // Call the compact callback, if any
                if(ha->pfnAddFileCB != NULL)
                    ha->pfnAddFileCB(ha->pvAddFileUserData, hf->dwFilePos, hf->dwDataSize, false);

                // Update the compressed file size
                pFileEntry->dwCmpSize += dwBytesInSector;
                dwBytesInSector = 0;
                dwSectorIndex++;
            }
        }
    }

    // Cleanup
    if(pbCompressed != NULL)
        STORM_FREE(pbCompressed);
    return nError;
}

//-----------------------------------------------------------------------------
// Recrypts file data for file renaming

static int RecryptFileData(
    TMPQArchive * ha,
    TMPQFile * hf,
    const char * szFileName,
    const char * szNewFileName)
{
    ULONGLONG RawFilePos;
    TFileEntry * pFileEntry = hf->pFileEntry;
    DWORD dwBytesToRecrypt = pFileEntry->dwCmpSize;
    DWORD dwOldKey;
    DWORD dwNewKey;
    int nError = ERROR_SUCCESS;

    // The file must be encrypted
    assert(pFileEntry->dwFlags & MPQ_FILE_ENCRYPTED);

    // File decryption key is calculated from the plain name
    szNewFileName = GetPlainFileName(szNewFileName);
    szFileName = GetPlainFileName(szFileName);

    // Calculate both file keys
    dwOldKey = DecryptFileKey(szFileName,    pFileEntry->ByteOffset, pFileEntry->dwFileSize, pFileEntry->dwFlags);
    dwNewKey = DecryptFileKey(szNewFileName, pFileEntry->ByteOffset, pFileEntry->dwFileSize, pFileEntry->dwFlags);

    // Incase the keys are equal, don't recrypt the file
    if(dwNewKey == dwOldKey)
        return ERROR_SUCCESS;
    hf->dwFileKey = dwOldKey;

    // Calculate the raw position of the file in the archive
    hf->MpqFilePos = pFileEntry->ByteOffset;
    hf->RawFilePos = ha->MpqPos + hf->MpqFilePos;

    // Allocate buffer for file transfer
    nError = AllocateSectorBuffer(hf);
    if(nError != ERROR_SUCCESS)
        return nError;

    // Also allocate buffer for sector offsets
    // Note: Don't load sector checksums, we don't need to recrypt them
    nError = AllocateSectorOffsets(hf, true);
    if(nError != ERROR_SUCCESS)
        return nError;

    // If we have sector offsets, recrypt these as well
    if(hf->SectorOffsets != NULL)
    {
        // Allocate secondary buffer for sectors copy
        DWORD * SectorOffsetsCopy = STORM_ALLOC(DWORD, hf->SectorOffsets[0] / sizeof(DWORD));
        DWORD dwSectorOffsLen = hf->SectorOffsets[0];

        if(SectorOffsetsCopy == NULL)
            return ERROR_NOT_ENOUGH_MEMORY;

        // Recrypt the array of sector offsets
        memcpy(SectorOffsetsCopy, hf->SectorOffsets, dwSectorOffsLen);
        EncryptMpqBlock(SectorOffsetsCopy, dwSectorOffsLen, dwNewKey - 1);
        BSWAP_ARRAY32_UNSIGNED(SectorOffsetsCopy, dwSectorOffsLen);

        // Write the recrypted array back
        if(!FileStream_Write(ha->pStream, &hf->RawFilePos, SectorOffsetsCopy, dwSectorOffsLen))
            nError = GetLastError();
        STORM_FREE(SectorOffsetsCopy);
    }

    // Now we have to recrypt all file sectors. We do it without
    // recompression, because recompression is not necessary in this case
    if(nError == ERROR_SUCCESS)
    {
        for(DWORD dwSector = 0; dwSector < hf->dwSectorCount; dwSector++)
        {
            DWORD dwRawDataInSector = hf->dwSectorSize;
            DWORD dwRawByteOffset = dwSector * hf->dwSectorSize;

            // Last sector: If there is not enough bytes remaining in the file, cut the raw size
            if(dwRawDataInSector > dwBytesToRecrypt)
                dwRawDataInSector = dwBytesToRecrypt;

            // Fix the raw data length if the file is compressed
            if(hf->SectorOffsets != NULL)
            {
                dwRawDataInSector = hf->SectorOffsets[dwSector+1] - hf->SectorOffsets[dwSector];
                dwRawByteOffset = hf->SectorOffsets[dwSector];
            }

            // Calculate the raw file offset of the file sector
            RawFilePos = CalculateRawSectorOffset(hf, dwRawByteOffset);

            // Read the file sector
            if(!FileStream_Read(ha->pStream, &RawFilePos, hf->pbFileSector, dwRawDataInSector))
            {
                nError = GetLastError();
                break;
            }

            // If necessary, re-encrypt the sector
            // Note: Recompression is not necessary here. Unlike encryption, 
            // the compression does not depend on the position of the file in MPQ.
            BSWAP_ARRAY32_UNSIGNED(hf->pbFileSector, dwRawDataInSector);
            DecryptMpqBlock(hf->pbFileSector, dwRawDataInSector, dwOldKey + dwSector);
            EncryptMpqBlock(hf->pbFileSector, dwRawDataInSector, dwNewKey + dwSector);
            BSWAP_ARRAY32_UNSIGNED(hf->pbFileSector, dwRawDataInSector);

            // Write the sector back
            if(!FileStream_Write(ha->pStream, &RawFilePos, hf->pbFileSector, dwRawDataInSector))
            {
                nError = GetLastError();
                break;
            }

            // Decrement number of bytes remaining
            dwBytesToRecrypt -= hf->dwSectorSize;
        }
    }

    return nError;
}

//-----------------------------------------------------------------------------
// Internal support for MPQ modifications

int SFileAddFile_Init(
    TMPQArchive * ha,
    const char * szFileName,
    ULONGLONG FileTime,
    DWORD dwFileSize,
    LCID lcLocale,
    DWORD dwFlags,
    TMPQFile ** phf)
{
    TFileEntry * pFileEntry = NULL;
    TMPQFile * hf = NULL;               // File structure for newly added file
    DWORD dwHashIndex = HASH_ENTRY_FREE;
    int nError = ERROR_SUCCESS;

    //
    // Note: This is an internal function so no validity checks are done.
    // It is the caller's responsibility to make sure that no invalid
    // flags get to this point
    //

    // Sestor CRC is not allowed with single unit files
    if(dwFlags & MPQ_FILE_SINGLE_UNIT)
        dwFlags &= ~MPQ_FILE_SECTOR_CRC;

    // Sector CRC is not allowed if the file is not compressed
    if(!(dwFlags & MPQ_FILE_COMPRESS_MASK))
        dwFlags &= ~MPQ_FILE_SECTOR_CRC;
    
    // Fix Key is not allowed if the file is not enrypted
    if(!(dwFlags & MPQ_FILE_ENCRYPTED))
        dwFlags &= ~MPQ_FILE_FIX_KEY;

    // If the MPQ is of version 3.0 or higher, we ignore file locale.
    // This is because HET and BET tables have no known support for it
    if(ha->pHeader->wFormatVersion >= MPQ_FORMAT_VERSION_3)
        lcLocale = 0;

    // Allocate the TMPQFile entry for newly added file
    hf = CreateWritableHandle(ha, dwFileSize);
    if(hf == NULL)
        return false;

    // Allocate file entry in the MPQ
    if(nError == ERROR_SUCCESS)
    {
        // Check if the file already exists in the archive
        pFileEntry = GetFileEntryExact(ha, szFileName, lcLocale, &dwHashIndex);
        if(pFileEntry != NULL)
        {
            if(dwFlags & MPQ_FILE_REPLACEEXISTING)
                InvalidateInternalFiles(ha);
            else
                nError = ERROR_ALREADY_EXISTS;
        }
        else
        {
            // Attempt to allocate new file entry
            pFileEntry = AllocateFileEntry(ha, szFileName, lcLocale, &dwHashIndex);
            if(pFileEntry != NULL)
                InvalidateInternalFiles(ha);
            else
                nError = ERROR_DISK_FULL;   
        }

        // Set the file entry to the file structure
        hf->pFileEntry = pFileEntry;
    }

    // Prepare the pointer to hash table entry
    if(nError == ERROR_SUCCESS && ha->pHashTable != NULL && dwHashIndex < ha->pHeader->dwHashTableSize)
    {
        hf->pHashEntry = ha->pHashTable + dwHashIndex;
        hf->pHashEntry->lcLocale = (USHORT)lcLocale;
    }

    // Prepare the file key
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_ENCRYPTED))
    {
        hf->dwFileKey = DecryptFileKey(szFileName, hf->MpqFilePos, dwFileSize, dwFlags);
        if(hf->dwFileKey == 0)
            nError = ERROR_UNKNOWN_FILE_KEY;
    }

    // Fill the file entry and TMPQFile structure
    if(nError == ERROR_SUCCESS)
    {
        // At this point, the file name in the file entry must be set
        assert(pFileEntry->szFileName != NULL);
        assert(_stricmp(pFileEntry->szFileName, szFileName) == 0);

        nError = FillWritableHandle(ha, hf, FileTime, dwFileSize, dwFlags);
    }

    // Free the file handle if failed
    if(nError != ERROR_SUCCESS && hf != NULL)
        FreeFileHandle(hf);

    // Give the handle to the caller
    *phf = hf;
    return nError;
}

int SFileAddFile_Init(
    TMPQArchive * ha,
    TMPQFile * hfSrc,
    TMPQFile ** phf)
{
    TFileEntry * pFileEntry = NULL;
    TMPQFile * hf = NULL;               // File structure for newly added file
    ULONGLONG FileTime = hfSrc->pFileEntry->FileTime;
    DWORD dwFileSize = hfSrc->pFileEntry->dwFileSize;
    DWORD dwFlags = hfSrc->pFileEntry->dwFlags;
    int nError = ERROR_SUCCESS;

    // Allocate the TMPQFile entry for newly added file
    hf = CreateWritableHandle(ha, dwFileSize);
    if(hf == NULL)
        nError = ERROR_NOT_ENOUGH_MEMORY;

    // We need to keep the file entry index the same like in the source archive
    // This is because multiple hash table entries can point to the same file entry
    if(nError == ERROR_SUCCESS)
    {
        // Retrieve the file entry for the target file
        pFileEntry = ha->pFileTable + (hfSrc->pFileEntry - hfSrc->ha->pFileTable);

        // Copy all variables except file name
        if((pFileEntry->dwFlags & MPQ_FILE_EXISTS) == 0)
        {
            pFileEntry[0] = hfSrc->pFileEntry[0];
            pFileEntry->szFileName = NULL;
        }
        else
            nError = ERROR_ALREADY_EXISTS;

        // Set the file entry to the file structure
        hf->pFileEntry = pFileEntry;
    }

    // Prepare the pointer to hash table entry
    if(nError == ERROR_SUCCESS && ha->pHashTable != NULL && hfSrc->pHashEntry != NULL)
    {
        hf->dwHashIndex = (DWORD)(hfSrc->pHashEntry - hfSrc->ha->pHashTable);
        hf->pHashEntry = ha->pHashTable + hf->dwHashIndex;
    }

    // Prepare the file key (copy from source file)
    if(nError == ERROR_SUCCESS && (dwFlags & MPQ_FILE_ENCRYPTED))
    {
        hf->dwFileKey = hfSrc->dwFileKey;
        if(hf->dwFileKey == 0)
            nError = ERROR_UNKNOWN_FILE_KEY;
    }

    // Fill the file entry and TMPQFile structure
    if(nError == ERROR_SUCCESS)
    {
        nError = FillWritableHandle(ha, hf, FileTime, dwFileSize, dwFlags);
    }

    // Free the file handle if failed
    if(nError != ERROR_SUCCESS && hf != NULL)
        FreeFileHandle(hf);

    // Give the handle to the caller
    *phf = hf;
    return nError;
}

int SFileAddFile_Write(TMPQFile * hf, const void * pvData, DWORD dwSize, DWORD dwCompression)
{
    TMPQArchive * ha;
    TFileEntry * pFileEntry;
    int nError = ERROR_SUCCESS;

    // Don't bother if the caller gave us zero size
    if(pvData == NULL || dwSize == 0)
        return ERROR_SUCCESS;

    // Get pointer to the MPQ archive
    pFileEntry = hf->pFileEntry;
    ha = hf->ha;

    // Allocate file buffers
    if(hf->pbFileSector == NULL)
    {
        ULONGLONG RawFilePos = hf->RawFilePos;

        // Allocate buffer for file sector
        hf->nAddFileError = nError = AllocateSectorBuffer(hf);
        if(nError != ERROR_SUCCESS)
            return nError;

        // Allocate patch info, if the data is patch
        if(hf->pPatchInfo == NULL && IsIncrementalPatchFile(pvData, dwSize, &hf->dwPatchedFileSize))
        {
            // Set the MPQ_FILE_PATCH_FILE flag
            hf->pFileEntry->dwFlags |= MPQ_FILE_PATCH_FILE;

            // Allocate the patch info
            hf->nAddFileError = nError = AllocatePatchInfo(hf, false);
            if(nError != ERROR_SUCCESS)
                return nError;
        }

        // Allocate sector offsets
        if(hf->SectorOffsets == NULL)
        {
            hf->nAddFileError = nError = AllocateSectorOffsets(hf, false);
            if(nError != ERROR_SUCCESS)
                return nError;
        }

        // Create array of sector checksums
        if(hf->SectorChksums == NULL && (pFileEntry->dwFlags & MPQ_FILE_SECTOR_CRC))
        {
            hf->nAddFileError = nError = AllocateSectorChecksums(hf, false);
            if(nError != ERROR_SUCCESS)
                return nError;
        }

        // Pre-save the patch info, if any
        if(hf->pPatchInfo != NULL)
        {
            if(!FileStream_Write(ha->pStream, &RawFilePos, hf->pPatchInfo, hf->pPatchInfo->dwLength))
                nError = GetLastError();
  
            pFileEntry->dwCmpSize += hf->pPatchInfo->dwLength;
            RawFilePos += hf->pPatchInfo->dwLength;
        }

        // Pre-save the sector offset table, just to reserve space in the file.
        // Note that we dont need to swap the sector positions, nor encrypt the table
        // at the moment, as it will be written again after writing all file sectors.
        if(hf->SectorOffsets != NULL)
        {
            if(!FileStream_Write(ha->pStream, &RawFilePos, hf->SectorOffsets, hf->SectorOffsets[0]))
                nError = GetLastError();

            pFileEntry->dwCmpSize += hf->SectorOffsets[0];
            RawFilePos += hf->SectorOffsets[0];
        }
    }

    // Write the MPQ data to the file
    if(nError == ERROR_SUCCESS)
    {
        // Save the first sector compression to the file structure
        // Note that the entire first file sector will be compressed
        // by compression that was passed to the first call of SFileAddFile_Write
        if(hf->dwFilePos == 0)
            hf->dwCompression0 = dwCompression;

        // Write the data to the MPQ
        nError = WriteDataToMpqFile(ha, hf, (LPBYTE)pvData, dwSize, dwCompression);
    }

    // If it succeeded and we wrote all the file data,
    // we need to re-save sector offset table
    if(nError == ERROR_SUCCESS)
    {
        if(hf->dwFilePos >= pFileEntry->dwFileSize)
        {
            // Finish calculating CRC32
            hf->pFileEntry->dwCrc32 = hf->dwCrc32;

            // Finish calculating MD5
            md5_done((hash_state *)hf->hctx, hf->pFileEntry->md5);

            // If we also have sector checksums, write them to the file
            if(hf->SectorChksums != NULL)
            {
                nError = WriteSectorChecksums(hf);
            }

            // Now write patch info
            if(hf->pPatchInfo != NULL)
            {
                memcpy(hf->pPatchInfo->md5, hf->pFileEntry->md5, MD5_DIGEST_SIZE);
                hf->pPatchInfo->dwDataSize  = hf->pFileEntry->dwFileSize;
                hf->pFileEntry->dwFileSize = hf->dwPatchedFileSize;
                nError = WritePatchInfo(hf);
            }

            // Now write sector offsets to the file
            if(hf->SectorOffsets != NULL)
            {
                nError = WriteSectorOffsets(hf);
            }

            // Write the MD5 hashes of each file chunk, if required
            if(ha->pHeader->dwRawChunkSize != 0)
            {
                nError = WriteMpqDataMD5(ha->pStream,
                                         ha->MpqPos + hf->pFileEntry->ByteOffset,
                                         hf->pFileEntry->dwCmpSize,
                                         ha->pHeader->dwRawChunkSize);
            }
        }
    }

    // Store the error code from the Write File operation
    hf->nAddFileError = nError;
    return nError;
}

int SFileAddFile_Finish(TMPQFile * hf)
{
    TMPQArchive * ha = hf->ha;
    TFileEntry * pFileEntry = hf->pFileEntry;
    int nError = hf->nAddFileError;

    // If all previous operations succeeded, we can update the MPQ
    if(nError == ERROR_SUCCESS)
    {
        // Verify if the caller wrote the file properly
        if(hf->pPatchInfo == NULL)
        {
            assert(pFileEntry != NULL);
            if(hf->dwFilePos != pFileEntry->dwFileSize)
                nError = ERROR_CAN_NOT_COMPLETE;
        }
        else
        {
            if(hf->dwFilePos != hf->pPatchInfo->dwDataSize)
                nError = ERROR_CAN_NOT_COMPLETE;
        }
    }

    // Now we need to recreate the HET table, if exists
    if(nError == ERROR_SUCCESS && ha->pHetTable != NULL)
    {
        nError = RebuildHetTable(ha);
    }

    // Update the block table size
    if(nError == ERROR_SUCCESS)
    {
        // Call the user callback, if any
        if(ha->pfnAddFileCB != NULL)
            ha->pfnAddFileCB(ha->pvAddFileUserData, hf->dwDataSize, hf->dwDataSize, true);
    }
    else
    {
        // Free the file entry in MPQ tables
        if(pFileEntry != NULL)
            DeleteFileEntry(ha, hf);
    }

    // Clear the add file callback
    FreeFileHandle(hf);
    return nError;
}

//-----------------------------------------------------------------------------
// Adds data as file to the archive 

bool WINAPI SFileCreateFile(
    HANDLE hMpq,
    const char * szArchivedName,
    ULONGLONG FileTime,
    DWORD dwFileSize,
    LCID lcLocale,
    DWORD dwFlags,
    HANDLE * phFile)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    int nError = ERROR_SUCCESS;

    // Check valid parameters
    if(!IsValidMpqHandle(hMpq))
        nError = ERROR_INVALID_HANDLE;
    if(szArchivedName == NULL || *szArchivedName == 0)
        nError = ERROR_INVALID_PARAMETER;
    if(phFile == NULL)
        nError = ERROR_INVALID_PARAMETER;
    
    // Don't allow to add file if the MPQ is open for read only
    if(nError == ERROR_SUCCESS)
    {
        if(ha->dwFlags & MPQ_FLAG_READ_ONLY)
            nError = ERROR_ACCESS_DENIED;

        // Don't allow to add a file under pseudo-file name
        if(IsPseudoFileName(szArchivedName, NULL))
            nError = ERROR_INVALID_PARAMETER;

        // Don't allow to add any of the internal files
        if(IsInternalMpqFileName(szArchivedName))
            nError = ERROR_INTERNAL_FILE;
    }

    // Perform validity check of the MPQ flags
    if(nError == ERROR_SUCCESS)
    {
        // Mask all unsupported flags out
        dwFlags &= (ha->dwFlags & MPQ_FLAG_WAR3_MAP) ? MPQ_FILE_VALID_FLAGS_W3X : MPQ_FILE_VALID_FLAGS;

        // Check for valid flag combinations
        if((dwFlags & (MPQ_FILE_IMPLODE | MPQ_FILE_COMPRESS)) == (MPQ_FILE_IMPLODE | MPQ_FILE_COMPRESS))
            nError = ERROR_INVALID_PARAMETER;
    }

    // Initiate the add file operation
    if(nError == ERROR_SUCCESS)
        nError = SFileAddFile_Init(ha, szArchivedName, FileTime, dwFileSize, lcLocale, dwFlags, (TMPQFile **)phFile);

    // Deal with the errors
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

bool WINAPI SFileWriteFile(
    HANDLE hFile,
    const void * pvData,
    DWORD dwSize,
    DWORD dwCompression)
{
    TMPQFile * hf = (TMPQFile *)hFile;
    int nError = ERROR_SUCCESS;

    // Check the proper parameters
    if(!IsValidFileHandle(hFile))
        nError = ERROR_INVALID_HANDLE;
    if(hf->bIsWriteHandle == false)
        nError = ERROR_INVALID_HANDLE;

    // Special checks for single unit files
    if(nError == ERROR_SUCCESS && (hf->pFileEntry->dwFlags & MPQ_FILE_SINGLE_UNIT))
    {
        //
        // Note: Blizzard doesn't support single unit files
        // that are stored as encrypted or imploded. We will allow them here,
        // the calling application must ensure that such flag combination doesn't get here
        //

//      if(dwFlags & MPQ_FILE_IMPLODE)
//          nError = ERROR_INVALID_PARAMETER;
//
//      if(dwFlags & MPQ_FILE_ENCRYPTED)
//          nError = ERROR_INVALID_PARAMETER;
        
        // Lossy compression is not allowed on single unit files
        if(dwCompression & MPQ_LOSSY_COMPRESSION_MASK)
            nError = ERROR_INVALID_PARAMETER;
    }


    // Write the data to the file
    if(nError == ERROR_SUCCESS)
        nError = SFileAddFile_Write(hf, pvData, dwSize, dwCompression);
    
    // Deal with errors
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

bool WINAPI SFileFinishFile(HANDLE hFile)
{
    TMPQFile * hf = (TMPQFile *)hFile;
    int nError = ERROR_SUCCESS;

    // Check the proper parameters
    if(!IsValidFileHandle(hFile))
        nError = ERROR_INVALID_HANDLE;
    if(hf->bIsWriteHandle == false)
        nError = ERROR_INVALID_HANDLE;

    // Finish the file
    if(nError == ERROR_SUCCESS)
        nError = SFileAddFile_Finish(hf);
    
    // Deal with errors
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// Adds a file to the archive 

bool WINAPI SFileAddFileEx(
    HANDLE hMpq,
    const TCHAR * szFileName,
    const char * szArchivedName,
    DWORD dwFlags,
    DWORD dwCompression,            // Compression of the first sector
    DWORD dwCompressionNext)        // Compression of next sectors
{
    ULONGLONG FileSize = 0;
    ULONGLONG FileTime = 0;
    TFileStream * pStream = NULL;
    HANDLE hMpqFile = NULL;
    LPBYTE pbFileData = NULL;
    DWORD dwBytesRemaining = 0;
    DWORD dwBytesToRead;
    DWORD dwSectorSize = 0x1000;
    DWORD dwChannels = 0;
    bool bIsAdpcmCompression = false;
    bool bIsFirstSector = true;
    int nError = ERROR_SUCCESS;

    // Check parameters
    if(hMpq == NULL || szFileName == NULL || *szFileName == 0)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    // Open added file
    pStream = FileStream_OpenFile(szFileName, STREAM_FLAG_READ_ONLY | STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE);
    if(pStream == NULL)
        return false;

    // Files bigger than 4GB cannot be added to MPQ
    FileStream_GetTime(pStream, &FileTime);
    FileStream_GetSize(pStream, &FileSize);
    if(FileSize >> 32)
        nError = ERROR_DISK_FULL;

    // Allocate data buffer for reading from the source file
    if(nError == ERROR_SUCCESS)
    {
        dwBytesRemaining = (DWORD)FileSize;
        pbFileData = STORM_ALLOC(BYTE, dwSectorSize);
        if(pbFileData == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Deal with various combination of compressions
    if(nError == ERROR_SUCCESS)
    {
        // When the compression for next blocks is set to default,
        // we will copy the compression for the first sector
        if(dwCompressionNext == MPQ_COMPRESSION_NEXT_SAME)
            dwCompressionNext = dwCompression;
        
        // If the caller wants ADPCM compression, we make sure
        // that the first sector is not compressed with lossy compression
        if(dwCompressionNext & (MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_ADPCM_STEREO))
        {
            // The compression of the first file sector must not be ADPCM
            // in order not to corrupt the headers
            if(dwCompression & (MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_ADPCM_STEREO))
                dwCompression = MPQ_COMPRESSION_PKWARE;
            
            // Remove both flag mono and stereo flags.
            // They will be re-added according to WAVE type
            dwCompressionNext &= ~(MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_ADPCM_STEREO);
            bIsAdpcmCompression = true;
        }

        // Initiate adding file to the MPQ
        if(!SFileCreateFile(hMpq, szArchivedName, FileTime, (DWORD)FileSize, lcFileLocale, dwFlags, &hMpqFile))
            nError = GetLastError();
    }

    // Write the file data to the MPQ
    while(nError == ERROR_SUCCESS && dwBytesRemaining != 0)
    {
        // Get the number of bytes remaining in the source file
        dwBytesToRead = dwBytesRemaining;
        if(dwBytesToRead > dwSectorSize)
            dwBytesToRead = dwSectorSize;

        // Read data from the local file
        if(!FileStream_Read(pStream, NULL, pbFileData, dwBytesToRead))
        {
            nError = GetLastError();
            break;
        }

        // If the file being added is a WAVE file, we check number of channels
        if(bIsFirstSector && bIsAdpcmCompression)
        {
            // The file must really be a WAVE file with at least 16 bits per sample,
            // otherwise the ADPCM compression will corrupt it
            if(IsWaveFile_16BitsPerAdpcmSample(pbFileData, dwBytesToRead, &dwChannels))
            {
                // Setup the compression of next sectors according to number of channels
                dwCompressionNext |= (dwChannels == 1) ? MPQ_COMPRESSION_ADPCM_MONO : MPQ_COMPRESSION_ADPCM_STEREO;
            }
            else
            {
                // Setup the compression of next sectors to a lossless compression
                dwCompressionNext = (dwCompression & MPQ_LOSSY_COMPRESSION_MASK) ? MPQ_COMPRESSION_PKWARE : dwCompression;
            }

            bIsFirstSector = false;
        }

        // Add the file sectors to the MPQ
        if(!SFileWriteFile(hMpqFile, pbFileData, dwBytesToRead, dwCompression))
        {
            nError = GetLastError();
            break;
        }

        // Set the next data compression
        dwBytesRemaining -= dwBytesToRead;
        dwCompression = dwCompressionNext;
    }

    // Finish the file writing
    if(hMpqFile != NULL)
    {
        if(!SFileFinishFile(hMpqFile))
            nError = GetLastError();
    }

    // Cleanup and exit
    if(pbFileData != NULL)
        STORM_FREE(pbFileData);
    if(pStream != NULL)
        FileStream_Close(pStream);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}
                                                                                                                                 
// Adds a data file into the archive
bool WINAPI SFileAddFile(HANDLE hMpq, const TCHAR * szFileName, const char * szArchivedName, DWORD dwFlags)
{
    return SFileAddFileEx(hMpq,
                          szFileName,
                          szArchivedName,
                          dwFlags,
                          DefaultDataCompression,
                          DefaultDataCompression);
}

// Adds a WAVE file into the archive
bool WINAPI SFileAddWave(HANDLE hMpq, const TCHAR * szFileName, const char * szArchivedName, DWORD dwFlags, DWORD dwQuality)
{
    DWORD dwCompression = 0;

    //
    // Note to wave compression level:
    // The following conversion table applied:
    // High quality:   WaveCompressionLevel = -1
    // Medium quality: WaveCompressionLevel = 4
    // Low quality:    WaveCompressionLevel = 2
    //
    // Starcraft files are packed as Mono (0x41) on medium quality.
    // Because this compression is not used anymore, our compression functions
    // will default to WaveCompressionLevel = 4 when using ADPCM compression
    // 

    // Convert quality to data compression
    switch(dwQuality)
    {
        case MPQ_WAVE_QUALITY_HIGH:
//          WaveCompressionLevel = -1;
            dwCompression = MPQ_COMPRESSION_PKWARE;
            break;

        case MPQ_WAVE_QUALITY_MEDIUM:
//          WaveCompressionLevel = 4;
            dwCompression = MPQ_COMPRESSION_ADPCM_STEREO | MPQ_COMPRESSION_HUFFMANN;
            break;

        case MPQ_WAVE_QUALITY_LOW:
//          WaveCompressionLevel = 2;
            dwCompression = MPQ_COMPRESSION_ADPCM_STEREO | MPQ_COMPRESSION_HUFFMANN;
            break;
    }

    return SFileAddFileEx(hMpq,
                          szFileName,
                          szArchivedName,
                          dwFlags,
                          MPQ_COMPRESSION_PKWARE,   // First sector should be compressed as data
                          dwCompression);           // Next sectors should be compressed as WAVE
}

//-----------------------------------------------------------------------------
// bool SFileRemoveFile(HANDLE hMpq, char * szFileName)
//
// This function removes a file from the archive.
//

bool WINAPI SFileRemoveFile(HANDLE hMpq, const char * szFileName, DWORD dwSearchScope)
{
    TMPQArchive * ha = IsValidMpqHandle(hMpq);
    TMPQFile * hf = NULL;
    int nError = ERROR_SUCCESS;

    // Keep compiler happy
    dwSearchScope = dwSearchScope;

    // Check the parameters
    if(ha == NULL)
        nError = ERROR_INVALID_HANDLE;
    if(szFileName == NULL || *szFileName == 0)
        nError = ERROR_INVALID_PARAMETER;
    if(IsInternalMpqFileName(szFileName))
        nError = ERROR_INTERNAL_FILE;

    // Do not allow to remove files from read-only or patched MPQs
    if(nError == ERROR_SUCCESS)
    {
        if((ha->dwFlags & MPQ_FLAG_READ_ONLY) || (ha->haPatch != NULL))
            nError = ERROR_ACCESS_DENIED;
    }

    // If all checks have passed, we can delete the file from the MPQ
    if(nError == ERROR_SUCCESS)
    {
        // Open the file from the MPQ
        if(SFileOpenFileEx(hMpq, szFileName, SFILE_OPEN_BASE_FILE, (HANDLE *)&hf))
        {
            // Delete the file entry
            nError = DeleteFileEntry(ha, hf);
            FreeFileHandle(hf);
        }
        else
            nError = GetLastError();
    }

    // If the file has been deleted, we need to invalidate
    // the internal files and recreate HET table
    if(nError == ERROR_SUCCESS)
    {
        // Invalidate the entries for internal files
        // After we are done with MPQ changes, we need to re-create them anyway
        InvalidateInternalFiles(ha);

        //
        // Don't rebuild HET table now; the file's flags indicate
        // that it's been deleted, which is enough
        //
    }

    // Resolve error and exit
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

// Renames the file within the archive.
bool WINAPI SFileRenameFile(HANDLE hMpq, const char * szFileName, const char * szNewFileName)
{
    TMPQArchive * ha = IsValidMpqHandle(hMpq);
    TMPQFile * hf;
    int nError = ERROR_SUCCESS;

    // Test the valid parameters
    if(ha == NULL)
        nError = ERROR_INVALID_HANDLE;
    if(szFileName == NULL || *szFileName == 0 || szNewFileName == NULL || *szNewFileName == 0)
        nError = ERROR_INVALID_PARAMETER;
    if(IsInternalMpqFileName(szFileName) || IsInternalMpqFileName(szNewFileName))
        nError = ERROR_INTERNAL_FILE;

    // Do not allow to rename files in MPQ open for read only
    if(nError == ERROR_SUCCESS)
    {
        if(ha->dwFlags & MPQ_FLAG_READ_ONLY)
            nError = ERROR_ACCESS_DENIED;
    }

    // Open the new file. If exists, we don't allow rename operation
    if(nError == ERROR_SUCCESS)
    {
        if(GetFileEntryLocale(ha, szNewFileName, lcFileLocale) != NULL)
            nError = ERROR_ALREADY_EXISTS;
    }

    // Open the file from the MPQ
    if(nError == ERROR_SUCCESS)
    {
        // Attempt to open the file
        if(SFileOpenFileEx(hMpq, szFileName, SFILE_OPEN_BASE_FILE, (HANDLE *)&hf))
        {
            ULONGLONG RawDataOffs;
            TFileEntry * pFileEntry = hf->pFileEntry;

            // Invalidate the entries for internal files
            InvalidateInternalFiles(ha);

            // Rename the file entry in the table
            nError = RenameFileEntry(ha, hf, szNewFileName);

            // If the file is encrypted, we have to re-crypt the file content
            // with the new decryption key
            if((nError == ERROR_SUCCESS) && (pFileEntry->dwFlags & MPQ_FILE_ENCRYPTED))
            {
                // Recrypt the file data in the MPQ
                nError = RecryptFileData(ha, hf, szFileName, szNewFileName);
                
                // Update the MD5 of the raw block
                if(nError == ERROR_SUCCESS && ha->pHeader->dwRawChunkSize != 0)
                {
                    RawDataOffs = ha->MpqPos + pFileEntry->ByteOffset;
                    WriteMpqDataMD5(ha->pStream,
                                    RawDataOffs,
                                    pFileEntry->dwCmpSize,
                                    ha->pHeader->dwRawChunkSize);
                }
            }

            // Free the file handle
            FreeFileHandle(hf);
        }
        else
        {
            nError = GetLastError();
        }
    }

    // We also need to rebuild the HET table, if present
    if(nError == ERROR_SUCCESS && ha->pHetTable != NULL)
        nError = RebuildHetTable(ha);

    // Resolve error and exit
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// Sets default data compression for SFileAddFile

bool WINAPI SFileSetDataCompression(DWORD DataCompression)
{
    unsigned int uValidMask = (MPQ_COMPRESSION_ZLIB | MPQ_COMPRESSION_PKWARE | MPQ_COMPRESSION_BZIP2 | MPQ_COMPRESSION_SPARSE);

    if((DataCompression & uValidMask) != DataCompression)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    DefaultDataCompression = DataCompression;
    return true;
}

//-----------------------------------------------------------------------------
// Changes locale ID of a file

bool WINAPI SFileSetFileLocale(HANDLE hFile, LCID lcNewLocale)
{
    TMPQArchive * ha;
    TFileEntry * pFileEntry;
    TMPQFile * hf = IsValidFileHandle(hFile);

    // Invalid handle => do nothing
    if(hf == NULL)
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    // Do not allow to rename files in MPQ open for read only
    ha = hf->ha;
    if(ha->dwFlags & MPQ_FLAG_READ_ONLY)
    {
        SetLastError(ERROR_ACCESS_DENIED);
        return false;
    }

    // Do not allow unnamed access
    if(hf->pFileEntry->szFileName == NULL)
    {
        SetLastError(ERROR_CAN_NOT_COMPLETE);
        return false;
    }

    // Do not allow to change locale of any internal file
    if(IsInternalMpqFileName(hf->pFileEntry->szFileName))
    {
        SetLastError(ERROR_INTERNAL_FILE);
        return false;
    }

    // Do not allow changing file locales if there is no hash table
    if(hf->pHashEntry == NULL)
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return false;
    }

    // We have to check if the file+locale is not already there
    pFileEntry = GetFileEntryExact(ha, hf->pFileEntry->szFileName, lcNewLocale, NULL);
    if(pFileEntry != NULL)
    {
        SetLastError(ERROR_ALREADY_EXISTS);
        return false;
    }

    // Update the locale in the hash table entry
    hf->pHashEntry->lcLocale = (USHORT)lcNewLocale;
    ha->dwFlags |= MPQ_FLAG_CHANGED;
    return true;
}

//-----------------------------------------------------------------------------
// Sets add file callback

bool WINAPI SFileSetAddFileCallback(HANDLE hMpq, SFILE_ADDFILE_CALLBACK AddFileCB, void * pvUserData)
{
    TMPQArchive * ha = (TMPQArchive *) hMpq;

    if(!IsValidMpqHandle(hMpq))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    ha->pvAddFileUserData = pvUserData;
    ha->pfnAddFileCB = AddFileCB;
    return true;
}
