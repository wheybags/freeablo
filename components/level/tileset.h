#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace Level
{
    typedef std::vector<int16_t> TilBlock;

    class TileSet
    {
    public:
        TileSet(const std::string&);
        TileSet() {}

        const TilBlock& operator[](size_t index) const;
        size_t size() const;

    private:
        std::vector<TilBlock> mBlocks;
    };
}
