#ifndef CEL_DECODING_H
#define CEL_DECODING_H

#include <map>
#include <vector>
#include <stdint.h>
#include <settings/settings.h>
#include "celframe.h"
#include "pal.h"

namespace Cel
{
    int32_t normalDecode(const std::vector<uint8_t>& frame, size_t frameNum, const Pal& pal, std::vector<Colour>& raw_image, bool tileCel = false);

    class CelDecoder
    {
    public:
        CelDecoder(const std::string& celPath);
        void decode();
        CelFrame& operator[](size_t index);
    private:

        typedef std::vector<uint8_t> FrameBytes;
        typedef FrameBytes& FrameBytesRef;

        enum FrameDecoder
        {
            DECODER_TYPE_0,
            DECODER_TYPE_1,
            DECODER_TYPE_2,
            DECODER_TYPE_3,
            DECODER_TYPE_4,
            DECODER_TYPE_5,
            DECODER_TYPE_6
        };

        void readConfiguration();
        void readCelName();
        void readPalette();

        void getFrames();
        void decodeFrames();
        FrameDecoder getFrameDecoder(const std::string& celName, FrameBytesRef frame, int frameNumber);
        bool isType0(const std::string& celName, int frameNumber);
        bool isType2or4(FrameBytesRef frame);
        bool isType3or5(FrameBytesRef frame);

        void decodeFrameType0(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType1(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType2(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType3(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType4(const std::vector<uint8_t>& frame, const Pal& pal, std::vector<Colour>& decodedFrame);

        void decodeLineTransparencyLeft(const std::vector<uint8_t>& frame,
                                        const Pal& pal,
                                        std::vector<Colour>& decodedFrame,
                                        int,
                                        int);
        void decodeLineTransparencyRight(const std::vector<uint8_t>& frame,
                                        const Pal& pal,
                                        std::vector<Colour>& decodedFrame,
                                        int,
                                        int);


        std::vector<FrameBytes> mFrames;
        std::map<size_t, CelFrame> mCache;
        std::string mCelPath;
        std::string mCelName;
        Pal mPal;
        bool mIsCl2;
        int mImageCount;
        int mFrameWidth;
        int mFrameHeight;
        int mHeaderSize;
        static Settings::Settings mSettingsCel;
        static Settings::Settings mSettingsCl2;
    };
}

#endif
