// Taken from OpenMW: https://github.com/zinnschlag/openmw/blob/6fd4cdb5fb94db503b5d3bf7ddc30160397862ec/components/misc/stringops.hpp
// License: GPLv3 https://github.com/zinnschlag/openmw/blob/6fd4cdb5fb94db503b5d3bf7ddc30160397862ec/GPL3.txt

#ifndef MISC_STRINGOPS_H
#define MISC_STRINGOPS_H

#include <cctype>
#include <string>
#include <algorithm>

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
    
    static bool endsWith(const std::string& full, const std::string& end)
    {
        return end.size() <= full.size() && full.substr(full.size() - end.size(), end.size()) == end;
    }

    static bool ciEndsWith(const std::string& full, const std::string& end)
    {
        return endsWith(lowerCase(full), lowerCase(end));
    }

    static std::string replaceEnd(const std::string& old_end, const std::string& new_end, const std::string& original)
    {
        std::string retval = original.substr(0, original.size() - old_end.size());
        retval.append(new_end);
        return retval;
    }


};

}
#endif
