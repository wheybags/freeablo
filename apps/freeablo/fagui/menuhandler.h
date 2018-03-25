
#pragma once

#include <memory>

namespace DiabloExe
{
    class CharacterStats;
}

namespace Engine
{
    enum class KeyboardInputAction;
    class EngineMain;
}

namespace FAWorld
{
    class World;
}

struct nk_context;

namespace FARender
{
    class AnimationPlayer;
}

namespace FAGui
{
    class MenuHandler;
    class MenuScreen;

    enum class MenuFontColor
    {
        gold,
        silver,
    };

    class MenuHandler
    {
    public:
        static const int menuWidth = 640;
        static const int menuHeight = 480;
        std::unique_ptr<MenuScreen> mActiveScreen;
        template <typename ScreenType> void setActiveScreen() { mActiveScreen.reset(new ScreenType(*this)); }

    public:
        std::unique_ptr<FARender::AnimationPlayer> createSmLogo();
        explicit MenuHandler(Engine::EngineMain& engine);
        void update(nk_context* ctx) const;
        bool isActive() const { return !!mActiveScreen; }
        void disable();
        Engine::EngineMain& engine() { return mEngine; }
        void notify(Engine::KeyboardInputAction action);

    private:
        Engine::EngineMain& mEngine;
    };
}
