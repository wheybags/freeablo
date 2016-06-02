#ifndef CEL_DECODING_H
#define CEL_DECODING_H

#include <map>
#include <vector>
#include <functional>
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
        size_t numFrames() const;
        size_t animationLength() const;
    private:

        typedef std::vector<uint8_t> FrameBytes;
        typedef FrameBytes& FrameBytesRef;
        typedef std::function<void(CelDecoder&, const FrameBytesRef, const Pal&, std::vector<Colour>&)> FrameDecoder;

        void readConfiguration();
        void readCelName();
        void readPalette();

        void getFrames();
        void decodeFrames();
        FrameDecoder getFrameDecoder(const std::string& celName, FrameBytesRef frame, int frameNumber);
        bool isType0(const std::string& celName, int frameNumber);
        bool isType2or4(FrameBytesRef frame);
        bool isType3or5(FrameBytesRef frame);

        void decodeFrameType0(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType1(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType2(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType3(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType4(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType5(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType6(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame);
        void decodeFrameType2or3(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame, bool frameType2);
        void decodeFrameType4or5(const FrameBytesRef frame, const Pal& pal, std::vector<Colour>& decodedFrame, bool frameType4);

        void decodeLineTransparencyLeft(const uint8_t* framePtr,
                                        const Pal& pal,
                                        std::vector<Colour>& decodedFrame,
                                        int,
                                        int);
        void decodeLineTransparencyRight(const uint8_t* framePtr,
                                        const Pal& pal,
                                        std::vector<Colour>& decodedFrame,
                                        int,
                                        int);
        void setObjcursCelDimensions(int frame);
        void setCharbutCelDimensions(int frame);

        std::vector<FrameBytes> mFrames;
        std::map<size_t, CelFrame> mCache;
        std::string mCelPath;
        std::string mCelName;
        Pal mPal;
        bool mIsCl2;
        bool mIsObjcursCel;
        bool mIsCharbutCel;
        int mImageCount;
        int mFrameWidth;
        int mFrameHeight;
        int mHeaderSize;
        size_t mAnimationLength;
        static Settings::Settings mSettingsCel;
        static Settings::Settings mSettingsCl2;
    };
}

#endif
