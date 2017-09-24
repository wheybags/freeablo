#include "misc.h"

#include <cmath>

#ifndef M_PI
    #define M_PI 3.142
#endif

namespace Misc
{
    std::pair<int32_t, int32_t> getNextPosByDir(std::pair<int32_t, int32_t> pos, int32_t dir)
    {
        std::pair<int32_t, int32_t> retval = pos;

        switch (dir)
        {
        case 0:
        {
            retval.first++;
            retval.second++;
            break;
        }

        case 7:
        {
            retval.first++;
            break;
        }

        case 6:
        {
            retval.first++;
            retval.second--;
            break;
        }

        case 5:
        {
            retval.second--;
            break;
        }

        case 4:
        {
            retval.first--;
            retval.second--;
            break;
        }

        case 3:
        {
            retval.first--;
            break;
        }

        case 2:
        {
            retval.first--;
            retval.second++;
            break;
        }

        case 1:
        {
            retval.second++;
            break;
        }

        default:
        {
            break;
        }
        }

        return retval;
    }

    int32_t getVecDir(const std::pair<float, float>& vector)
    {
        if(vector.first == 0 && vector.second == 0)
            return -1;

        auto angle = (std::atan2(vector.second, vector.first) / M_PI) * 180.0;

        if(angle < 0)
            angle = 360 + angle;

        if((angle <= 22.5 && angle >= 0.0) || (angle <= 360.0 && angle >= 337.5))
            return 7;
        else if(angle <= 67.5 && angle >= 22.5)
            return 0;
        else if(angle <= 112.5 && angle >= 67.5)
            return 1;
        else if(angle <= 157.5 && angle >= 112.5)
            return 2;
        else if(angle <= 202.5 && angle >= 157.5)
            return 3;
        else if(angle <= 247.5 && angle >= 202.5)
            return 4;
        else if(angle <= 292.5 && angle >= 247.5)
            return 5;
        else if(angle <= 337.5 && angle >= 292.5)
            return 6;
        return -1; // shut up compiler
    }
}
