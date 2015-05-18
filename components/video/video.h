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

namespace Video
{

class Video
{
public:

    static bool init();

    Video();
    ~Video();

    bool load(const std::string& filename);
    void start();
    void stop();
    size_t numFrames() const;
    bool isPlaying();
    Render::Sprite& currentFrame();

private:

    std::string mFilename;
    std::vector<Render::Sprite> mSprites;

    int mCurrentFrame;
    int mNumFrames;
    boost::timer::cpu_timer mTimer;
    boost::timer::cpu_times mLastTime;
};


}

#endif
