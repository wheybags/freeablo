#ifndef MIN_H
#define MIN_H

#include <string>
#include <stdint.h>
#include <vector>

namespace Level
{
    typedef std::vector<int16_t> MinPillar;

    class Min
    {
        public:
            Min(const std::string&);
            const MinPillar& operator[] (size_t index) const;
            size_t size();

        private:
            std::vector<MinPillar> mPillars;
    };
}

#endif
