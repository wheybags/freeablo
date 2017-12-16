#pragma once
#include "../faworld/gamelevel.h"
#include "fa_nuklear.h"
#include <boost/optional/optional.hpp>
#include <memory>

namespace DiabloExe
{
    class CharacterStats;
}

namespace Engine
{
    class EngineMain;
}

struct nk_context;

namespace FARender
{
    class AnimationPlayer;
}

namespace FAGui
{
    class MenuHandler;

    enum class MenuFontColor
    {
        gold,
        silver,
    };

    class MenuScreen
    {
    public:
        explicit MenuScreen(MenuHandler& menu);
        virtual ~MenuScreen();
        virtual void update(nk_context* ctx) = 0;

    protected:
        static void menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment);

    protected:
        MenuHandler& mMenuHandler;
    };

    class PauseMenuScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;

    public:
        explicit PauseMenuScreen(MenuHandler& menu);
        void bigTGoldText(nk_context* ctx, const char* text, nk_flags alignment);
        float bigTGoldTextWidth(const char* text);
        void menuItems(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        std::unique_ptr<FARender::AnimationPlayer> mBigPentagram;
        int activeItemIndex = 0;
    };

    class StartingScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;

    public:
        explicit StartingScreen(MenuHandler& menu);
        void menuItems(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        int activeItemIndex = 0;
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus42;
    };

    class SelectHeroScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;
        enum class ContentType
        {
            heroList,
            chooseClass,
            enterName,
        };
        // TODO: replace when appropriate enum will be program wide
        enum class ClassType
        {
            warrior = 0,
            rogue,
            sorceror,
        };

        class characterInfo
        {
        public:
            characterInfo(ClassType charClassArg, const DiabloExe::CharacterStats& stats);

            int level;
            int strength;
            int magic;
            int dexterity;
            int vitality;
            ClassType charClass;
        };

    public:
        explicit SelectHeroScreen(MenuHandler& menu);
        bool chooseClass(nk_context* ctx);
        void content(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus;
        std::unique_ptr<FARender::AnimationPlayer> mFocus16;
        ContentType mContentType = ContentType::chooseClass;
        boost::optional<characterInfo> mSelectedCharacterInfo;
        int activeItemIndex = 0;
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
        void quit();
        void startGame();
        bool isActive() const { return !!mActiveScreen; }
        void disable();
        Engine::EngineMain& engine() { return mEngine; }

    private:
        Engine::EngineMain& mEngine;
    };
}
