#include "sol.h"

#include <faio/faio.h>

namespace Level
{
    Sol::Sol(const std::string& path)
    {
        FAIO::FAFile* solF = FAIO::FAfopen(path);
        size_t size = FAIO::FAsize(solF);
        mData.resize(size);

        FAIO::FAfread(&mData[0], 1, size, solF);

        FAIO::FAfclose(solF);
    }

    bool Sol::passable(size_t index) const
    {
        return !(mData[index] & 0x01);
    }

}
