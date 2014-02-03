#ifndef CEL_FILE_H
#define CEL_FILE_H

#include <stdint.h>
#include <vector>
#include <map>

#include "pal.h"

class CelFrame;
class Pal;

namespace FAIO
{
    class FAFile;
}

class CelFile
{
    public:
        CelFile(std::string filename);

        size_t numFrames();

        CelFrame& operator[] (size_t index);

    private:
        size_t getFrame(const std::vector<uint8_t>& frame, std::vector<Colour>& rawImage);

        void readNormalFrames(FAIO::FAFile* file);
        void readCl2ArchiveFrames(FAIO::FAFile* file);
               
        Pal getPallette(std::string filename);

        Pal mPal;

        std::vector<std::vector<uint8_t> > mFrames;

        bool mIsTileCel;
        bool mIsCl2;

        std::map<size_t, CelFrame> mCache;
};

#endif
