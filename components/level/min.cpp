#include "min.h"

#include <stdio.h>
#include <iostream>

#include <faio/fafileobject.h>
#include <misc/stringops.h>

namespace Level
{
    Min::Min(const std::string& filename)
    {
        FAIO::FAFileObject minF(filename);

        size_t minSize;
        // These two files contain 16 blocks, all else are 10. Nothing to do but a workaround...
        if(Misc::StringUtils::endsWith(filename, "l4.min") || Misc::StringUtils::endsWith(filename, "town.min"))
            minSize = 16;
        else
            minSize = 10;

        size_t numPillars = minF.FAsize()/(minSize*2);
        
        minF.FAfseek(0, SEEK_SET);
        
        std::vector<int16_t> temp(minSize);
        
        for(size_t i = 0; i < numPillars; i++)
        {
            minF.FAfread(&temp[0], 2, minSize);
            mPillars.push_back(std::vector<int16_t>(temp));
        }
    }
         
    const std::vector<int16_t>& Min::operator[] (size_t index) const
    {
        return mPillars[index];
    }

    size_t Min::size() const
    {
        return mPillars.size();
    }
}
