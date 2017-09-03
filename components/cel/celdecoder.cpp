#include <iostream>
#include <set>
#include <functional>
#include <misc/stringops.h>
#include <faio/fafileobject.h>
#include "celdecoder.h"

namespace Cel
{
    Settings::Settings CelDecoder::mSettingsCel;
    Settings::Settings CelDecoder::mSettingsCl2;

    CelDecoder::CelDecoder(const std::string& celPath)
        : mCelPath(celPath),
          mAnimationLength(0)
    {
        readCelName();
        readConfiguration();
        readPalette();
        getFrames();
    }

    CelFrame& CelDecoder::operator [](int32_t index) {

        if(mCache.count(index)) {
            return mCache[index];
        }

        auto& frame = mFrames[index];
        CelFrame celFrame;
        decodeFrame(index, frame, celFrame);
        mCache[index] = celFrame;

        return mCache[index];
    }

    int32_t CelDecoder::numFrames() const
    {
        return mFrames.size();
    }

    int32_t CelDecoder::animationLength() const
    {
        return mAnimationLength;
    }

    void CelDecoder::readCelName()
    {
        if(mCelPath.size() == 0) {
            throw "Cel path is empty";
        }

        std::vector<std::string> celPathComponents;

        if(mCelPath.find_first_of('/') != std::string::npos) {
            celPathComponents = Misc::StringUtils::split(mCelPath, '/');
        } else {
            celPathComponents = Misc::StringUtils::split(mCelPath, '\\');
        }

        mCelName = celPathComponents[celPathComponents.size() - 1];
        mCelName = Misc::StringUtils::toLower(mCelName);
    }

    void CelDecoder::readConfiguration()
    {
        static bool isConfigurationRead = false;

        if(!isConfigurationRead) {
            mSettingsCel.loadFromFile("resources/cel.ini");
            mSettingsCl2.loadFromFile("resources/cl2.ini");
            isConfigurationRead = true;
        }

        Settings::Settings* settings = &mSettingsCel;
        std::string celNameWithoutExtension = mCelName;
        std::string extension = "cel";

        if(Misc::StringUtils::ciEndsWith(mCelPath,"cl2")) {
            settings = &mSettingsCl2;
            extension = "cl2";
            mIsCl2 = true;
        }

        int32_t pos = celNameWithoutExtension.find_last_of(extension) - 3;
        celNameWithoutExtension = celNameWithoutExtension.substr(0, pos);

        // If more than one image in cel
        // read configuration from first image
        // (temporary solution)

        mImageCount = settings->get<int>(celNameWithoutExtension, "image_count");
        if(mImageCount > 0) {
            celNameWithoutExtension += "0";
        }

        mFrameWidth = settings->get<int>(celNameWithoutExtension, "width");
        mFrameHeight= settings->get<int>(celNameWithoutExtension, "height");
        mHeaderSize = settings->get<int>(celNameWithoutExtension, "header_size", 0);
        mIsObjcursCel = mCelName == "objcurs.cel";
        mIsCharbutCel = mCelName == "charbut.cel";
    }

    void CelDecoder::readPalette()
    {
        std::string& filename = mCelPath;
        std::string palFilename;
        if(Misc::StringUtils::startsWith(filename, "levels") && Misc::StringUtils::endsWith(filename, "l1.cel"))
            palFilename = Misc::StringUtils::replaceEnd("l1.cel", "l1.pal", filename);
        else if (Misc::StringUtils::startsWith(filename, "levels") && Misc::StringUtils::endsWith(filename, "l2.cel"))
            palFilename = Misc::StringUtils::replaceEnd("l2.cel", "l2.pal", filename);
        else if(Misc::StringUtils::startsWith(filename, "levels") && Misc::StringUtils::endsWith(filename, "l3.cel"))
            palFilename = Misc::StringUtils::replaceEnd("l3.cel", "l3.pal", filename);
        else if(Misc::StringUtils::startsWith(filename, "levels") && Misc::StringUtils::endsWith(filename, "l4.cel"))
            palFilename = Misc::StringUtils::replaceEnd("l4.cel", "l4_1.pal", filename);
        else if (Misc::StringUtils::startsWith(Misc::StringUtils::lowerCase(filename), "gendata"))
            palFilename = Misc::StringUtils::replaceEnd(".cel", ".pal", filename);
        else
            palFilename = "levels/towndata/town.pal";

        mPal = Pal(palFilename);
    }

