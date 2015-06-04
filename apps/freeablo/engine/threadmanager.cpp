#include "threadmanager.h"

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

        while(true)
        {
            while(mQueue.pop(msg))
                handleMessage(msg);

            inputManager->poll();

            if(!renderer->renderFrame(mRenderState))
                break;

            if(mRenderState)
                mRenderState->ready = true;

            mRenderState = NULL;
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
                if(mRenderState)
                    mRenderState->ready = true;

                mRenderState = message.data.renderState;
                break;
            }
        }
    }
}
