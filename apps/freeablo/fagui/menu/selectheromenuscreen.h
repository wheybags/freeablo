#pragma once
#include "menuscreen.h"
#include <boost/optional.hpp>
#include <memory>

namespace FARender
{
    class AnimationPlayer;
}

namespace DiabloExe
{
    class CharacterStats;
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
        // TODO: replace when appropriate enum will be program wide
        enum class ClassType
        {
            warrior = 0,
            rogue,
            sorcerer,
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
        boost::optional<characterInfo> mSelectedCharacterInfo;
        int activeItemIndex = 0;
    };
}