    void CelDecoder::decode()
    {
        int frameNumber = 0;
        for(FrameBytesRef frame : mFrames) {

            if(mCache.count(frameNumber)) {
                frameNumber++;
                continue;
            }

            CelFrame celFrame;
            decodeFrame(frameNumber, frame, celFrame);
            mCache[frameNumber] = celFrame;

            frameNumber++;
        }
    }

    void CelDecoder::getFrames()
    {
        // Open CEL file.

        FAIO::FAFileObject file(mCelPath);
        if(!file.isValid()) {
            return;
        }

        // Read first word.
        uint32_t frameCount = 0;
        uint32_t firstWord = 0;
        uint32_t repeat = 1;
        file.FAfread(&firstWord, 4, 1);

        std::vector<uint32_t> headerOffsets;

        // If firstWord == 32 then it is archive
        // that contains 8 cels and information about offsets in header.

        file.FAfseek(0, SEEK_SET);

        if(firstWord == 32) {
            repeat = 8;

            // Read header offsets
            for(uint32_t i = 0; i < repeat ; i++)
            {
                uint32_t offset = 0;
                file.FAfread(&offset, 4, 1);
                headerOffsets.push_back(offset);
            }
        }

        for(uint32_t r = 0; r < repeat ; r++)
        {
            // Offset file
            if(!headerOffsets.empty()) {
                file.FAfseek(headerOffsets[r], SEEK_SET);
            }

            // Read frame count
            file.FAfread(&frameCount, 4, 1);

            // Read frame offsets.
            std::vector<uint32_t> frameOffsets(frameCount+1);
            for(uint32_t i = 0 ; i < frameCount + 1; i++)
            {
                file.FAfread(&frameOffsets[i], 4, 1);
            }

            // Magic offset that fixes everything!
            if(!headerOffsets.empty()) {
                file.FAfseek( headerOffsets[r] + frameOffsets[0], SEEK_SET);
            }

            // Read frame contents
            for(uint32_t i = 0 ; i < frameCount ; i++)
            {
                int64_t frameStart = int64_t(frameOffsets[i]) + mHeaderSize;
                int64_t frameEnd = int64_t(frameOffsets[i+1]);
                int64_t frameSize = frameEnd - frameStart;

                if(frameSize < 0) {
                    return;
                }

                mFrames.push_back(std::vector<uint8_t>(static_cast<int32_t> (frameSize)));
                uint32_t idx = mFrames.size() - 1;
                file.FAfseek(mHeaderSize, SEEK_CUR);
                file.FAfread(&mFrames[idx][0], 1, static_cast<int32_t> (frameSize));
            }

            mAnimationLength = frameCount;
        }
    }

    void CelDecoder::decodeFrame(int32_t index, FrameBytesRef frame, CelFrame& celFrame) {
        auto decoder = getFrameDecoder(mCelName, frame, index);

        if(mIsObjcursCel) {
            setObjcursCelDimensions(index);
        } else if(mIsCharbutCel) {
            setCharbutCelDimensions(index);
        }

        celFrame.mWidth = mFrameWidth;
        celFrame.mHeight = mFrameHeight;

        decoder(*this, frame, mPal, celFrame.mRawImage);
    }

    CelDecoder::FrameDecoder CelDecoder::getFrameDecoder(const std::string& celName, FrameBytesRef frame, int frameNumber)
    {
        static std::set<std::string> filenames = { "l1.cel", "l2.cel", "l3.cel", "l4.cel", "town.cel" };
        int frameSize = frame.size();
        bool isInFilenames = filenames.find(celName) != filenames.end();

        if(isInFilenames)
        {
            switch(frameSize)
            {
            case 0x400:
                if(isType0(celName, frameNumber))
                    return &CelDecoder::decodeFrameType0;
                break;
            case 0x220:
                if(isType2or4(frame)) {
                    return &CelDecoder::decodeFrameType2;
                } else if(isType3or5(frame)) {
                    return &CelDecoder::decodeFrameType3;
                }
            case 0x320:
                if(isType2or4(frame)) {
                    return &CelDecoder::decodeFrameType4;
                } else if(isType3or5(frame)) {
                    return &CelDecoder::decodeFrameType5;
                }
            }
        } else if(Misc::StringUtils::endsWith(celName,"cl2"))
        {
            return &CelDecoder::decodeFrameType6;
        }

        return &CelDecoder::decodeFrameType1;
    }

