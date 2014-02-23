#ifndef DUN_FILE_H
#define DUN_FILE_H

#include <string>
#include <stdint.h>
#include <vector>

namespace Level
{
    class Dun
    {

        class DunHelper
        {
            private:
                const Dun& parent;
                size_t x;
            public:    
                DunHelper(const Dun& _p, size_t _x) : parent(_p), x(_x) {}
                
                const int16_t& operator[](size_t y){ return parent.at(x, y); }
        };


        public:
            Dun(const std::string&);
            
            static Dun getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4);

            DunHelper operator[] (size_t x) const { return DunHelper(*this, x); }
            
            int16_t mWidth;
            int16_t mHeight;

            std::vector<int16_t> mBlocks;
        
        //TODO: sort this out
        //private:
            Dun();
            Dun(size_t width, size_t height);
            void resize(size_t width, size_t height);
            int16_t& at(size_t x, size_t y) { return mBlocks[x+y*mWidth]; }
            const int16_t& at(size_t x, size_t y) const { return mBlocks[x+y*mWidth]; }
            friend Dun getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4);
            friend class DunHelper;

    };
}

#endif
