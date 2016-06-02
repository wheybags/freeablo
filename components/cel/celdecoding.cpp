#include <iostream>
#include <set>
#include <functional>
#include <misc/stringops.h>
#include <faio/faio.h>
#include "celdecoding.h"

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

    CelFrame& CelDecoder::operator [](size_t index) {

        if(mCache.count(index)) {
            return mCache[index];
        }

        auto& frame = mFrames[index];
        CelFrame celFrame;
        decodeFrame(index, frame, celFrame);
        mCache[index] = celFrame;

        return mCache[index];
    }

    size_t CelDecoder::numFrames() const
    {
        return mFrames.size();
    }

    size_t CelDecoder::animationLength() const
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

        size_t pos = celNameWithoutExtension.find_last_of(extension) - 3;
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
        decodeFrames();
    }

    void CelDecoder::getFrames()
    {
        // Open CEL file.

        FAIO::FAFile* file = FAIO::FAfopen(mCelPath);
        if(file == NULL) {
            return;
        }

        // Read first word.
        uint32_t frameCount = 0;
        uint32_t firstWord = 0;
        uint32_t repeat = 1;
        FAIO::FAfread(&firstWord, 4, 1, file);

        std::vector<uint32_t> headerOffsets;

        // If firstWord == 32 then it is archive
        // that contains 8 cels and information about offsets in header.

        FAIO::FAfseek(file, 0, SEEK_SET);

        if(firstWord == 32) {
            repeat = 8;

            // Read header offsets
            for(uint32_t i = 0; i < repeat ; i++)
            {
                uint32_t offset = 0;
                FAIO::FAfread(&offset, 4, 1, file);
                headerOffsets.push_back(offset);
            }
        }

        for(uint32_t r = 0; r < repeat ; r++)
        {
            // Offset file
            if(!headerOffsets.empty()) {
                FAIO::FAfseek(file, headerOffsets[r], SEEK_SET);
            }

            // Read frame count
            FAIO::FAfread(&frameCount, 4, 1, file);

            // Read frame offsets.
            std::vector<uint32_t> frameOffsets(frameCount+1);
            for(uint32_t i = 0 ; i < frameCount + 1; i++)
            {
                FAIO::FAfread(&frameOffsets[i], 4, 1, file);
            }

            // Magic offset that fixes everything!
            if(!headerOffsets.empty()) {
                FAIO::FAfseek(file, headerOffsets[r] + frameOffsets[0], SEEK_SET);
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

                mFrames.push_back(std::vector<uint8_t>(frameSize));
                uint32_t idx = mFrames.size() - 1;
                FAIO::FAfseek(file, mHeaderSize, SEEK_CUR);
                FAIO::FAfread(&mFrames[idx][0], 1, frameSize, file);
            }

            mAnimationLength = frameCount;
        }

        FAIO::FAfclose(file);
    }

    void CelDecoder::decodeFrames()
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

    void CelDecoder::decodeFrame(size_t index, FrameBytesRef frame, CelFrame& celFrame) {
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
    void CelDecoder::decodeFrameType0(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {
        size_t len = frame.size();
        for(size_t pos = 0 ; pos < len ; pos++)
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
    void CelDecoder::decodeFrameType1(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {
        size_t len = frame.size();
        for(size_t pos = 0 ; pos < len ;)
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
    void CelDecoder::decodeFrameType2(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
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
    void CelDecoder::decodeFrameType3(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
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
    void CelDecoder::decodeFrameType4(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
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
    void CelDecoder::decodeFrameType5(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
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
    void CelDecoder::decodeFrameType6(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {
        size_t len = frame.size();
        for(size_t pos = 0 ; pos < len ;)
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

    void CelDecoder::decodeFrameType2or3(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame, bool frameType2)
    {
        // Select line decoding function

        auto decodeLineTransparency = &CelDecoder::decodeLineTransparencyRight;

        if(frameType2) {
            decodeLineTransparency = &CelDecoder::decodeLineTransparencyLeft;
        }

        // Decode

        static std::vector<int> decodeCounts =
            {0, 4, 4, 8, 8, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 28, 28, 24, 24, 20, 20, 16, 16, 12, 12, 8, 8, 4, 4};

        int lineNum = 0;
        const uint8_t *framePtr = &frame[0];
        for(int decodeCount : decodeCounts)
        {
            int zeroCount = 0;
            if(lineNum % 2 == 1) {
                zeroCount = 2;
            }

            int regularCount = decodeCount - zeroCount;
            (this->*decodeLineTransparency)(framePtr, pal, decodedFrame, regularCount, zeroCount);
            framePtr += decodeCount;
            lineNum++;
        }
    }

    void CelDecoder::decodeFrameType4or5(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame, bool frameType4)
    {
        // Select line decoding function

        auto decodeLineTransparency = &CelDecoder::decodeLineTransparencyRight;

        if(frameType4) {
            decodeLineTransparency = &CelDecoder::decodeLineTransparencyLeft;
        }

        // Decode

        static std::vector<int> decodeCounts =
            {4, 4, 8, 8, 12, 12, 16, 16, 20, 20, 24, 24, 28, 28, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
        int lineNum = 0;
        const uint8_t *framePtr = &frame[0];
        for(int decodeCount : decodeCounts)
        {
            static std::set<int> lineNumbers = { 0, 2, 4, 6, 8, 10, 12, 14 };
            int zeroCount = 0;

            if(lineNumbers.find(lineNum) != lineNumbers.end()) {
                zeroCount = 2;
            }

            int regularCount = decodeCount - zeroCount;
            (this->*decodeLineTransparency)(framePtr, pal, decodedFrame, regularCount, zeroCount);
            framePtr += decodeCount;
            lineNum++;
        }
    }

    // decodeLineTransparencyLeft decodes a line of the frame, where regularCount
    // represent the number of explicit regular pixels, zeroCount the number of
    // explicit transparent pixels and the rest of the line is implicitly
    // transparent. Each line is assumed to have a width of 32 pixels.
    //
    void CelDecoder::decodeLineTransparencyLeft(const uint8_t* framePtr,
                                                const Pal& pal,
                                                std::vector<Colour>& decodedFrame,
                                                int regularCount,
                                                int zeroCount)
    {
        // Total number of explicit pixels.
        int decodeCount = zeroCount + regularCount;

        // Implicit transparent pixels.
        for (int i = decodeCount; i < 32; i++) {
            decodedFrame.push_back(Colour(255,0,255,false));
        }
        // Explicit transparent pixels (zeroes).
        for (int i = 0; i < zeroCount; i++) {
            decodedFrame.push_back(Colour(0,255,0,false));
        }
        // Explicit regular pixels.
        for (int i = zeroCount; i < decodeCount; i++) {
            Colour color = pal[framePtr[i]];
            decodedFrame.push_back(color);
        }
    }

    // decodeLineTransparencyRight decodes a line of the frame, where regularCount
    // represent the number of explicit regular pixels, zeroCount the number of
    // explicit transparent pixels and the rest of the line is implicitly
    // transparent. Each line is assumed to have a width of 32 pixels.
    void CelDecoder::decodeLineTransparencyRight(const uint8_t* framePtr,
                                                const Pal& pal,
                                                std::vector<Colour>& decodedFrame,
                                                int regularCount,
                                                int zeroCount)
    {
        // Total number of explicit pixels.
        int decodeCount = zeroCount + regularCount;

        // Explicit regular pixels.
        for (int i = zeroCount; i < decodeCount; i++) {
            Colour color = pal[framePtr[i]];
            decodedFrame.push_back(color);
        }

        // Explicit transparent pixels (zeroes).
        for (int i = 0; i < zeroCount; i++) {
            decodedFrame.push_back(Colour(0,0,255,false));
        }

        // Implicit transparent pixels.
        for (int i = decodeCount ; i < 32; i++) {
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