    // isType0 returns true if the image is a plain 32x32.
    bool CelDecoder::isType0(const std::string& celName, int frameNumber)
    {
        std::set<int> numbers;

        if(celName == "l1.cel") {
            numbers = { 148, 159, 181, 186, 188 };
        } else if(celName == "l2.cel") {
            numbers = { 47, 1397, 1399, 1411 };
        } else if(celName == "l4.cel") {
            numbers = { 336, 639 };
        } else if(celName == "town.cel") {
            numbers = { 2328, 2367, 2593 };
        }

        if(numbers.find(frameNumber) != numbers.end()) {
            return false;
        }

        return true;
    }

    // isType2or4 returns true if the image is a triangle or a trapezoid pointing to
    // the left.
    bool CelDecoder::isType2or4(FrameBytesRef frame) {

        std::vector<int> zeroPositions = {0, 1, 8, 9, 24, 25, 48, 49, 80, 81, 120, 121, 168, 169, 224, 225};
        for(int i : zeroPositions)
        {
            if(frame[i] != 0) {
                return false;
            }
        }

        return true;
    }

    // isType3or5 returns true if the image is a triangle or a trapezoid pointing to
    // the right.
    bool CelDecoder::isType3or5(FrameBytesRef frame) {

        std::vector<int> zeroPositions = {2, 3, 14, 15, 34, 35, 62, 63, 98, 99, 142, 143, 194, 195, 254, 255};
        for(int i : zeroPositions)
        {
            if(frame[i] != 0) {
                return false;
            }
        }

        return true;
    }

    // DecodeFrameType0 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Range through the frame, one byte at the time.
    //       - Each byte corresponds to a color index of the palette.
    //       - Set one regular pixel per byte, using the color index to locate the
    //         color in the palette.
    //
    // Type0 corresponds to a plain 32x32 images, with no transparency.
    //
    void CelDecoder::decodeFrameType0(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        int32_t len = frame.size();
        for(int32_t pos = 0 ; pos < len ; pos++)
        {
            Colour color = pal[frame[pos]];
            decodedFrame.push_back(color);
        }
    }

    // DecodeFrameType1 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Read one byte (chunkSize).
    //    2) If chunkSize is negative, set that many transparent pixels.
    //    3) If chunkSize is positive, read that many bytes.
    //       - Each byte read this way corresponds to a color index of the palette.
    //       - Set one regular pixel per byte, using the color index to locate the
    //         color in the palette.
    //    4) goto 1 until EOF is reached.
    //
    // Type1 corresponds to a regular CEL frame image of the specified dimensions.
    //
    void CelDecoder::decodeFrameType1(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        int32_t len = frame.size();
        for(int32_t pos = 0 ; pos < len ;)
        {
            int chunkSize = int(int8_t(frame[pos]));
            pos++;
            if (chunkSize < 0) {
                // Transparent pixels.
                for (int i = 0; i > chunkSize; i--) {
                    Colour color(0, 0, 0, false);
                    decodedFrame.push_back(color);
                }
            } else {
                // Regular pixels.
                for (int i = 0; i < chunkSize; i++) {

                    Colour color = pal[frame[pos]];
                    decodedFrame.push_back(color);
                    pos++;
                }
            }
        }
    }

