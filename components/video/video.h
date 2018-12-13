#pragma once

#include <stdint.h>
#include <string>

namespace Video
{
    void init();
    void quit();
    // No event system, so update() must be called periodically.
    void update();

    void playVideo(const std::string& path);
    void stopVideo();

    bool waitForVideoComplete(int ms); // Thread safe.

    inline bool isVideoPlaying() { return !waitForVideoComplete(0); }
}
