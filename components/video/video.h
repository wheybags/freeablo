#ifndef VIDEO_H
#define VIDEO_H

#include <string>
#include <vector>
#include <render/render.h>
#include <boost/timer/timer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
}

namespace Video
{
class Video
{
public:

    static bool init();
    static void close();

    bool load(const std::string& filename, unsigned int width = 0, unsigned int height = 0);
    void start();
    void stop();
    bool isPlaying();

    Render::Sprite currentFrame();

    inline unsigned int width() const { return mWidth; }
    inline unsigned int height() const { return mHeight; }

private:

    struct ConsumerArguments
    {
        Render::Sprite* sprite;
        bool * isProducerFinished;
        bool * isConsumerFinished;
        bool * isForceStop;
    };

    void work();
    void loadFrameToBuffer();
    void consumerFunction(ConsumerArguments args);


    unsigned int        mWidth;
    unsigned int        mHeight;
    int                 mCurrentProducerIndex;
    bool                mFirstRun;
    bool                mForceStop;
    int                 mVideoStream;
    unsigned char*      mBuffer;
    uint8_t *           mBufferRGB;
    std::string         mFilename;

    FAIO::FAFile *      mVideoFile;
    AVIOContext*        mAvioContext;
    AVCodecContext*     mCodecContextOrig;
    AVCodecContext*     mCodecContext;
    AVFormatContext*    mFormatContext;
    AVPacket            mPacket;
    SwsContext *        mSwsContext;
    Render::Sprite      mCurrentSprite;

    bool mIsProducerFinished;
    bool mIsConsumerFinished;
    boost::shared_ptr<boost::thread> mProducer;
    boost::shared_ptr<boost::thread> mConsumer;

    static const int BUFFER_SIZE;
    static AVCodec * sCodec;
    static AVFrame * sFrame;
    static AVFrame * sFrameRGB;
};


}

#endif
