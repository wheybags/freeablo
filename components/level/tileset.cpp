#include "tileset.h"

#include <stdio.h>

#include <faio/fafileobject.h>

namespace Level
{
    TileSet::TileSet(const std::string& filename)
    {
        FAIO::FAFileObject tFile(filename);
        
        size_t numBlocks = tFile.FAsize()/(4*2);
        
        tFile.FAfseek(0, SEEK_SET);
     
        TilBlock tmp(4);
        
        for(size_t i = 0; i < numBlocks; i++)
        {
            tFile.FAfread(&tmp[0], 2, 4);
            mBlocks.push_back(tmp);
        }
    }

    const TilBlock& TileSet::operator[] (size_t index) const
    {
        return mBlocks[index];
    }


    size_t TileSet::size() const
    {
        return mBlocks.size();
    }
}
