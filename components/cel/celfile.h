#ifndef CEL_FILE_H
#define CEL_FILE_H

#include <stdint.h>
#include <vector>
#include <map>

#include "pal.h"
#include "celframe.h"


namespace FAIO
{
    struct FAFile;
}

namespace Cel
{
    class CelFile
    {
        public:
            CelFile(std::string filename);

            size_t numFrames();

            CelFrame& operator[] (size_t index);

            size_t animLength(); ///< if normal cel file, returns same as numFrames(), for an archive, the number of frames in each subcel

        private:
            size_t getFrame(const std::vector<uint8_t>& frame, size_t frameNum, std::vector<Colour>& rawImage);

            size_t readNormalFrames(FAIO::FAFile* file);
            size_t readCl2ArchiveFrames(FAIO::FAFile* file);
                   
            Pal getPallette(std::string filename);

            Pal mPal;

            std::vector<std::vector<uint8_t> > mFrames;

            bool mIsTileCel;
            bool mIsCl2;

            std::map<size_t, CelFrame> mCache;

            size_t mAnimLength;
    };
}

#endif
