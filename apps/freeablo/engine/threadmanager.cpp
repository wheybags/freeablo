#include "threadmanager.h"

#include <chrono>

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
        :mRenderState(NULL)
        ,audioManager(50, 100)
    {
        mThreadManager = this;
    }

    void ThreadManager::run()
    {
        Input::InputManager* inputManager = Input::InputManager::get();
        FARender::Renderer* renderer = FARender::Renderer::get();

        Message msg;
        
        auto last = std::chrono::system_clock::now();
        size_t numFrames = 0;

        while(true)
        {
            while(mQueue.pop(msg))
                handleMessage(msg);

            inputManager->poll();

            if(!renderer->renderFrame(mRenderState))
                break;
            
            auto now = std::chrono::system_clock::now();
            numFrames++;
            
            size_t dur = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch() - last.time_since_epoch()).count();
            
            if(dur >= 1000)
            {
                std::cout << "FPS: " << ((float)numFrames) / (((float)dur)/1000.0f) << std::endl;
                numFrames = 0;
                last = now;
            }
        }

        renderer->cleanup();
    }


    void ThreadManager::playMusic(const std::string& path)
    {
        Message msg;
        msg.type = musicPlay;
        msg.data.musicPath = new std::string(path);

        mQueue.push(msg);
    }

    void ThreadManager::playSound(const std::string& path)
    {
        Message msg;
        msg.type = soundPlay;
        msg.data.soundPath = new std::string(path);

        mQueue.push(msg);
    }

    void ThreadManager::sendRenderState(FARender::RenderState* state)
    {
        Message msg;
        msg.type = renderState;
        msg.data.renderState = state;

        mQueue.push(msg);
    }



    void ThreadManager::handleMessage(const Message& message)
    {
        switch(message.type)
        {
            case musicPlay:
            {
                audioManager.playMusic(*message.data.musicPath);
                delete message.data.musicPath;
                break;
            }

            case soundPlay:
            {
                audioManager.play(*message.data.soundPath);
                delete message.data.soundPath;
                break;
            }

            case renderState:
            {
				if (mRenderState && mRenderState != message.data.renderState)
                    mRenderState->ready = true;

                mRenderState = message.data.renderState;
                break;
            }

        }
    }
}
