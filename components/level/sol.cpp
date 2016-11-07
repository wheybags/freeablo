#include "sol.h"

#include <faio/fafileobject.h>

namespace Level
{
    Sol::Sol(const std::string& path)
    {
        FAIO::FAFileObject solF(path);
        size_t size = solF.FAsize();
        mData.resize(size);

        solF.FAfread(&mData[0], 1, size);
    }

    bool Sol::passable(size_t index) const
    {
        if(index >= size())
            return 0;
        else
            return !(mData[index] & 0x01);
    }

    size_t Sol::size() const
    {
        return mData.size();
    }
}
