#include "til.h"

#include <stdio.h>

#include <faio/faio.h>

TilFile::TilFile(const std::string& filename)
{
    FAIO::FAFile* tFile = FAIO::FAfopen(filename);
    
    size_t numBlocks = FAIO::FAsize(tFile)/4; 
    
    FAIO::FAfseek(tFile, 0, SEEK_SET);
 
    TilBlock tmp(4);
    
    for(size_t i = 0; i < numBlocks; i++)
    {
        FAIO::FAfread(&tmp[0], 2, 4, tFile);
        mBlocks.push_back(tmp);
    }

    FAIO::FAfclose(tFile);
}

const TilBlock& TilFile::operator[] (size_t index) const
{
    return mBlocks[index];
}


size_t TilFile::size() const
{
    return mBlocks.size();
}

