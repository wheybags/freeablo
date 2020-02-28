#include "menuhandler.h"
#include "../engine/enginemain.h"
#include "../faworld/world.h"
#include "menu/menuscreen.h"

namespace FAGui
{
    std::unique_ptr<FARender::AnimationPlayer> MenuHandler::createSmLogo()
    {
        auto ret = std::make_unique<FARender::AnimationPlayer>();
        auto renderer = FARender::Renderer::get();
        ret->playAnimation(renderer->loadImage("ui_art/smlogo.pcx&trans=0,255,0&vanim=154"),
                           FAWorld::World::getTicksInPeriod("0.06"),
                           FARender::AnimationPlayer::AnimationType::Looped);
        return ret;
    }

    MenuHandler::MenuHandler(Engine::EngineMain& engine) : mEngine(engine) {}

    void MenuHandler::update(nk_context* ctx)
    {
        if (mNextMenu)
            mActiveScreen.reset(mNextMenu.release());

        if (mActiveScreen)
            mActiveScreen->update(ctx);
    }

    void MenuHandler::disable()
    {
        mActiveScreen.reset();
        mNextMenu.reset();
    }

    void MenuHandler::notify(Engine::KeyboardInputAction action)
    {
        if (mActiveScreen)
            mActiveScreen->notify(action);
    }
}
