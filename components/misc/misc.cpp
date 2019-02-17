#include "misc.h"

namespace Misc
{
    std::string numberToHumanFileSize(double sizeInBytes)
    {
        double size = sizeInBytes;
        std::string suffix;

        if (size > 1024 * 1024 * 1024)
        {
            size /= 1024 * 1024 * 1024;
            suffix = "G";
        }
        else if (size > 1024 * 1024)
        {
            size /= 1024 * 1024;
            suffix = "M";
        }
        else if (size > 1024)
        {
            size /= 1024;
            suffix = "K";
        }

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size;

        return ss.str() + suffix;
    }
}