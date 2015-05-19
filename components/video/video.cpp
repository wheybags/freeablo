#include "video.h"

#include <faio/faio.h>
#include <SDL.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
}

namespace Video
{

AVCodec * Video::sCodec;
AVFrame * Video::sFrame;
AVFrame * Video::sFrameRGB;
const int Video::BUFFER_SIZE = 8192;


bool Video::init()
{
    av_register_all();
    avcodec_register_all();

    Video::sCodec       = avcodec_find_decoder(AV_CODEC_ID_SMACKVIDEO);
    Video::sFrame       = av_frame_alloc();
    Video::sFrameRGB    = av_frame_alloc();

    return true;
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


static int readFunction(void* opaque, uint8_t* buf, int buf_size) {

    FAIO::FAFile& file = *reinterpret_cast<FAIO::FAFile*>(opaque);
    size_t bytes = FAIO::FAfread(reinterpret_cast<void*>(buf), buf_size, 1, &file);
    return bytes;
}

Video::Video(unsigned int width, unsigned int height)
    :
      mWidth(width),
      mHeight(height),
      mCurrentFrame(-1),
      mNumFrames(0)
{
}

Video::~Video()
{
    int size = mSprites.size();
    for(int i = 0 ; i < size; i++)
    {
        SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(mSprites[i]));
    }
}

bool Video::load(const std::string& filename)
{
    mFilename = filename;
    mNumFrames = 0;

    int                 videoStream = -1;
    unsigned char*      buffer;
    FAIO::FAFile *      videoFile;
    AVIOContext*        avioContext;
    AVCodecContext*     codecContextOrig;
    AVCodecContext*     codecContext;
    AVFormatContext*    formatContext;

    // Open video file

    videoFile = FAIO::FAfopen(filename);
    if(videoFile == NULL)
    {
        return false;
    }

    buffer              = reinterpret_cast<unsigned char*>(av_malloc(BUFFER_SIZE));
    avioContext         = avio_alloc_context(buffer, BUFFER_SIZE, 0, reinterpret_cast<void*>(static_cast<FAIO::FAFile*>(videoFile)), &readFunction, NULL, NULL);
    formatContext       = avformat_alloc_context();
    formatContext->pb   = avioContext;

    avformat_open_input(&formatContext, "", NULL, 0);
    avformat_find_stream_info(formatContext, NULL);
    //av_dump_format(formatContext, 0, "", 0);

    // Find the first video stream
    for(unsigned int i = 0; i < formatContext->nb_streams; i++)
    {
        if(formatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
            break;
        }
    }

    // Get codec context
    codecContextOrig    = formatContext->streams[videoStream]->codec;
    codecContext        = avcodec_alloc_context3(NULL);
    avcodec_copy_context(codecContext, codecContextOrig);

    if(mWidth == 0)
        mWidth = codecContext->width;
    if(mHeight == 0)
        mHeight = codecContext->height;

    // Open codec
    if(avcodec_open2(codecContext, sCodec, 0) < 0)
    {
        av_free(avioContext);
        av_free(buffer);
        return false;
    }

    SwsContext *swsContext= NULL;
    swsContext = sws_getContext(codecContext->width,
        codecContext->height,
        codecContext->pix_fmt,
        mWidth,
        mHeight,
        PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );

    AVPacket    packet;
    int         frameFinished;
    int         numBytes;
    uint8_t *   bufferRGB;

    numBytes        = avpicture_get_size(PIX_FMT_RGB32, mWidth, mHeight);
    bufferRGB       = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    avpicture_fill((AVPicture *)sFrameRGB, bufferRGB, PIX_FMT_RGB32, mWidth, mHeight);

    while(av_read_frame(formatContext, &packet) >= 0)
    {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream)
        {
            // Decode video frame
            avcodec_decode_video2(codecContext,sFrame,&frameFinished,&packet);

            // Did we get a video frame?
            if(frameFinished)
            {
                sws_scale(swsContext ,sFrame->data,sFrame->linesize,0, codecContext->height, sFrameRGB->data, sFrameRGB->linesize);
                mSprites.push_back(Render::loadVideoFrame(sFrameRGB->data, sFrameRGB->linesize, mWidth, mHeight));
                ++mNumFrames;

                //Save frame on disk example
                //SaveFrame(frameRGB, codecContext->width, codecContext->height, frameCounter);
             }
          }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    av_free(bufferRGB);

    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
    avcodec_close(codecContextOrig);
    avcodec_close(codecContext);
    av_free(avioContext);

    FAIO::FAfclose(videoFile);

    return true;
}

void Video::close()
{
    av_free(sFrame);
    av_free(sFrameRGB);
}

void Video::start()
{
    mTimer.start();
    mLastTime = mTimer.elapsed();
    mCurrentFrame = 0;
}

bool Video::isPlaying()
{
    if(mCurrentFrame < 0 || mCurrentFrame == mNumFrames)
        return false;

    return true;
}

size_t Video::numFrames() const
{
    return mNumFrames;
}

Render::Sprite& Video::currentFrame()
{
    // TO DO: read FPS from file
    static const boost::int_least64_t SECOND_IN_NANO = 1000000000;
    static const boost::int_least64_t FPS = 15;
    static const boost::int_least64_t delta = SECOND_IN_NANO / FPS;

    boost::timer::cpu_times time = mTimer.elapsed();
    boost::int_least64_t diff = time.wall - mLastTime.wall;
    if(diff >= delta)
    {
        mLastTime = mTimer.elapsed();
        if(++mCurrentFrame >= mNumFrames)
            mCurrentFrame = mNumFrames;
    }

    if(mCurrentFrame == mNumFrames)
        return mSprites[mNumFrames - 1];

    return mSprites[mCurrentFrame];
}



}