    // DecodeFrameType2 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Dump one line of 32 pixels at the time.
    //       - The illustration below tells if a pixel is transparent or regular.
    //       - Only regular and zero (transparent) pixels are explicitly stored in
    //         the frame content. All other pixels of the illustration are
    //         implicitly transparent.
    //
    // Below is an illustration of the 32x32 image, where a space represents an
    // implicit transparent pixel, a '0' represents an explicit transparent pixel
    // and an 'x' represents an explicit regular pixel.
    //
    //    +--------------------------------+
    //    |                                |
    //    |                            00xx|
    //    |                            xxxx|
    //    |                        00xxxxxx|
    //    |                        xxxxxxxx|
    //    |                    00xxxxxxxxxx|
    //    |                    xxxxxxxxxxxx|
    //    |                00xxxxxxxxxxxxxx|
    //    |                xxxxxxxxxxxxxxxx|
    //    |            00xxxxxxxxxxxxxxxxxx|
    //    |            xxxxxxxxxxxxxxxxxxxx|
    //    |        00xxxxxxxxxxxxxxxxxxxxxx|
    //    |        xxxxxxxxxxxxxxxxxxxxxxxx|
    //    |    00xxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |    xxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |00xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |00xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |    xxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |    00xxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |        xxxxxxxxxxxxxxxxxxxxxxxx|
    //    |        00xxxxxxxxxxxxxxxxxxxxxx|
    //    |            xxxxxxxxxxxxxxxxxxxx|
    //    |            00xxxxxxxxxxxxxxxxxx|
    //    |                xxxxxxxxxxxxxxxx|
    //    |                00xxxxxxxxxxxxxx|
    //    |                    xxxxxxxxxxxx|
    //    |                    00xxxxxxxxxx|
    //    |                        xxxxxxxx|
    //    |                        00xxxxxx|
    //    |                            xxxx|
    //    |                            00xx|
    //    +--------------------------------+
    //
    // Type2 corresponds to a 32x32 images of a left facing triangle.
    //
    void CelDecoder::decodeFrameType2(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        decodeFrameType2or3(frame, pal, decodedFrame, true);
    }

    // DecodeFrameType3 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Dump one line of 32 pixels at the time.
    //       - The illustration below tells if a pixel is transparent or regular.
    //       - Only regular and zero (transparent) pixels are explicitly stored in
    //         the frame content. All other pixels of the illustration are
    //         implicitly transparent.
    //
    // Below is an illustration of the 32x32 image, where a space represents an
    // implicit transparent pixel, a '0' represents an explicit transparent pixel
    // and an 'x' represents an explicit regular pixel.
    //
    //
    //    +--------------------------------+
    //    |                                |
    //    |xx00                            |
    //    |xxxx                            |
    //    |xxxxxx00                        |
    //    |xxxxxxxx                        |
    //    |xxxxxxxxxx00                    |
    //    |xxxxxxxxxxxx                    |
    //    |xxxxxxxxxxxxxx00                |
    //    |xxxxxxxxxxxxxxxx                |
    //    |xxxxxxxxxxxxxxxxxx00            |
    //    |xxxxxxxxxxxxxxxxxxxx            |
    //    |xxxxxxxxxxxxxxxxxxxxxx00        |
    //    |xxxxxxxxxxxxxxxxxxxxxxxx        |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxx00    |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxx    |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx00|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx00|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxx    |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxx00    |
    //    |xxxxxxxxxxxxxxxxxxxxxxxx        |
    //    |xxxxxxxxxxxxxxxxxxxxxx00        |
    //    |xxxxxxxxxxxxxxxxxxxx            |
    //    |xxxxxxxxxxxxxxxxxx00            |
    //    |xxxxxxxxxxxxxxxx                |
    //    |xxxxxxxxxxxxxx00                |
    //    |xxxxxxxxxxxx                    |
    //    |xxxxxxxxxx00                    |
    //    |xxxxxxxx                        |
    //    |xxxxxx00                        |
    //    |xxxx                            |
    //    |xx00                            |
    //    +--------------------------------+
    //
    // Type3 corresponds to a 32x32 images of a right facing triangle.
    void CelDecoder::decodeFrameType3(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        decodeFrameType2or3(frame, pal, decodedFrame, false);
    }

    // DecodeFrameType4 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Dump one line of 32 pixels at the time.
    //       - The illustration below tells if a pixel is transparent or regular.
    //       - Only regular and zero (transparent) pixels are explicitly stored in
    //         the frame content. All other pixels of the illustration are
    //         implicitly transparent.
    //
    // Below is an illustration of the 32x32 image, where a space represents an
    // implicit transparent pixel, a '0' represents an explicit transparent pixel
    // and an 'x' represents an explicit regular pixel.
    //
    //
    //    +--------------------------------+
    //    |                            00xx|
    //    |                            xxxx|
    //    |                        00xxxxxx|
    //    |                        xxxxxxxx|
    //    |                    00xxxxxxxxxx|
    //    |                    xxxxxxxxxxxx|
    //    |                00xxxxxxxxxxxxxx|
    //    |                xxxxxxxxxxxxxxxx|
    //    |            00xxxxxxxxxxxxxxxxxx|
    //    |            xxxxxxxxxxxxxxxxxxxx|
    //    |        00xxxxxxxxxxxxxxxxxxxxxx|
    //    |        xxxxxxxxxxxxxxxxxxxxxxxx|
    //    |    00xxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |    xxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |00xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    +--------------------------------+
    //
    // Type4 corresponds to a 32x32 images of a left facing trapezoid.
    void CelDecoder::decodeFrameType4(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        decodeFrameType4or5(frame, pal, decodedFrame, true);
    }

