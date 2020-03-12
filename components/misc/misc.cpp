#include "misc.h"
#include <iostream>
#include <misc/assert.h>

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

    std::string escapePathForShell(const std::string& path)
    {
        std::stringstream ret;

        ret << '"';
        for (char c : path)
        {
            if (c == '"')
                ret << "\\\"";
            else
                ret << c;
        }
        ret << '"';

        return ret.str();
    }

    std::string argv0;

    filesystem::path getResourcesPath()
    {
        static filesystem::path resourcesPath;

        if (resourcesPath.empty())
        {
            release_assert(!argv0.empty());

            filesystem::path thisBinary = filesystem::path(argv0).make_absolute();
            filesystem::path folder = thisBinary.parent_path();

            for (size_t i = 0; i < 5; i++)
            {
                if ((folder / "resources").exists())
                {
                    resourcesPath = folder / "resources";
                    break;
                }

                folder = folder.parent_path();
            }

            if (resourcesPath.empty())
                message_and_abort(("Unable to find resources path, binary path: " + thisBinary.str()).c_str());

            std::cout << "Using resources path: \"" << resourcesPath << "\"" << std::endl;
        }

        return resourcesPath;
    }

    void saveArgv0(const char* _argv0) { argv0 = _argv0; }
}
