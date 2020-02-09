/*****************************************************************************/
/* SFileCompactArchive.cpp                Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Archive compacting function                                               */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 14.04.03  1.00  Lad  Splitted from SFileCreateArchiveEx.cpp               */
/* 19.11.03  1.01  Dan  Big endian handling                                  */
/* 21.04.13  1.02  Dea  Compact callback now part of TMPQArchive             */
/*****************************************************************************/

#define __STORMLIB_SELF__
#include "StormLib.h"
#include "StormCommon.h"

/*****************************************************************************/
/* Local functions                                                           */
/*****************************************************************************/

static int CheckIfAllFilesKnown(TMPQArchive * ha)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;
    DWORD dwBlockIndex = 0;
    int nError = ERROR_SUCCESS;

    // Verify the file table
    if(nError == ERROR_SUCCESS)
    {
        for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++, dwBlockIndex++)
        {
            // If there is an existing entry in the file table, check its name
            if(pFileEntry->dwFlags & MPQ_FILE_EXISTS)
            {
                // The name must be valid and must not be a pseudo-name
                if(pFileEntry->szFileName == NULL || IsPseudoFileName(pFileEntry->szFileName, NULL))
                {
                    nError = ERROR_UNKNOWN_FILE_NAMES;
                    break;
                }
            }
        }
    }

    return nError;
}

static int CheckIfAllKeysKnown(TMPQArchive * ha, const TCHAR * szListFile, LPDWORD pFileKeys)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;
    DWORD dwBlockIndex = 0;
    int nError = ERROR_SUCCESS;

    // Add the listfile to the MPQ
    if(szListFile != NULL)
    {
        // Notify the user
        if(ha->pfnCompactCB != NULL)
            ha->pfnCompactCB(ha->pvCompactUserData, CCB_CHECKING_FILES, ha->CompactBytesProcessed, ha->CompactTotalBytes);

        nError = SFileAddListFile((HANDLE)ha, szListFile);
    }

    // Verify the file table
    if(nError == ERROR_SUCCESS)
    {
        for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++, dwBlockIndex++)
        {
            // If the file exists and it's encrypted
            if(pFileEntry->dwFlags & MPQ_FILE_EXISTS)
            {
                // If we know the name, we decrypt the file key from the file name
                if(pFileEntry->szFileName != NULL && !IsPseudoFileName(pFileEntry->szFileName, NULL))
                {
                    // Give the key to the caller
                    pFileKeys[dwBlockIndex] = DecryptFileKey(pFileEntry->szFileName,
                                                             pFileEntry->ByteOffset,
                                                             pFileEntry->dwFileSize,
                                                             pFileEntry->dwFlags);
                    continue;
                }

                // We don't know the encryption key of this file,
                // thus we cannot compact the file
                nError = ERROR_UNKNOWN_FILE_NAMES;
                break;
            }
        }
    }

    return nError;
}

static int CopyNonMpqData(
    TMPQArchive * ha,
    TFileStream * pSrcStream,
    TFileStream * pTrgStream,
    ULONGLONG & ByteOffset,
    ULONGLONG & ByteCount)
{
    ULONGLONG DataSize = ByteCount;
    DWORD dwToRead;
    char DataBuffer[0x1000];
    int nError = ERROR_SUCCESS;

    // Copy the data
    while(DataSize > 0)
    {
        // Get the proper size of data
        dwToRead = sizeof(DataBuffer);
        if(DataSize < dwToRead)
            dwToRead = (DWORD)DataSize;

        // Read from the source stream
        if(!FileStream_Read(pSrcStream, &ByteOffset, DataBuffer, dwToRead))
        {
            nError = GetLastError();
            break;
        }

        // Write to the target stream
        if(!FileStream_Write(pTrgStream, NULL, DataBuffer, dwToRead))
        {
            nError = GetLastError();
            break;
        }

        // Update the progress
        if(ha->pfnCompactCB != NULL)
        {
            ha->CompactBytesProcessed += dwToRead;
            ha->pfnCompactCB(ha->pvCompactUserData, CCB_COPYING_NON_MPQ_DATA, ha->CompactBytesProcessed, ha->CompactTotalBytes);
        }

        // Decrement the number of data to be copied
        ByteOffset += dwToRead;
        DataSize -= dwToRead;
    }

    return nError;
}

