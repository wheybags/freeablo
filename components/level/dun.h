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
            Dun(int32_t width, int32_t height);

            static Dun getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4);

            Misc::Helper2D<Dun, int32_t&> operator[] (int32_t x);
            Misc::Helper2D<const Dun, const int32_t&> operator[] (int32_t x) const;

            int32_t width() const;
            int32_t height() const;

        private:
            void resize(int32_t width, int32_t height);

            std::vector<int32_t> mBlocks;
            int32_t mWidth;
            int32_t mHeight;

            friend const int32_t& get(int32_t x, int32_t y, const Dun& dun);
            friend int32_t& get(int32_t x, int32_t y, Dun& dun);

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
