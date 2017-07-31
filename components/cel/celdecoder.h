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
    class CelDecoder
    {
    public:
        CelDecoder(const std::string& celPath);
        void decode();
        CelFrame& operator[](int32_t index);
        int32_t numFrames() const;
        int32_t animationLength() const;
    private:

        typedef std::vector<uint8_t> FrameBytes;
        typedef const std::vector<uint8_t>& FrameBytesRef;
        typedef std::vector<Colour>& ColoursRef;
        typedef std::function<void(CelDecoder&, FrameBytesRef, const Pal&, ColoursRef)> FrameDecoder;

        void readConfiguration();
        void readCelName();
        void readPalette();

        void getFrames();
        void decodeFrame(int32_t index, FrameBytesRef frame, CelFrame& celFrame);
        FrameDecoder getFrameDecoder(const std::string& celName, FrameBytesRef frame, int frameNumber);
        bool isType0(const std::string& celName, int frameNumber);
        bool isType2or4(FrameBytesRef frame);
        bool isType3or5(FrameBytesRef frame);

        void decodeFrameType0(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType1(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType2(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType3(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType4(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType5(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType6(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame);
        void decodeFrameType2or3(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame, bool frameType2);
        void decodeFrameType4or5(FrameBytesRef frame, const Pal& pal, ColoursRef decodedFrame, bool frameType4);

        void decodeLineTransparencyLeft(const uint8_t** framePtr,
                                        const Pal& pal,
                                        ColoursRef decodedFrame,
                                        int);
        void decodeLineTransparencyRight(const uint8_t** framePtr,
                                        const Pal& pal,
                                        ColoursRef decodedFrame,
                                        int);
        void setObjcursCelDimensions(int frame);
        void setCharbutCelDimensions(int frame);

        std::vector<FrameBytes> mFrames;
        std::map<int32_t, CelFrame> mCache;
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
        int32_t mAnimationLength;
        static Settings::Settings mSettingsCel;
        static Settings::Settings mSettingsCl2;
    };
}

#endif
