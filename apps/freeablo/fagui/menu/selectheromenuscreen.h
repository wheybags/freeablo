#pragma once
#include "menuscreen.h"
#include <memory>
#include <optional>

namespace FARender
{
    class AnimationPlayer;
}

namespace DiabloExe
{
    class CharacterStats;
}

namespace FAWorld
{
    enum class PlayerClass;
}

namespace FAGui
{
    class SelectHeroMenuScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;
        enum class ContentType
        {
            heroList,
            chooseClass,
            enterName,
        };

        class characterInfo
        {
        public:
            characterInfo(FAWorld::PlayerClass charClassArg, const DiabloExe::CharacterStats& stats);

            int level;
            int strength;
            int magic;
            int dexterity;
            int vitality;
            FAWorld::PlayerClass charClass;
        };

    public:
        explicit SelectHeroMenuScreen(MenuHandler& menu);
        ActionResult chooseClass(nk_context* ctx);
        void generateChooseClassMenu();
        void setType(ContentType type);
        void content(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus;
        std::unique_ptr<FARender::AnimationPlayer> mFocus16;
        ContentType mContentType = ContentType::chooseClass;
        std::optional<characterInfo> mSelectedCharacterInfo;
    };
}
