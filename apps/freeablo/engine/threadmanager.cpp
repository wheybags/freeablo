#include "threadmanager.h"

#include <chrono>
#include <iostream>

#include <input/inputmanager.h>

#include "../farender/renderer.h"

namespace Engine
{
    ThreadManager* ThreadManager::mThreadManager = NULL;
    ThreadManager* ThreadManager::get()
    {
        return mThreadManager;
    }

    ThreadManager::ThreadManager()
        :mRenderState(NULL),
        mAudioManager(50, 100)
    {
        mThreadManager = this;
    }

    void ThreadManager::run()
    {
        const int MAXIMUM_DURATION_IN_MS = 1000;
        Input::InputManager* inputManager = Input::InputManager::get();
        FARender::Renderer* renderer = FARender::Renderer::get();

        Message message;

        auto last = std::chrono::system_clock::now();
        size_t numFrames = 0;

        while(true)
        {
            mSpritesToPreload.clear();

            while(mQueue.pop(message))
                handleMessage(message);

            inputManager->poll();

            if(!renderer->renderFrame(mRenderState, mSpritesToPreload))
                break;

            auto now = std::chrono::system_clock::now();
            numFrames++;

            size_t duration = static_cast<size_t> (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() - last.time_since_epoch()).count());

            if(duration >= MAXIMUM_DURATION_IN_MS)
            {
                std::cout << "FPS: " << ((float)numFrames) / (((float)duration)/MAXIMUM_DURATION_IN_MS) << std::endl;
                numFrames = 0;
                last = now;
            }
        }

        renderer->cleanup();
    }


    void ThreadManager::playMusic(const std::string& path)
    {
        Message message;
        message.type = ThreadState::PLAY_MUSIC;
        message.data.musicPath = new std::string(path);

        mQueue.push(message);
    }

    void ThreadManager::playSound(const std::string& path)
    {
        if (path == "")
        {
            std::cerr << "Attempt to play invalid sound!" << std::endl;
            return;
        }

        Message message;
        message.type = ThreadState::PLAY_SOUND;
        message.data.soundPath = new std::string(path);

        mQueue.push(message);
    }

    void ThreadManager::stopSound()
    {
        Message message;
        message.type = ThreadState::STOP_SOUND;
        mQueue.push(message);
    }

    void ThreadManager::sendRenderState(FARender::RenderState* state)
    {
        Message message;
        message.type = ThreadState::RENDER_STATE;
        message.data.renderState = state;

        mQueue.push(message);
    }

    void ThreadManager::sendSpritesForPreload(std::vector<uint32_t> sprites)
    {
        Message message;
        message.type = ThreadState::PRELOAD_SPRITES;
        message.data.preloadSpriteIds = new std::vector<uint32_t>(sprites);

        mQueue.push(message);
    }


    void ThreadManager::handleMessage(const Message& message)
    {
        switch(message.type)
        {
            case ThreadState::PLAY_MUSIC:
            {
                mAudioManager.playMusic(*message.data.musicPath);
                delete message.data.musicPath;
                break;
            }

            case ThreadState::PLAY_SOUND:
            {
                mAudioManager.playSound(*message.data.soundPath);
                delete message.data.soundPath;
                break;
            }

            case ThreadState::STOP_SOUND:
            {
                mAudioManager.stopSound();
                break;
            }

            case ThreadState::RENDER_STATE:
            {
                if (mRenderState && mRenderState != message.data.renderState)
                    mRenderState->ready = true;

                mRenderState = message.data.renderState;
                break;
            }
            case ThreadState::PRELOAD_SPRITES:
            {
                mSpritesToPreload.insert(mSpritesToPreload.end(), message.data.preloadSpriteIds->begin(), message.data.preloadSpriteIds->end());
                delete message.data.preloadSpriteIds;
                break;
            }
        }
    }
}
