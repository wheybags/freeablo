#include "video.h"

#include <faio/faio.h>
#include <boost/asio/steady_timer.hpp>

namespace Video
{

#ifndef AV_CODEC_ID_SMACKVIDEO
    #define AV_CODEC_ID_SMACKVIDEO CODEC_ID_SMACKVIDEO
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
    #define av_frame_alloc  avcodec_alloc_frame
#endif

AVCodec * Video::sCodec;
AVFrame * Video::sFrame;
AVFrame * Video::sFrameRGB;
const int Video::BUFFER_SIZE = 8192;

boost::mutex gMutex1;
boost::mutex gMutex2;

boost::unique_lock<boost::mutex> gLock1(gMutex1);
boost::unique_lock<boost::mutex> gLock2(gMutex2);

boost::condition_variable gConditionVariable1;
boost::condition_variable gConditionVariable2;

boost::asio::io_service g_ioService;
boost::asio::steady_timer gTimer(g_ioService);


bool Video::init()
{
    av_register_all();
    avcodec_register_all();

    Video::sCodec       = avcodec_find_decoder(AV_CODEC_ID_SMACKVIDEO);
    Video::sFrame       = av_frame_alloc();
    Video::sFrameRGB    = av_frame_alloc();

    return true;
}

static int readFunction(void* opaque, uint8_t* buf, int buf_size) {

    FAIO::FAFile& file = *reinterpret_cast<FAIO::FAFile*>(opaque);
    size_t bytes = FAIO::FAfread(reinterpret_cast<void*>(buf), buf_size, 1, &file);
    return bytes;
}

void Video::consumerFunction(ConsumerArguments args)
{    
    gConditionVariable1.wait(gLock1);

    int currentConsumerIndex = 0;
    while(*(args.isProducerFinished) == 0)
    {
        if(*args.isForceStop)
        {
            break;
        }

        // TO DO: read FPS from file
        static const boost::int_least64_t SECOND_IN_NANO = 1000000000;
        static const boost::int_least64_t FPS = 15;
        static const boost::int_least64_t delta = SECOND_IN_NANO / FPS;
        static const boost::int_least64_t mili = delta / 1000 / 1000;

        gTimer.expires_from_now(boost::chrono::milliseconds(mili));
        gTimer.wait();

        *(args.sprite) = Render::getVideoBufferFrame(currentConsumerIndex);
        ++currentConsumerIndex;
        currentConsumerIndex =  currentConsumerIndex % Render::VIDEO_BUFFER_SIZE;

        gConditionVariable2.notify_all();
    }

    *(args.isConsumerFinished) = true;
}

bool Video::load(const std::string& filename, unsigned int width, unsigned int height)
{
    mFilename = filename;
    mWidth = width;
    mHeight = height;

    if(mWidth == 0)
        mWidth = mCodecContext->width;
    if(mHeight == 0)
        mHeight = mCodecContext->height;

    // Open video file

    mVideoFile = FAIO::FAfopen(mFilename);
    if(mVideoFile == NULL)
    {
        return false;
    }

    mBuffer              = reinterpret_cast<unsigned char*>(av_malloc(BUFFER_SIZE));
    mAvioContext         = avio_alloc_context(mBuffer, BUFFER_SIZE, 0, reinterpret_cast<void*>(static_cast<FAIO::FAFile*>(mVideoFile)), &readFunction, NULL, NULL);
    mFormatContext       = avformat_alloc_context();
    mFormatContext->pb   = mAvioContext;

    avformat_open_input(&mFormatContext, "", NULL, 0);
    avformat_find_stream_info(mFormatContext, NULL);

    // Find the first video stream
    mVideoStream = -1;
    for(unsigned int i = 0; i < mFormatContext->nb_streams; i++)
    {
        if(mFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            mVideoStream = i;
            break;
        }
    }

    // Get codec context
    mCodecContextOrig    = mFormatContext->streams[mVideoStream]->codec;
    mCodecContext        = avcodec_alloc_context3(NULL);
    avcodec_copy_context(mCodecContext, mCodecContextOrig);

    if(mWidth == 0)
        mWidth = mCodecContext->width;
    if(mHeight == 0)
        mHeight = mCodecContext->height;

    // Open codec
    if(avcodec_open2(mCodecContext, sCodec, 0) < 0)
    {
        av_free(mAvioContext);
        av_free(mBuffer);
        return false;
    }

    AVPixelFormat pixelFormat = PIX_FMT_RGB32;
    mSwsContext = sws_getContext(mCodecContext->width,
        mCodecContext->height,
        mCodecContext->pix_fmt,
        mWidth,
        mHeight,
        pixelFormat,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

    int numBytes     = avpicture_get_size(pixelFormat, mWidth, mHeight);
    mBufferRGB       = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    avpicture_fill((AVPicture *)sFrameRGB, mBufferRGB, pixelFormat, mWidth, mHeight);

    return true;
}

void Video::work()
{
    if(mFirstRun)
    {
        for(int i = 0 ; i < Render::VIDEO_BUFFER_SIZE - 1; i++)
            loadFrameToBuffer();

        mFirstRun = false;
        gConditionVariable1.notify_all();
    }
    else
    {
        gConditionVariable2.wait(gLock2);
        loadFrameToBuffer();
        gConditionVariable1.notify_all();
    }
}

void Video::loadFrameToBuffer()
{
    for(unsigned int i = 0 ; i < mFormatContext->nb_streams; i++)
    {
        if(mForceStop)
        {
            mIsProducerFinished = true;
            return;
        }

        if(av_read_frame(mFormatContext, &mPacket) < 0)
        {
            mIsProducerFinished = true;
            return;
        }

        int frameFinished = 0;

        // Is this a packet from the video stream?
        if(mPacket.stream_index==mVideoStream)
        {
            // Decode video frame
            avcodec_decode_video2(mCodecContext,sFrame,&frameFinished,&mPacket);

            // Did we get a video frame?
            if(frameFinished)
            {
                sws_scale(mSwsContext ,sFrame->data,sFrame->linesize,0, mCodecContext->height, sFrameRGB->data, sFrameRGB->linesize);
                Render::copyVideoFrameToBuffer(mCurrentProducerIndex,sFrameRGB->data, sFrameRGB->linesize, mWidth, mHeight);

                ++mCurrentProducerIndex;
                mCurrentProducerIndex =  mCurrentProducerIndex % Render::VIDEO_BUFFER_SIZE;
             }
          }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&mPacket);
    }
}

void Video::stop()
{
    mForceStop = true;
    mConsumer->join();
    mIsProducerFinished = true;

    av_free(mBufferRGB);

    avformat_close_input(&mFormatContext);
    avcodec_close(mCodecContextOrig);
    avcodec_close(mCodecContext);
    av_free(mAvioContext);

    FAIO::FAfclose(mVideoFile);
}

void Video::close()
{
    av_free(sFrame);
    av_free(sFrameRGB);
}

void Video::start()
{
    mForceStop              = false;
    mFirstRun               = true;
    mCurrentProducerIndex   = 0;
    mIsProducerFinished     = false;
    mIsConsumerFinished     = false;
    mCurrentSprite = Render::getVideoBufferFrame(0);

    ConsumerArguments args;
    args.sprite = &mCurrentSprite;
    args.isConsumerFinished = &mIsConsumerFinished;
    args.isProducerFinished = &mIsProducerFinished;
    args.isForceStop = &mForceStop;

    mConsumer = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Video::consumerFunction, *this, args)));
}

bool Video::isPlaying()
{
    if(mIsProducerFinished && mIsConsumerFinished)
        return false;

    return true;
}

Render::Sprite Video::currentFrame()
{
    work();
    return mCurrentSprite;
}

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);

    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

    // Close file
    fclose(pFile);
}

}
