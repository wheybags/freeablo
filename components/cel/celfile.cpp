#include <assert.h>

#ifdef CEL_DEBUG
    #include <iostream>
#endif

#include <faio/faio.h>
#include <misc/stringops.h>

#include "celfile.h"
#include "celframe.h"

#include "tiledecoding.h"
#include "celdecoding.h"
#include "cl2decoding.h"

namespace Cel
{
    CelFile::CelFile(std::string filename) : mPal(getPallette(filename))
    {
        FAIO::FAFile* file = FAIO::FAfopen(filename);

        mIsCl2 = Misc::StringUtils::ciEndsWith(filename, "cl2");

        uint32_t first;
        FAIO::FAfread(&first, 4, 1, file);
        
        // If the first uint16_t in the file is 32,
        // then it is a cel archive, containing 8 cels,
        // each of which is a collection of frames 
        // representing an animation of an object at 
        // one of the eight possible rotations.
        // This is a side effect of cel archives containing
        // a header liek the normal cel header pointing to
        // each of the cels it contains, and there always being
        // 8 cels in each cel archive, so 8*4=32, the start
        // of the first cel
        if(first == 32)
        {
            if(mIsCl2)
            {
                mAnimLength = readCl2ArchiveFrames(file);
            }
            else
            {
                FAIO::FAfseek(file, 32, SEEK_SET);
                for(size_t i = 0; i < 8; i++)
                    mAnimLength = readNormalFrames(file);
            }
        }
        else
        {
            FAIO::FAfseek(file, 0, SEEK_SET);
            mAnimLength = readNormalFrames(file);
        }


        FAIO::FAfclose(file);
        
        mIsTileCel = isTileCel(filename);
    }


    size_t CelFile::numFrames()
    {
        return mFrames.size();
    }

    CelFrame& CelFile::operator[] (size_t index)
    {
        assert(index < numFrames());
        assert(index >= 0);

        if(mCache.count(index))
            return mCache[index];


        CelFrame frame;
        std::vector<Colour> rawImage;
        frame.mRawImage = rawImage;

        frame.mWidth = getFrame(mFrames[index], index, frame.mRawImage);

        frame.mHeight = frame.mRawImage.size() / frame.mWidth;

        mCache[index] = frame;
        
        #ifdef CEL_DEBUG
            std::cout << "w: " << frame.mWidth << ", h: " << frame.mHeight << std::endl;
        #endif

        return mCache[index];
    }

    size_t CelFile::animLength()
    {
        return mAnimLength;
    }

    size_t CelFile::getFrame(const std::vector<uint8_t>& frame, size_t frameNum, std::vector<Colour>& rawImage)
    {
        if(mIsCl2)
            return cl2Decode(frame, mPal, rawImage);

        if(mIsTileCel)
            return decodeTileFrame(frame, mPal, rawImage);

        return normalDecode(frame, frameNum, mPal, rawImage);
    }

    size_t CelFile::readCl2ArchiveFrames(FAIO::FAFile* file)
    {
        FAIO::FAfseek(file, 0, SEEK_SET);
        
        std::vector<uint32_t> headerOffsets(8);
        FAIO::FAfread(&headerOffsets[0], 4, 8, file);
        
        uint32_t numFrames;

        for(size_t i = 0; i < 8; i++)
        {
            FAIO::FAfseek(file, headerOffsets[i], SEEK_SET);

            FAIO::FAfread(&numFrames, 4, 1, file);

            std::vector<uint32_t> frameOffsets(numFrames+1);


            for(size_t j = 0; j <= numFrames; j++)
                FAIO::FAfread(&frameOffsets[j], 4, 1, file);


            FAIO::FAfseek(file, headerOffsets[i]+ frameOffsets[0], SEEK_SET);
            
            for(size_t j = 0; j < numFrames; j++)
            {
                mFrames.push_back(std::vector<uint8_t>(frameOffsets[j+1]-frameOffsets[j]));
                FAIO::FAfread(&mFrames[mFrames.size()-1][0], 1, frameOffsets[j+1]-frameOffsets[j], file);
            }
        }

        return numFrames;
    }

    size_t CelFile::readNormalFrames(FAIO::FAFile* file)
    {
        uint32_t numFrames;

        FAIO::FAfread(&numFrames, 4, 1, file);

        std::vector<uint32_t> frameOffsets(numFrames+1);


        for(size_t i = 0; i < numFrames; i++)
            FAIO::FAfread(&frameOffsets[i], 4, 1, file);

        FAIO::FAfread(&frameOffsets[numFrames], 4, 1, file);

        for(size_t i = 0; i < numFrames; i++)
        {
            mFrames.push_back(std::vector<uint8_t>(frameOffsets[i+1]-frameOffsets[i]));
            FAIO::FAfread(&mFrames[mFrames.size()-1][0], 1, frameOffsets[i+1]-frameOffsets[i], file);
        }

        return numFrames;
    }

    Pal CelFile::getPallette(std::string filename)
    {
        std::string palFilename;
        if(Misc::StringUtils::endsWith(filename, "l1.cel"))
            palFilename = Misc::StringUtils::replaceEnd("l1.cel", "l1.pal", filename);
        else if(Misc::StringUtils::endsWith(filename, "l2.cel"))
            palFilename = Misc::StringUtils::replaceEnd("l2.cel", "l2.pal", filename);
        else
            palFilename = "levels/towndata/town.pal";

        
        return Pal(palFilename);
    }
}
