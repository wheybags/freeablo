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
        :mMusic(NULL)
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

        Message msg;

        while(true)
        {
            while(mQueue.pop(msg))
                handleMessage(msg);

            inputManager->poll();

            if(!renderer->renderFrame())
                break;
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

    void ThreadManager::handleMessage(const Message& message)
    {
        switch(message.type)
        {
            case musicPlay:
            {
                if(mMusic != NULL)
                    Audio::freeMusic(mMusic);

                mMusic = Audio::loadMusic(*message.data.musicPath);
                delete message.data.musicPath;

                Audio::playMusic(mMusic);
                break;
            }

            default:
            {
                break;
            }
        }
    }
}
