#pragma once
#include "celframe.h"
#include "pal.h"
#include <functional>
#include <map>
#include <settings/settings.h>
#include <stdint.h>
#include <vector>

namespace Cel
{
    class Pal;
    class XYIterator;

    class CelDecoder
    {
    public:
        explicit CelDecoder(std::string celPath);
        std::vector<Image> decode();
        int32_t numFrames() const;
        int32_t animationLength() const;

        static void loadConfigFiles();

    private:
        typedef std::vector<uint8_t> FrameBytes;
        typedef const std::vector<uint8_t>& FrameBytesRef;
        typedef std::vector<Colour>& ColoursRef;
        typedef std::vector<Colour>::iterator ColoursRefIterator;
        typedef std::function<void(FrameBytesRef, const Pal&, CelFrame&)> FrameDecoder;

        void readConfiguration();
        void readCelName();
        void readPalette();
        void getFrames();
        void decodeFrame(int32_t index, FrameBytesRef frame, CelFrame& celFrame);
        void setObjcursCelDimensions(int frame);
        void setCharbutCelDimensions(int frame);

    private:
        static FrameDecoder getFrameDecoder(const std::string& celName, FrameBytesRef frame, int frameNumber);
        static bool isType0(const std::string& celName, int frameNumber);
        static bool isType2or4(FrameBytesRef frame);
        static bool isType3or5(FrameBytesRef frame);
        static void decodeFrameType0(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType1(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType2(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType3(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType4(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType5(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType6(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame);
        static void decodeFrameType2or3(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame, bool frameType2);
        static void decodeFrameType4or5(FrameBytesRef frame, const Pal& pal, CelFrame& decodedFrame, bool frameType4);
        static void decodeLineTransparencyLeft(const uint8_t*& framePtr, const Pal& pal, CelFrame& decodedFrame, XYIterator& it, int32_t regularCount);
        static void decodeLineTransparencyRight(const uint8_t*& framePtr, const Pal& pal, CelFrame& decodedFrame, XYIterator& it, int32_t regularCount);

    private:
        std::vector<FrameBytes> mFrames;
        std::string mCelPath;
        std::string mCelName;
        Pal mPal;
        bool mIsObjcursCel = false;
        bool mIsCharbutCel = false;
        int mImageCount = 0;
        int mFrameWidth = 0;
        int mFrameHeight = 0;
        int mHeaderSize = 0;
        int32_t mAnimationLength = 0;

        static std::unique_ptr<Settings::Settings> mSettingsCel;
        static std::unique_ptr<Settings::Settings> mSettingsCl2;
    };
}
