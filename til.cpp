#include "til.h"

#include <stdio.h>

TilFile::TilFile(const std::string& filename)
{
    FILE* tFile = fopen(filename.c_str(), "rb");
    
    fseek(tFile, 0, SEEK_END);
    size_t numBlocks = ftell(tFile)/4; 
    
    fseek(tFile, 0, SEEK_SET);
 
    TilBlock tmp(4);
    
    for(size_t i = 0; i < numBlocks; i++)
    {
        fread(&tmp[0], 2, 4, tFile);
        mBlocks.push_back(tmp);
    }

    fclose(tFile);
}

const TilBlock& TilFile::operator[] (size_t index) const
{
    return mBlocks[index];
}


size_t TilFile::size()
{
    return mBlocks.size();
}

