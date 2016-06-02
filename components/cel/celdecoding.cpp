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
        : mCelPath(celPath)
    {
        readCelName();
        readConfiguration();
        readPalette();
    }

    CelFrame& CelDecoder::operator [](size_t index) {
        return mCache[index];
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
        std::string celName = mCelName;
        std::string extension = "cel";

        if(Misc::StringUtils::endsWith(mCelPath,"cl2")) {
            settings = &mSettingsCl2;
            extension = "cl2";
            mIsCl2 = true;
        }

        // If more than one image in cel
        // read configuration from first subcel
        // (temporary solution)

        mImageCount = settings->get<int>(mCelName, "image_count");
        if(mImageCount > 0) {
            size_t pos = celName.find_last_of(extension) - 3;
            celName = celName.substr(0, pos) + "0." + extension;
            //Misc::StringUtils::replace(celName, "." + extension, "0." + extension);
        }

        mFrameWidth = settings->get<int>(celName, "width");
        mFrameHeight= settings->get<int>(celName, "height");
        mHeaderSize = settings->get<int>(celName, "header_size", 0);
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
        getFrames();
        decodeFrames();
    }

    void CelDecoder::getFrames()
    {
        if(Misc::StringUtils::endsWith(mCelName,"cl2"))
            return;

        // Open CEL file.

        FAIO::FAFile* file = FAIO::FAfopen(mCelPath);
        if(file == NULL) {
            return;
        }

        // Read first word.
        uint32_t frameCount = 0;
        uint32_t firstWord = 0;
        uint32_t offset = 0;
        uint32_t repeat = 1;
        FAIO::FAfread(&firstWord, 4, 1, file);

        // If firstWord == 32 then it is archive
        // that contains 8 cels and offset size is 32
        if(firstWord == 32) {
            repeat = 8;
            offset = 32;
        }

        // Offset file
        FAIO::FAfseek(file, offset, SEEK_SET);

        for(int r = 0; r < repeat ; r++)
        {
            // Read frame count
            FAIO::FAfread(&frameCount, 4, 1, file);

            // Read frame offsets.
            std::vector<uint32_t> frameOffsets(frameCount+1);
            for(int i = 0 ; i < frameCount + 1; i++)
            {
                FAIO::FAfread(&frameOffsets[i], 4, 1, file);
            }

            // Read frame contents
            for(int i = 0 ; i < frameCount ; i++)
            {
                int64_t frameStart = int64_t(frameOffsets[i]);
                int64_t frameEnd = int64_t(frameOffsets[i+1]);
                int64_t frameSize = frameEnd - frameStart;

                if(frameSize < 0) {
                    return;
                }

                mFrames.push_back(std::vector<uint8_t>(frameSize));
                uint32_t idx = mFrames.size() - 1;
                FAIO::FAfread(&mFrames[idx][0], 1, frameSize, file);
            }
        }

        FAIO::FAfclose(file);
    }

    void CelDecoder::decodeFrames()
    {
        int frameNumber = 0;
        for(FrameBytesRef frame : mFrames) {
            auto decoder = getFrameDecoder(mCelName, frame, frameNumber);

            CelFrame celFrame;
            celFrame.mWidth = mFrameWidth;
            celFrame.mHeight = mFrameHeight;

            decoder(*this, frame, mPal, celFrame.mRawImage);

            mCache[frameNumber] = celFrame;

            frameNumber++;
        }
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

    void CelDecoder::decodeFrameType3(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {
        decodeFrameType2or3(frame, pal, decodedFrame, false);
    }


    void CelDecoder::decodeFrameType4(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {
        decodeFrameType4or5(frame, pal, decodedFrame, true);
    }


    void CelDecoder::decodeFrameType5(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {
        decodeFrameType4or5(frame, pal, decodedFrame, false);
    }

    void CelDecoder::decodeFrameType6(const FrameBytesRef frame,
                                      const Pal& pal,
                                      std::vector<Colour>& decodedFrame)
    {}

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
            decodedFrame.push_back(Colour(0,0,0,false));
        }
        // Explicit transparent pixels (zeroes).
        for (int i = 0; i < zeroCount; i++) {
            decodedFrame.push_back(Colour(0,0,0,false));
        }
        // Explicit regular pixels.
        for (int i = zeroCount; i < decodeCount; i++) {
            Colour color = pal[framePtr[i]];
            decodedFrame.push_back(color);
        }
    }

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
            decodedFrame.push_back(Colour(0,0,0,false));
        }

        // Implicit transparent pixels.
        for (int i = decodeCount; i < 32; i++) {
            decodedFrame.push_back(Colour(0,0,0,false));
        }
    }




    int32_t normalWidth(const std::vector<uint8_t>& frame, size_t frameNum, bool fromHeader, uint16_t offset)
    {
        // If we have a header, we know that offset points to the end of the 32nd line.
        // So, when we reach that point, we will have produced 32 lines of pixels, so we 
        // can divide the number of pixels we have passed at this point by 32, to get the 
        // width.
        if(fromHeader)
        {
            // Workaround for objcurs.cel, the only cel file containing frames with a header whose offset is zero
            if(offset == 0)
            {
                if(frameNum == 0)
                    return 33;
                else if(frameNum > 0 && frameNum <10)
                    return 32;
                else if(frameNum == 10)
                    return 23;
                else if(frameNum > 10 && frameNum < 86)
                    return 28;
            }

            int32_t widthHeader = 0; 
            
            for(size_t i = 10; i < frame.size(); i++){
                
                if(i == offset && fromHeader)
                {
                    widthHeader = widthHeader/32;
                    break;
                }
                // Regular command
                if(frame[i] <= 127){
                    widthHeader += frame[i];
                    i += frame[i];
                }

                // Transparency command
                else if(128 <= frame[i]){
                    widthHeader += 256 - frame[i];
                }
            }

            return widthHeader;
        }
        
        // If we do not have a header we probably (definitely?) don't have any transparency.
        // The maximum stretch of opaque pixels following a command byte is 127.
        // Since commands can't wrap over lines (it seems), if the width is shorter than 127,
        // the first (command) byte will indicate an entire line, so it's value is the width.
        // If the width is larger than 127, it will be some sequence of 127 byte long stretches,
        // followed by some other value to bring it to the end of a line (presuming the width is
        // not divisible by 127).
        // So, for all image except those whose width is divisible by 127, we can determine width
        // by looping through control bits, adding 127 each time, until we find some value which
        // is not 127, then add that to the 127-total and that is our width.
        //
        // The above is the basic idea, but there is also a bunch of crap added in to maybe deal
        // with frames that don't quite fit the criteria.
        else
        {
            int32_t widthRegular = 0;
            bool hasTrans = false;

            uint8_t lastVal = 0;
            uint8_t lastTransVal = 0;
            
            for(size_t i = 0; i < frame.size(); i++){
                uint8_t val = frame[i];


                // Regular command
                if(val <= 127)
                {
                    widthRegular += val;
                    i += val;
                    
                    // Workaround for frames that start with a few px, then trans for the rest of the line
                    if(i+1 >= frame.size() || 128 <= frame[i+1])
                        hasTrans = true;
                }

                else if(128 <= val)
                {
                    
                    // Workaround for frames that start trans, then a few px of colour at the end
                    if(val == lastTransVal && lastVal <= 127 && lastVal == frame[i+1])
                        break;

                    widthRegular += 256 - val;
                    
                    // Workaround - presumes all headerless frames first lines start transparent, then go colour,
                    // then go transparent again, at which point they hit the end of the line, or if the first two
                    // commands are both transparency commands, that the image starts with a fully transparent line
                    if((hasTrans || 128 <= frame[i+1]) && val != 128)
                        break;

                    hasTrans = true;

                    lastTransVal = val;
                }

                if(val != 127 && !hasTrans)
                    break;

                lastVal = val;
            }

            return widthRegular;
        }
    }


    int32_t normalDecode(const std::vector<uint8_t>& frame, size_t frameNum, const Pal& pal, std::vector<Colour>& rawImage, bool tileCel)
    {
        #ifdef CEL_DEBUG
            std::cout << "NORMAL DECODE" << std::endl;
        #endif 
        
        size_t i = 0;

        uint16_t offset = 0;
        bool fromHeader = false;
        
        // The frame has a header which we can use to determine width
        if(!tileCel && frame[0] == 10)
        {
            fromHeader = true;
            offset = (uint16_t) (frame[3] << 8 | frame[2]);
            i = 10; // Skip the header
        }
     
        for(; i < frame.size(); i++)
        {
            // Regular command
            if(frame[i] <= 127)
            {
                #ifdef CEL_DEBUG
                    std::cout << i << " regular: " << (int)frame[i] << std::endl;
                #endif
                
                size_t j;
                // Just push the number of pixels specified by the command
                for(j = 1; j < frame[i]+1 && i+j < frame.size(); j++)
                {
                    int index = i+j;
                    uint8_t f = frame[index];
                    
                    if(index > frame.size()-1)
                        std::cout << "invalid read from f " << index << " " << frame.size() << std::endl;

                    Colour col = pal[f];

                    rawImage.push_back(col);
                }
                
                i+= frame[i];
            }


            // Transparency command
            else if(128 <= frame[i])
            {
                #ifdef CEL_DEBUG
                    std::cout << i << " transparency: " << (int)frame[i] << " " << (256 - frame[i]) << std::endl;
                #endif
                
                // Push (256 - command value) transparent pixels
                for(size_t j = 0; j < 256-frame[i]; j++)
                    rawImage.push_back(Colour(255, 0, 255, false));
            }
        }

        return normalWidth(frame, frameNum, fromHeader, offset);
    }
}
