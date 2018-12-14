#include "video.h"

#include <iostream>

namespace Video
{
    void init() { std::cout << "No video backend supplied, video disabled" << std::endl; }

    void quit() {}

    void update() {}

    void playVideo(const std::string& path) { (void)path; }

    bool waitForVideoComplete(int ms)
    {
        (void)ms;
        return true;
    }

    void stopVideo() {}
}
