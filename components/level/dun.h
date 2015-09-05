#ifndef DUN_FILE_H
#define DUN_FILE_H

#include <string>
#include <stdint.h>
#include <vector>

#include <misc/helper2d.h>
#include <misc/misc.h>

#include <boost/serialization/access.hpp>

namespace Level
{
    class Dun
    {

        public:
            Dun(const std::string&);
            Dun();
            Dun(size_t width, size_t height);
            
            static Dun getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4);

            Misc::Helper2D<Dun, int16_t&> operator[] (size_t x);
            Misc::Helper2D<const Dun, const int16_t&> operator[] (size_t x) const;

            size_t width() const;
            size_t height() const;
            
        private:
            void resize(size_t width, size_t height);

            std::vector<int16_t> mBlocks;
            size_t mWidth;
            size_t mHeight;

            friend const int16_t& get(size_t x, size_t y, const Dun& dun);
            friend int16_t& get(size_t x, size_t y, Dun& dun);

            friend class boost::serialization::access;

            template<class Archive>
            void serialize(Archive& ar, unsigned int version)
            {
                UNUSED_PARAM(version);

                ar & mBlocks;
                ar & mWidth & mHeight;
            }
    };
}

#endif
