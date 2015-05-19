#ifndef VIDEO_H
#define VIDEO_H

#include <string>
#include <vector>
#include <render/render.h>
#include <boost/timer/timer.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <boost/shared_ptr.hpp>


class AVFrame;
class AVFormatContext;
class AVCodec;

namespace Video
{
class Video
{
public:

    static bool init();
    static void close();

    Video(unsigned int width = 0, unsigned int height = 0);
    ~Video();

    bool load(const std::string& filename);
    void start();
    void stop();
    size_t numFrames() const;
    bool isPlaying();
    Render::Sprite& currentFrame();

    inline unsigned int width() const { return mWidth; }
    inline unsigned int height() const { return mHeight; }

private:

    static const int BUFFER_SIZE;
    static AVCodec * sCodec;
    static AVFrame * sFrame;
    static AVFrame * sFrameRGB;

    std::string mFilename;
    std::vector<Render::Sprite> mSprites;

    unsigned int mWidth;
    unsigned int mHeight;
    int mCurrentFrame;
    int mNumFrames;
    boost::timer::cpu_timer mTimer;
    boost::timer::cpu_times mLastTime;
};


}

#endif
