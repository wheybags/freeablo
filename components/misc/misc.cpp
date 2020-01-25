#include "misc.h"

#ifndef _WIN32
#include <sstream>
#endif

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

    std::string escapeSpacesOnPath(const std::string& path)
    {
#ifdef _WIN32
        return "\"" + path + "\"";
#else
        std::stringstream ret;
        for (char c : path)
        {
            if (c == ' ')
                ret << "\\ ";
            else
                ret << c;
        }

        return ret.str();
#endif
    }
}
