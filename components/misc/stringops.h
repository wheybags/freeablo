// Taken from OpenMW: https://github.com/zinnschlag/openmw/blob/6fd4cdb5fb94db503b5d3bf7ddc30160397862ec/components/misc/stringops.hpp
// License: GPLv3 https://github.com/zinnschlag/openmw/blob/6fd4cdb5fb94db503b5d3bf7ddc30160397862ec/GPL3.txt

#ifndef MISC_STRINGOPS_H
#define MISC_STRINGOPS_H

#include <cctype>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>

#include <faio/fafileobject.h>

namespace Misc
{
class StringUtils
{
    struct ci
    {
        bool operator()(int x, int y) const {
            return std::tolower(x) < std::tolower(y);
        }
    };

public:
    static bool ciLess(const std::string &x, const std::string &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end(), ci());
    }

    static bool ciEqual(const std::string &x, const std::string &y) {
        if (x.size() != y.size()) {
            return false;
        }
        std::string::const_iterator xit = x.begin();
        std::string::const_iterator yit = y.begin();
        for (; xit != x.end(); ++xit, ++yit) {
            if (std::tolower(*xit) != std::tolower(*yit)) {
                return false;
            }
        }
        return true;
    }

    static int ciCompareLen(const std::string &x, const std::string &y, size_t len)
    {
        std::string::const_iterator xit = x.begin();
        std::string::const_iterator yit = y.begin();
        for(;xit != x.end() && yit != y.end() && len > 0;++xit,++yit,--len)
        {
            int res = *xit - *yit;
            if(res != 0 && std::tolower(*xit) != std::tolower(*yit))
                return (res > 0) ? 1 : -1;
        }
        if(len > 0)
        {
            if(xit != x.end())
                return 1;
            if(yit != y.end())
                return -1;
        }
        return 0;
    }

    /// Transforms input string to lower case w/o copy
    static std::string &toLower(std::string &inout) {
        std::transform(
            inout.begin(),
            inout.end(),
            inout.begin(),
            (int (*)(int)) std::tolower
        );
        return inout;
    }

    /// Returns lower case copy of input string
    static std::string lowerCase(const std::string &in)
    {
        std::string out = in;
        return toLower(out);
    }

    static bool containsNonPrint(const std::string & s)
    {
        for(uint32_t i=0; i < s.length(); i++)
        {
            if(!isprint(s[i]))
                return true;

        }
        return false;
    }

    static bool endsWith(const std::string& full, const std::string& end)
    {
        return end.size() <= full.size() && full.substr(full.size() - end.size(), end.size()) == end;
    }

    static bool ciEndsWith(const std::string& full, const std::string& end)
    {
        return endsWith(lowerCase(full), lowerCase(end));
    }

    static bool startsWith(const std::string& full, const std::string& start)
    {
        return start.size() <= full.size() && full.substr(0, start.size()) == start;
    }

    static bool ciStartsWith(const std::string& full, const std::string& start)
    {
        return startsWith(lowerCase(full), lowerCase(start));
    }

    static std::string replaceEnd(const std::string& old_end, const std::string& new_end, const std::string& original)
    {
        std::string retval = original.substr(0, original.size() - old_end.size());
        retval.append(new_end);
        return retval;
    }

    static std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> elems;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            elems.push_back(item);
        }
        return elems;
    }

    static bool replaceOne(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    static void replace(std::string& str, const std::string& from, const std::string& to)
    {
        while(replaceOne(str, from, to)) {}
    }

    static std::string readAsString(const std::string& path)
    {
        FAIO::FAFileObject f(path);

        size_t size = f.FAsize();
        std::string retval;
        retval.resize(size);

        f.FAfread(&retval[0], 1, size);

        return retval;
    }
};

}
#endif
