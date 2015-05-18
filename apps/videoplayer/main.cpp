#include <iostream>

#include <faio/faio.h>
#include <render/render.h>
#include <input/inputmanager.h>
#include <video/video.h>


using namespace std;

bool done = false;

int main(int, char** argv)
{
    FAIO::init();

    Render::RenderSettings settings;
    settings.windowWidth = 1280;
    settings.windowHeight = 960;
    Render::init(settings);

    Video::Video::init();

    vector<string> filenames;
    filenames.push_back("gendata/logo.smk");
    filenames.push_back("gendata/fbutch3.smk");
    filenames.push_back("gendata/loopdend.smk");
    filenames.push_back("gendata/fprst3.smk");
    filenames.push_back("gendata/doom.smk");
    filenames.push_back("gendata/diabvic3.smk");
    filenames.push_back("gendata/diabvic2.smk");
    filenames.push_back("gendata/diabvic1.smk");
    filenames.push_back("gendata/diabend.smk");
    filenames.push_back("gendata/diablo1.smk");

    int currentVideo = 0;
    int numVideos = filenames.size();

    while(!done)
    {
        Video::Video video;
        video.load(filenames[currentVideo]);
        video.start();

        while(video.isPlaying())
        {
            Render::clear();
            Render::drawAt(video.currentFrame(), 0, 160);
            Render::draw();
        }

        if(++currentVideo >= numVideos)
            currentVideo = 0;
    }

    FAIO::quit();
    return 0;
}
