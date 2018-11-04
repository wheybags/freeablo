#pragma once
#include <array>
#include <functional>
#include <string>
#include <vector>
#include "../engine/inputobserverinterface.h"
#include "../farender/spritecache.h"
#include "textcolor.h"
#include <boost/optional.hpp>
#include <memory>

namespace FAWorld
{
    class World;
    class Actor;
    class Item;
    struct StoreItem;
}

namespace FAGui
{
    class GuiManager;
    class CharacterDialoguePopup;

    class DialogManager
    {
    public:
        explicit DialogManager(GuiManager& gui_manager, FAWorld::World& world);
        ~DialogManager();

        void talk(const FAWorld::Actor* npc);

        void update(struct nk_context* ctx);

    public:
        GuiManager& mGuiManager;
//    private:
        FAWorld::World& mWorld;
        std::vector<std::unique_ptr<CharacterDialoguePopup>> mDialogStack;
    };
}
