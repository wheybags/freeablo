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
            while(mQueue.pop(message))
                handleMessage(message);

            inputManager->poll();

            if(!renderer->renderFrame(mRenderState))
                break;
            
            auto now = std::chrono::system_clock::now();
            numFrames++;
            
            size_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() - last.time_since_epoch()).count();
            
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
        message.type = PLAY_MUSIC;
        message.data.musicPath = new std::string(path);

        mQueue.push(message);
    }

    void ThreadManager::playSound(const std::string& path)
    {
        Message message;
        message.type = PLAY_SOUND;
        message.data.soundPath = new std::string(path);

        mQueue.push(message);
    }

    void ThreadManager::stopSound()
    {
        Message message;
        message.type = STOP_SOUND;
        mQueue.push(message);
    }

    void ThreadManager::sendRenderState(FARender::RenderState* state)
    {
        Message message;
        message.type = RENDER_STATE;
        message.data.renderState = state;

        mQueue.push(message);
    }

    void ThreadManager::handleMessage(const Message& message)
    {
        switch(message.type)
        {
            case PLAY_MUSIC:
            {
                mAudioManager.playMusic(*message.data.musicPath);
                delete message.data.musicPath;
                break;
            }

            case PLAY_SOUND:
            {
                mAudioManager.playSound(*message.data.soundPath);
                delete message.data.soundPath;
                break;
            }

            case STOP_SOUND:
            {
                mAudioManager.stopSound();
                break;
            }

            case RENDER_STATE:
            {
				if (mRenderState && mRenderState != message.data.renderState)
                    mRenderState->ready = true;

                mRenderState = message.data.renderState;
                break;
            }

        }
    }
}
