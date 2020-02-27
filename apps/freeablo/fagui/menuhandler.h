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
        template <typename ScreenType> void setActiveScreen() { mNextMenu.reset(new ScreenType(*this)); }

    public:
        std::unique_ptr<FARender::AnimationPlayer> createSmLogo();
        explicit MenuHandler(Engine::EngineMain& engine);
        void update(nk_context* ctx);
        bool isActive() const { return mActiveScreen || mNextMenu; }
        void disable();
        Engine::EngineMain& engine() { return mEngine; }
        void notify(Engine::KeyboardInputAction action);

    private:
        std::unique_ptr<MenuScreen> mActiveScreen;
        std::unique_ptr<MenuScreen> mNextMenu;
        Engine::EngineMain& mEngine;
    };
}
