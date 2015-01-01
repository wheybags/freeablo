#include "threadmanager.h"

#include <audio/audio.h>
#include <input/inputmanager.h>

#include "../farender/renderer.h"

namespace Engine
{
    ThreadManager* ThreadManager::mThreadManager = NULL;
    ThreadManager* ThreadManager::get()
    {
        return mThreadManager;
    }

    struct LoadGuiTextureArgs ///< args struct for thread communication
    {
        Rocket::Core::TextureHandle* texture_handle;
        Rocket::Core::Vector2i* texture_dimensions;
        const Rocket::Core::String* source;
    };
    struct GenerateGuiTextureArgs ///< args struct for thread communication
    {
        Rocket::Core::TextureHandle* texture_handle;
        const Rocket::Core::byte* source;
        const Rocket::Core::Vector2i* source_dimensions;
    };

    ThreadManager::ThreadManager():
        mThreadState(stopped)
    {
        mThreadManager = this;
    }

    ThreadManager::~ThreadManager()
    {
    }

    void ThreadManager::run()
    {
        while(!Input::InputManager::get()) {}
        Input::InputManager* inputManager = Input::InputManager::get();

        FARender::Renderer* renderer = FARender::Renderer::get();

        Audio::Music* music = NULL;

        while(true)
        {
            switch(mThreadState)
            {
                case musicPlay:
                {
                    if(music != NULL)
                        Audio::freeMusic(music);

                    music = Audio::loadMusic(*((std::string*)mThreadCommunicationTmp));
                    Audio::playMusic(music);
                    mThreadState = running;
                    break;
                }

                case guiLoadTexture:
                {
                    LoadGuiTextureArgs* args = (LoadGuiTextureArgs*) mThreadCommunicationTmp;
                    mThreadCommunicationTmp = (void*) Render::guiLoadImage(*(args->texture_handle), *(args->texture_dimensions), *(args->source));
                    mThreadState = running;
                    break;
                }

                case guiGenerateTexture:
                {
                    GenerateGuiTextureArgs* args = (GenerateGuiTextureArgs*) mThreadCommunicationTmp;
                    mThreadCommunicationTmp = (void*) Render::guiGenerateTexture(*(args->texture_handle), args->source, *(args->source_dimensions));
                    mThreadState = running;
                    break;
                }

                case guiReleaseTexture:
                {
                    Render::guiReleaseTexture(*((Rocket::Core::TextureHandle*)mThreadCommunicationTmp));
                    mThreadState = running;
                    break;
                }

                case levelChange:
                {
                    Level::Level* level = (Level::Level*)mThreadCommunicationTmp;

                    if(level)
                    {
                        Render::RenderLevel* renderLevel = Render::setLevel(*level);
                        renderer->setLevel(renderLevel, level);
                    }

                    mThreadState = running;
                    break;
                }

                case running:
                case stopped:
                {
                    break;
                }
            }

            inputManager->poll();

            if(!renderer->renderFrame())
                break;
        }

        renderer->cleanup();
    }


    void ThreadManager::playMusic(const std::string& path)
    {
        mThreadCommunicationTmp = (void*)&path;
        mThreadState = musicPlay;
        while(mThreadState != running) {}
    }

    bool ThreadManager::loadGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
    {
        LoadGuiTextureArgs args;
        args.texture_handle = &texture_handle;
        args.texture_dimensions = &texture_dimensions;
        args.source = &source;

        mThreadCommunicationTmp = (void*) &args;
        mThreadState = guiLoadTexture;
        while(mThreadState != running) {}

        return (bool) mThreadCommunicationTmp;
    }

    bool ThreadManager::generateGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
    {
        GenerateGuiTextureArgs args;
        args.texture_handle = &texture_handle;
        args.source = source;
        args.source_dimensions = &source_dimensions;

        mThreadCommunicationTmp = (void*) &args;
        mThreadState = guiGenerateTexture;
        while(mThreadState != running) {}

        return (bool) mThreadCommunicationTmp;
    }

    void ThreadManager::releaseGuiTextureFunc(Rocket::Core::TextureHandle texture_handle)
    {
        mThreadCommunicationTmp = (void*) &texture_handle;

        mThreadState = guiReleaseTexture;
        while(mThreadState != running) {}
    }

    void ThreadManager::setLevel(const Level::Level* level)
    {
        mThreadCommunicationTmp = (void*)level;
        mThreadState = levelChange;
        while(mThreadState != running){}
    }
}