    // DecodeFrameType5 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Dump one line of 32 pixels at the time.
    //       - The illustration below tells if a pixel is transparent or regular.
    //       - Only regular and zero (transparent) pixels are explicitly stored in
    //         the frame content. All other pixels of the illustration are
    //         implicitly transparent.
    //
    // Below is an illustration of the 32x32 image, where a space represents an
    // implicit transparent pixel, a '0' represents an explicit transparent pixel
    // and an 'x' represents an explicit regular pixel.
    //
    //
    //    +--------------------------------+
    //    |xx00                            |
    //    |xxxx                            |
    //    |xxxxxx00                        |
    //    |xxxxxxxx                        |
    //    |xxxxxxxxxx00                    |
    //    |xxxxxxxxxxxx                    |
    //    |xxxxxxxxxxxxxx00                |
    //    |xxxxxxxxxxxxxxxx                |
    //    |xxxxxxxxxxxxxxxxxx00            |
    //    |xxxxxxxxxxxxxxxxxxxx            |
    //    |xxxxxxxxxxxxxxxxxxxxxx00        |
    //    |xxxxxxxxxxxxxxxxxxxxxxxx        |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxx00    |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxx    |
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx00|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
    //    +--------------------------------+
    //
    // Type5 corresponds to a 32x32 images of a right facing trapezoid.
    void CelDecoder::decodeFrameType5(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        decodeFrameType4or5(frame, pal, decodedFrame, false);
    }

    // DecodeFrameType6 returns an image after decoding the frame in the following
    // way:
    //
    //    1) Read one byte (chunkSize).
    //    2) If chunkSize is positive, set that many transparent pixels.
    //    3) If chunkSize is negative, invert it's sign.
    //       3a) If chunkSize is below or equal to 65, read that many bytes.
    //          - Each byte read this way corresponds to a color index of the
    //            palette.
    //          - Set one regular pixel per byte, using the color index to locate
    //            the color in the palette.
    //       3b) If chunkSize is above 65, subtract 65 from it and read one byte.
    //          - The byte read this way corresponds to a color index of the
    //            palette.
    //          - Set chunkSize regular pixels, using the color index to locate the
    //            color in the palette.
    //    4) goto 1 until EOF is reached.
    //
    // Type6 is the only type for CL2 images.
    void CelDecoder::decodeFrameType6(FrameBytesRef frame,
                                      const Pal& pal,
                                      ColoursRef decodedFrame)
    {
        int32_t len = frame.size();
        for(int32_t pos = 0 ; pos < len ;)
        {
            int chunkSize = int(int8_t(frame[pos]));
            pos++;
            if (chunkSize >= 0) {
                // Transparent pixels.
                for (int i = 0; i < chunkSize; i++) {
                    Colour color(0, 0, 0, false);
                    decodedFrame.push_back(color);
                }
            } else {
                chunkSize = -chunkSize;
                if(chunkSize <= 65) {
                    // Regular pixels.
                    for (int i = 0; i < chunkSize; i++) {
                        Colour color = pal[frame[pos]];
                        decodedFrame.push_back(color);
                        pos++;
                    }
                } else {
                    chunkSize -= 65;
                    // Run-length encoded pixels.
                    Colour c = pal[frame[pos]];
                    for (int i = 0; i < chunkSize; i++) {
                        decodedFrame.push_back(c);
                    }
                    pos++;
                }

            }
        }
    }

