#include "input.h"

#include <boost/bind.hpp>

#include "../farender/renderer.h"
#include "../faworld/world.h"

namespace Engine
{
    bool done = false;
    bool paused = false;
    bool noclip = false;

    size_t xClick = 0, yClick = 0;
    bool mouseDown = false;
    bool click = false;

    Input::Hotkey quit_key;
    Input::Hotkey noclip_key;
    Input::Hotkey changelvldwn_key;
    Input::Hotkey changelvlup_key;

    EngineInputManager::EngineInputManager():
        mInput( boost::bind(&EngineInputManager::keyPress,this, _1),
                NULL,
                boost::bind(&EngineInputManager::mouseClick, this, _1, _2, _3),
                boost::bind(&EngineInputManager::mouseRelease, this, _1, _2, _3),
                boost::bind(&EngineInputManager::mouseMove, this, _1, _2),
                FARender::Renderer::get()->getRocketContext())
    { }

    void EngineInputManager::keyPress(Input::Key key)
    {
        switch(key)
        {
            case Input::KEY_RSHIFT:;
            case Input::KEY_LSHIFT:;
            case Input::KEY_RCTRL:;
            case Input::KEY_LCTRL:;
            case Input::KEY_RALT:;
            case Input::KEY_LALT:;
            case Input::KEY_RSUPER:;
            case Input::KEY_LSUPER:;
            case Input::KEY_NUMLOCK:;
            case Input::KEY_SCROLLOCK: return;
            default:
                {
                    break;
                }
        }

        Input::Hotkey hotkey;
        hotkey.key = key;
        Input::InputManager& input = *Input::InputManager::get();

        uint32_t modifiers = input.getModifiers();

        switch(modifiers)
        {
            case 0: break;
            case 1: hotkey.ctrl = true; break;
            case 2: hotkey.alt = true; break;
            case 3: hotkey.ctrl = true; hotkey.alt = true; break;
            case 4: hotkey.shift = true; break;
            case 5: hotkey.ctrl = true; hotkey.shift = true; break;
            case 6: hotkey.alt = true; hotkey.shift = true; break;
            case 7: hotkey.ctrl = true; hotkey.alt = true; hotkey.shift = true; break;
        }

        if (hotkey == quit_key)
        {
            done = true;
        }
        else if (hotkey == noclip_key)
        {
            noclip = !noclip;
        }
        else if (hotkey == changelvlup_key || hotkey == changelvldwn_key)
        {
            FAWorld::World* world = FAWorld::World::get();
            size_t nextLevelIndex;
            if(hotkey == changelvlup_key)
                nextLevelIndex = world->getCurrentLevel()->getPreviousLevel();
            else
                nextLevelIndex = world->getCurrentLevel()->getNextLevel();

            world->setLevel(nextLevelIndex);

            Level::Level* level = world->getCurrentLevel();
            FAWorld::Player* player = world->getPlayer();

            if(hotkey == changelvlup_key)
                player->mPos = FAWorld::Position(level->downStairsPos().first, level->downStairsPos().second);
            else
                player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);


            player->destination() = player->mPos.current();
        }
    }

    void EngineInputManager::mouseClick(size_t x, size_t y, Input::Key key)
    {
        if(key == Input::KEY_LEFT_MOUSE)
        {
            xClick = x;
            yClick = y;
            mouseDown = true;
            click = true;
        }
    }

    void EngineInputManager::mouseRelease(size_t, size_t, Input::Key key)
    {
        if(key == Input::KEY_LEFT_MOUSE)
            mouseDown = false;
    }

    void EngineInputManager::mouseMove(size_t x, size_t y)
    {
        xClick = x;
        yClick = y;
    }

    void EngineInputManager::update(bool paused)
    {
        mInput.processInput(paused);
    }
}