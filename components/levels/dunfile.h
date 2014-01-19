#ifndef DUN_FILE_H
#define DUN_FILE_H

#include <string>
#include <stdint.h>
#include <vector>

class DunFile
{

    class DunFileHelper
    {
        private:
            const DunFile& parent;
            size_t x;
        public:    
            DunFileHelper(const DunFile& _p, size_t _x) : parent(_p), x(_x) {}
            
            const int16_t& operator[](size_t y){ return parent.at(x, y); }
    };


    public:
        DunFile(const std::string&);
        
        static DunFile getTown(const DunFile& sector1, const DunFile& sector2, const DunFile& sector3, const DunFile& sector4);

        DunFileHelper operator[] (size_t x) const { return DunFileHelper(*this, x); }
        
        int16_t mWidth;
        int16_t mHeight;

        std::vector<int16_t> mBlocks;
    
    //TODO: sort this out
    //private:
        DunFile();
        DunFile(size_t width, size_t height);
        void resize(size_t width, size_t height);
        int16_t& at(size_t x, size_t y) { return mBlocks[x+y*mWidth]; }
        const int16_t& at(size_t x, size_t y) const { return mBlocks[x+y*mWidth]; }
        friend DunFile getTown(const DunFile& sector1, const DunFile& sector2, const DunFile& sector3, const DunFile& sector4);
        friend class DunFileHelper;

};

#endif