    void CelDecoder::decodeFrameType2or3(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame, bool frameType2)
    {
        // Select line decoding function

        auto decodeLineTransparency = &CelDecoder::decodeLineTransparencyRight;

        if(frameType2) {
            decodeLineTransparency = &CelDecoder::decodeLineTransparencyLeft;
        }

        // Decode

        const uint8_t *framePtr = &frame[0];

        for(int row = 0; row <= 32 ; row++)
        {
            if(row == 15)
                row++;

            if(frameType2) {
                if( (row < 16 && row % 2 == 0) || (row >= 16 && row % 2 != 0) )  {
                    framePtr += 2;
                }
            } else {
                if( (row < 16 && row % 2 != 0) || (row >= 16 && row % 2 == 0) )  {
                    framePtr += 2;
                }
            }

            int regularCount = 0;
            if(row < 16)
            {
                regularCount = 2 + (row * 2);
            }
            else
            {
                regularCount = 32 - ((row - 16) * 2);
            }

            (this->*decodeLineTransparency)(&framePtr, pal, decodedFrame, regularCount);
        }
    }

    void CelDecoder::decodeFrameType4or5(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame, bool frameType4)
    {
        // Select line decoding function

        auto decodeLineTransparency = &CelDecoder::decodeLineTransparencyRight;

        if(frameType4) {
            decodeLineTransparency = &CelDecoder::decodeLineTransparencyLeft;
        }

        // Decode

        const uint8_t *framePtr = &frame[0];

        for(int row = 0; row < 15 ; row++)
        {
            if(frameType4) {
                if(row % 2 == 0) {
                    framePtr += 2;
                }
            } else {
                if(row % 2 != 0) {
                    framePtr += 2;
                }
            }

            int regularCount = 2 + (row * 2);

            (this->*decodeLineTransparency)(&framePtr, pal, decodedFrame, regularCount);
        }

        for(unsigned int framePos = 256 ; framePos < frame.size() ; framePos++)
        {
            Colour color = pal[frame[framePos]];
            decodedFrame.push_back(color);
        }
    }

    void CelDecoder::decodeLineTransparencyLeft(const uint8_t** framePtr,
                                                const Pal& pal,
                                                ColoursRef decodedFrame,
                                                int regularCount)
    {
        int transparentCount = 32 - regularCount;

        // Implicit transparent pixels.
        for (int i = 0; i < transparentCount; i++) {
            decodedFrame.push_back(Colour(0,0,0,false));
        }

        // Explicit regular pixels.
        for (int i = 0; i < regularCount; i++) {
            Colour color = pal[(*framePtr)[0]];
            decodedFrame.push_back(color);
            (*framePtr)++;
        }
    }

    void CelDecoder::decodeLineTransparencyRight(const uint8_t** framePtr,
                                                const Pal& pal,
                                                ColoursRef decodedFrame,
                                                int regularCount)
    {
        int transparentCount = 32 - regularCount;

        // Explicit regular pixels.
        for (int i = 0; i < regularCount; i++) {
            Colour color = pal[(*framePtr)[0]];
            decodedFrame.push_back(color);
            (*framePtr)++;
        }

        // Transparent pixels.
        for (int i = 0 ; i < transparentCount; i++) {
            decodedFrame.push_back(Colour(0,0,0,false));
        }
    }


    void CelDecoder::setObjcursCelDimensions(int frameNumber)
    {
        mFrameWidth = 56;
        mFrameHeight = 84;

        // Width
        if(frameNumber == 0) {
            mFrameWidth = 33;
        }
        else if(frameNumber > 0 && frameNumber <10) {
            mFrameWidth = 32;
        }
        else if(frameNumber == 10) {
            mFrameWidth = 23;
        }
        else if(frameNumber > 10 && frameNumber < 86) {
            mFrameWidth = 28;
        }
        else if(frameNumber >= 86 && frameNumber < 111){
            mFrameWidth = 56;
        }

        // Height
        if(frameNumber == 0) {
            mFrameHeight = 29;
        }
        else if(frameNumber > 0 && frameNumber <10) {
            mFrameHeight = 32;
        }
        else if(frameNumber == 10) {
            mFrameHeight = 35;
        }
        else if(frameNumber >= 11 && frameNumber < 61) {
            mFrameHeight = 28;
        }
        else if(frameNumber >= 61 && frameNumber < 67) {
            mFrameHeight = 56;
        }
        else if(frameNumber >= 67 && frameNumber < 86) {
            mFrameHeight = 84;
        }
        else if(frameNumber >= 86 && frameNumber < 111){
            mFrameHeight = 56;
        }
    }

    void CelDecoder::setCharbutCelDimensions(int frameNumber)
    {
        mFrameWidth = 41;

        if(frameNumber == 0) {
            mFrameWidth = 95;
        }
    }
}