// Copies all file sectors into another archive.
static int CopyMpqFileSectors(
    TMPQArchive * ha,
    TMPQFile * hf,
    TFileStream * pNewStream,
    ULONGLONG MpqFilePos)               // MPQ file position in the new archive
{
    TFileEntry * pFileEntry = hf->pFileEntry;
    ULONGLONG RawFilePos;               // Used for calculating sector offset in the old MPQ archive
    DWORD dwBytesToCopy = pFileEntry->dwCmpSize;
    DWORD dwPatchSize = 0;              // Size of patch header
    DWORD dwFileKey1 = 0;               // File key used for decryption
    DWORD dwFileKey2 = 0;               // File key used for encryption
    DWORD dwCmpSize = 0;                // Compressed file size, including patch header
    int nError = ERROR_SUCCESS;

    // Resolve decryption keys. Note that the file key given 
    // in the TMPQFile structure also includes the key adjustment
    if(nError == ERROR_SUCCESS && (pFileEntry->dwFlags & MPQ_FILE_ENCRYPTED))
    {
        dwFileKey2 = dwFileKey1 = hf->dwFileKey;
        if(pFileEntry->dwFlags & MPQ_FILE_FIX_KEY)
        {
            dwFileKey2 = (dwFileKey1 ^ pFileEntry->dwFileSize) - (DWORD)pFileEntry->ByteOffset;
            dwFileKey2 = (dwFileKey2 + (DWORD)MpqFilePos) ^ pFileEntry->dwFileSize;
        }
    }

    // If we have to save patch header, do it
    if(nError == ERROR_SUCCESS && hf->pPatchInfo != NULL)
    {
        BSWAP_ARRAY32_UNSIGNED(hf->pPatchInfo, sizeof(DWORD) * 3);
        if(!FileStream_Write(pNewStream, NULL, hf->pPatchInfo, hf->pPatchInfo->dwLength))
            nError = GetLastError();

        // Save the size of the patch info
        dwPatchSize = hf->pPatchInfo->dwLength;
    }

    // If we have to save sector offset table, do it.
    if(nError == ERROR_SUCCESS && hf->SectorOffsets != NULL)
    {
        DWORD * SectorOffsetsCopy = STORM_ALLOC(DWORD, hf->SectorOffsets[0] / sizeof(DWORD));
        DWORD dwSectorOffsLen = hf->SectorOffsets[0];

        assert((pFileEntry->dwFlags & MPQ_FILE_SINGLE_UNIT) == 0);
        assert(pFileEntry->dwFlags & MPQ_FILE_COMPRESS_MASK);

        if(SectorOffsetsCopy == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;

        // Encrypt the secondary sector offset table and write it to the target file
        if(nError == ERROR_SUCCESS)
        {
            memcpy(SectorOffsetsCopy, hf->SectorOffsets, dwSectorOffsLen);
            if(pFileEntry->dwFlags & MPQ_FILE_ENCRYPTED)
                EncryptMpqBlock(SectorOffsetsCopy, dwSectorOffsLen, dwFileKey2 - 1);

            BSWAP_ARRAY32_UNSIGNED(SectorOffsetsCopy, dwSectorOffsLen);
            if(!FileStream_Write(pNewStream, NULL, SectorOffsetsCopy, dwSectorOffsLen))
                nError = GetLastError();

            dwBytesToCopy -= dwSectorOffsLen;
            dwCmpSize += dwSectorOffsLen;
        }

        // Update compact progress
        if(ha->pfnCompactCB != NULL)
        {
            ha->CompactBytesProcessed += dwSectorOffsLen;
            ha->pfnCompactCB(ha->pvCompactUserData, CCB_COMPACTING_FILES, ha->CompactBytesProcessed, ha->CompactTotalBytes);
        }

        STORM_FREE(SectorOffsetsCopy);
    }

    // Now we have to copy all file sectors. We do it without
    // recompression, because recompression is not necessary in this case
    if(nError == ERROR_SUCCESS)
    {
        for(DWORD dwSector = 0; dwSector < hf->dwSectorCount; dwSector++)
        {
            DWORD dwRawDataInSector = hf->dwSectorSize;
            DWORD dwRawByteOffset = dwSector * hf->dwSectorSize;

            // Fix the raw data length if the file is compressed
            if(hf->SectorOffsets != NULL)
            {
                dwRawDataInSector = hf->SectorOffsets[dwSector+1] - hf->SectorOffsets[dwSector];
                dwRawByteOffset = hf->SectorOffsets[dwSector];
            }

            // Last sector: If there is not enough bytes remaining in the file, cut the raw size
            if(dwRawDataInSector > dwBytesToCopy)
                dwRawDataInSector = dwBytesToCopy;

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
            if((pFileEntry->dwFlags & MPQ_FILE_ENCRYPTED) && dwFileKey1 != dwFileKey2)
            {
                BSWAP_ARRAY32_UNSIGNED(hf->pbFileSector, dwRawDataInSector);
                DecryptMpqBlock(hf->pbFileSector, dwRawDataInSector, dwFileKey1 + dwSector);
                EncryptMpqBlock(hf->pbFileSector, dwRawDataInSector, dwFileKey2 + dwSector);
                BSWAP_ARRAY32_UNSIGNED(hf->pbFileSector, dwRawDataInSector);
            }

            // Now write the sector back to the file
            if(!FileStream_Write(pNewStream, NULL, hf->pbFileSector, dwRawDataInSector))
            {
                nError = GetLastError();
                break;
            }

            // Update compact progress
            if(ha->pfnCompactCB != NULL)
            {
                ha->CompactBytesProcessed += dwRawDataInSector;
                ha->pfnCompactCB(ha->pvCompactUserData, CCB_COMPACTING_FILES, ha->CompactBytesProcessed, ha->CompactTotalBytes);
            }

            // Adjust byte counts
            dwBytesToCopy -= dwRawDataInSector;
            dwCmpSize += dwRawDataInSector;
        }
    }

    // Copy the sector CRCs, if any
    // Sector CRCs are always compressed (not imploded) and unencrypted
    if(nError == ERROR_SUCCESS && hf->SectorOffsets != NULL && hf->SectorChksums != NULL)
    {
        DWORD dwCrcLength;

        dwCrcLength = hf->SectorOffsets[hf->dwSectorCount + 1] - hf->SectorOffsets[hf->dwSectorCount];
        if(dwCrcLength != 0)
        {
            if(!FileStream_Read(ha->pStream, NULL, hf->SectorChksums, dwCrcLength))
                nError = GetLastError();

            if(!FileStream_Write(pNewStream, NULL, hf->SectorChksums, dwCrcLength))
                nError = GetLastError();

            // Update compact progress
            if(ha->pfnCompactCB != NULL)
            {
                ha->CompactBytesProcessed += dwCrcLength;
                ha->pfnCompactCB(ha->pvCompactUserData, CCB_COMPACTING_FILES, ha->CompactBytesProcessed, ha->CompactTotalBytes);
            }

            // Size of the CRC block is also included in the compressed file size
            dwBytesToCopy -= dwCrcLength;
            dwCmpSize += dwCrcLength;
        }
    }

    // There might be extra data beyond sector checksum table
    // Sometimes, these data are even part of sector offset table
    // Examples:
    // 2012 - WoW\15354\locale-enGB.MPQ:DBFilesClient\SpellLevels.dbc
    // 2012 - WoW\15354\locale-enGB.MPQ:Interface\AddOns\Blizzard_AuctionUI\Blizzard_AuctionUI.xml
    if(nError == ERROR_SUCCESS && dwBytesToCopy != 0)
    {
        LPBYTE pbExtraData;

        // Allocate space for the extra data
        pbExtraData = STORM_ALLOC(BYTE, dwBytesToCopy);
        if(pbExtraData != NULL)
        {
            if(!FileStream_Read(ha->pStream, NULL, pbExtraData, dwBytesToCopy))
                nError = GetLastError();

            if(!FileStream_Write(pNewStream, NULL, pbExtraData, dwBytesToCopy))
                nError = GetLastError();

            // Include these extra data in the compressed size
            dwCmpSize += dwBytesToCopy;
            STORM_FREE(pbExtraData);
        }
        else
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Write the MD5's of the raw file data, if needed
    if(nError == ERROR_SUCCESS && ha->pHeader->dwRawChunkSize != 0)
    {
        nError = WriteMpqDataMD5(pNewStream, 
                                 ha->MpqPos + MpqFilePos,
                                 pFileEntry->dwCmpSize,
                                 ha->pHeader->dwRawChunkSize);
    }

    // Verify the number of bytes written
    if(nError == ERROR_SUCCESS)
    {
        // At this point, number of bytes written should be exactly
        // the same like the compressed file size. If it isn't,
        // there's something wrong (an unknown archive version, MPQ malformation, ...)
        // 
        // Note: Diablo savegames have very weird layout, and the file "hero"
        // seems to have improper compressed size. Instead of real compressed size,
        // the "dwCmpSize" member of the block table entry contains
        // uncompressed size of file data + size of the sector table.
        // If we compact the archive, Diablo will refuse to load the game
        //
        // Note: Some patch files in WOW patches don't count the patch header
        // into compressed size
        //

        if(!(dwCmpSize <= pFileEntry->dwCmpSize && pFileEntry->dwCmpSize <= dwCmpSize + dwPatchSize))
        {
            nError = ERROR_FILE_CORRUPT;
            assert(false);
        }
    }

    return nError;
}

static int CopyMpqFiles(TMPQArchive * ha, LPDWORD pFileKeys, TFileStream * pNewStream)
{
    TFileEntry * pFileTableEnd = ha->pFileTable + ha->dwFileTableSize;
    TFileEntry * pFileEntry;
    TMPQFile * hf = NULL;
    ULONGLONG MpqFilePos;
    int nError = ERROR_SUCCESS;

    // Walk through all files and write them to the destination MPQ archive
    for(pFileEntry = ha->pFileTable; pFileEntry < pFileTableEnd; pFileEntry++)
    {
        // Copy all the file sectors
        // Only do that when the file has nonzero size
        if((pFileEntry->dwFlags & MPQ_FILE_EXISTS))
        {
            // Query the position where the destination file will be
            FileStream_GetPos(pNewStream, &MpqFilePos);
            MpqFilePos = MpqFilePos - ha->MpqPos;

            // Perform file copy ONLY if the file has nonzero size
            if(pFileEntry->dwFileSize != 0)
            {
                // Allocate structure for the MPQ file
                hf = CreateFileHandle(ha, pFileEntry);
                if(hf == NULL)
                    return ERROR_NOT_ENOUGH_MEMORY;

                // Set the file decryption key
                hf->dwFileKey = pFileKeys[pFileEntry - ha->pFileTable];

                // If the file is a patch file, load the patch header
                if(pFileEntry->dwFlags & MPQ_FILE_PATCH_FILE)
                {
                    nError = AllocatePatchInfo(hf, true);
                    if(nError != ERROR_SUCCESS)
                        break;
                }

                // Allocate buffers for file sector and sector offset table
                nError = AllocateSectorBuffer(hf);
                if(nError != ERROR_SUCCESS)
                    break;

                // Also allocate sector offset table and sector checksum table
                nError = AllocateSectorOffsets(hf, true);
                if(nError != ERROR_SUCCESS)
                    break;

                // Also load sector checksums, if any
                if(pFileEntry->dwFlags & MPQ_FILE_SECTOR_CRC)
                {
                    nError = AllocateSectorChecksums(hf, false);
                    if(nError != ERROR_SUCCESS)
                        break;
                }

                // Copy all file sectors
                nError = CopyMpqFileSectors(ha, hf, pNewStream, MpqFilePos);
                if(nError != ERROR_SUCCESS)
                    break;

                // Free buffers. This also sets "hf" to NULL.
                FreeFileHandle(hf);
            }

            // Note: DO NOT update the compressed size in the file entry, no matter how bad it is.
            pFileEntry->ByteOffset = MpqFilePos;
        }
    }

    // Cleanup and exit
    if(hf != NULL)
        FreeFileHandle(hf);
    return nError;
}

/*****************************************************************************/
/* Public functions                                                          */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// Changing hash table size

DWORD WINAPI SFileGetMaxFileCount(HANDLE hMpq)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;

    return ha->dwMaxFileCount;
}

bool WINAPI SFileSetMaxFileCount(HANDLE hMpq, DWORD dwMaxFileCount)
{
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    DWORD dwNewHashTableSize = 0;
    int nError = ERROR_SUCCESS;

    // Test the valid parameters
    if(!IsValidMpqHandle(hMpq))
        nError = ERROR_INVALID_HANDLE;
    if(ha->dwFlags & MPQ_FLAG_READ_ONLY)
        nError = ERROR_ACCESS_DENIED;
    if(dwMaxFileCount < ha->dwFileTableSize)
        nError = ERROR_DISK_FULL;

    // ALL file names must be known in order to be able to rebuild hash table
    if(nError == ERROR_SUCCESS && ha->pHashTable != NULL)
    {
        nError = CheckIfAllFilesKnown(ha);
        if(nError == ERROR_SUCCESS)
        {
            // Calculate the hash table size for the new file limit
            dwNewHashTableSize = GetNearestPowerOfTwo(dwMaxFileCount);

            // Rebuild both file tables
            nError = RebuildFileTable(ha, dwNewHashTableSize);
        }
    }

    // We always have to rebuild the (attributes) file due to file table change
    if(nError == ERROR_SUCCESS)
    {
        // Invalidate (listfile) and (attributes)
        InvalidateInternalFiles(ha);

        // Rebuild the HET table, if we have any
        if(ha->pHetTable != NULL)
            nError = RebuildHetTable(ha);
    }

    // Return the error
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// Archive compacting

bool WINAPI SFileSetCompactCallback(HANDLE hMpq, SFILE_COMPACT_CALLBACK pfnCompactCB, void * pvUserData)
{
    TMPQArchive * ha = (TMPQArchive *) hMpq;

    if (!IsValidMpqHandle(hMpq))
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return false;
    }

    ha->pfnCompactCB = pfnCompactCB;
    ha->pvCompactUserData = pvUserData;
    return true;
}

bool WINAPI SFileCompactArchive(HANDLE hMpq, const TCHAR * szListFile, bool /* bReserved */)
{
    TFileStream * pTempStream = NULL;
    TMPQArchive * ha = (TMPQArchive *)hMpq;
    ULONGLONG ByteOffset;
    ULONGLONG ByteCount;
    LPDWORD pFileKeys = NULL;
    TCHAR szTempFile[MAX_PATH+1] = _T("");
    int nError = ERROR_SUCCESS;

    // Test the valid parameters
    if(!IsValidMpqHandle(hMpq))
        nError = ERROR_INVALID_HANDLE;
    if(ha->dwFlags & MPQ_FLAG_READ_ONLY)
        nError = ERROR_ACCESS_DENIED;

    // If the MPQ is changed at this moment, we have to flush the archive
    if(nError == ERROR_SUCCESS && (ha->dwFlags & MPQ_FLAG_CHANGED))
    {
        SFileFlushArchive(hMpq);
    }

    // Create the table with file keys
    if(nError == ERROR_SUCCESS)
    {
        if((pFileKeys = STORM_ALLOC(DWORD, ha->dwFileTableSize)) != NULL)
            memset(pFileKeys, 0, sizeof(DWORD) * ha->dwFileTableSize);
        else
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // First of all, we have to check of we are able to decrypt all files.
    // If not, sorry, but the archive cannot be compacted.
    if(nError == ERROR_SUCCESS)
    {
        // Initialize the progress variables for compact callback
        FileStream_GetSize(ha->pStream, &(ha->CompactTotalBytes));
        ha->CompactBytesProcessed = 0;
        nError = CheckIfAllKeysKnown(ha, szListFile, pFileKeys);
    }

    // Get the temporary file name and create it
    if(nError == ERROR_SUCCESS)
    {
        // Create temporary file name. Prevent buffer overflow
        StringCopy(szTempFile, _countof(szTempFile), FileStream_GetFileName(ha->pStream));
        StringCat(szTempFile, _countof(szTempFile), _T(".tmp"));

        // Create temporary file
        pTempStream = FileStream_CreateFile(szTempFile, STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE);
        if(pTempStream == NULL)
            nError = GetLastError();
    }

    // Write the data before MPQ user data (if any)
    if(nError == ERROR_SUCCESS && ha->UserDataPos != 0)
    {
        // Inform the application about the progress
        if(ha->pfnCompactCB != NULL)
            ha->pfnCompactCB(ha->pvCompactUserData, CCB_COPYING_NON_MPQ_DATA, ha->CompactBytesProcessed, ha->CompactTotalBytes);

        ByteOffset = 0;
        ByteCount = ha->UserDataPos;
        nError = CopyNonMpqData(ha, ha->pStream, pTempStream, ByteOffset, ByteCount);
    }

    // Write the MPQ user data (if any)
    if(nError == ERROR_SUCCESS && ha->MpqPos > ha->UserDataPos)
    {
        // At this point, we assume that the user data size is equal
        // to pUserData->dwHeaderOffs.
        // If this assumption doesn't work, then we have an unknown version of MPQ
        ByteOffset = ha->UserDataPos;
        ByteCount = ha->MpqPos - ha->UserDataPos;

        assert(ha->pUserData != NULL);
        assert(ha->pUserData->dwHeaderOffs == ByteCount);
        nError = CopyNonMpqData(ha, ha->pStream, pTempStream, ByteOffset, ByteCount);
    }

    // Write the MPQ header
    if(nError == ERROR_SUCCESS)
    {
        TMPQHeader SaveMpqHeader;

        // Write the MPQ header to the file
        memcpy(&SaveMpqHeader, ha->pHeader, ha->pHeader->dwHeaderSize);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_1);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_2);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_3);
        BSWAP_TMPQHEADER(&SaveMpqHeader, MPQ_FORMAT_VERSION_4);
        if(!FileStream_Write(pTempStream, NULL, &SaveMpqHeader, ha->pHeader->dwHeaderSize))
            nError = GetLastError();

        // Update the progress
        ha->CompactBytesProcessed += ha->pHeader->dwHeaderSize;
    }

    // Now copy all files
    if(nError == ERROR_SUCCESS)
        nError = CopyMpqFiles(ha, pFileKeys, pTempStream);

    // If succeeded, switch the streams
    if(nError == ERROR_SUCCESS)
    {
        ha->dwFlags |= MPQ_FLAG_CHANGED;
        if(FileStream_Replace(ha->pStream, pTempStream))
            pTempStream = NULL;
        else
            nError = ERROR_CAN_NOT_COMPLETE;
    }

    // Final user notification
    if(nError == ERROR_SUCCESS && ha->pfnCompactCB != NULL)
    {
        ha->CompactBytesProcessed += (ha->pHeader->dwHashTableSize * sizeof(TMPQHash));
        ha->CompactBytesProcessed += (ha->dwFileTableSize * sizeof(TMPQBlock));
        ha->pfnCompactCB(ha->pvCompactUserData, CCB_CLOSING_ARCHIVE, ha->CompactBytesProcessed, ha->CompactTotalBytes);
    }

    // Cleanup and return
    if(pTempStream != NULL)
        FileStream_Close(pTempStream);
    if(pFileKeys != NULL)
        STORM_FREE(pFileKeys);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}
