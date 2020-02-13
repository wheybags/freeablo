#pragma once
#include "../engine/inputobserverinterface.h"
#include "../farender/spritecache.h"
#include "textcolor.h"
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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

        void talk(FAWorld::Actor* npc);
        void update(struct nk_context* ctx);

        void pushDialog(CharacterDialoguePopup* dialog);
        void popDialog();
        bool hasDialog() const { return !mDialogStack.empty(); }

    public:
        GuiManager& mGuiManager;
        FAWorld::World& mWorld;

    private:
        std::vector<std::unique_ptr<CharacterDialoguePopup>> mDialogStack;
    };
}
