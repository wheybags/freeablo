#include "dialogmanager.h"
#include "../engine/enginemain.h"
#include "../engine/localinputhandler.h"
#include "../engine/threadmanager.h"
#include "../faworld/actor.h"
#include "../faworld/equiptarget.h"
#include "../faworld/player.h"
#include "../faworld/storedata.h"
#include "characterdialoguepopup.h"
#include "guimanager.h"
#include "mouseandclickmenu.h"
#include "nkhelpers.h"
#include "npcs/adriadialog.h"
#include "npcs/caindialog.h"
#include "npcs/farnhamdialog.h"
#include "npcs/gilliandialog.h"
#include "npcs/griswolddialog.h"
#include "npcs/ogdendialog.h"
#include "npcs/pepindialog.h"
#include "npcs/wirtdialog.h"
#include <misc/assert.h>
#include <utility>

namespace FAGui
{
    DialogManager::DialogManager(GuiManager& gui_manager, FAWorld::World& world) : mGuiManager(gui_manager), mWorld(world) {}

    DialogManager::~DialogManager() = default;

    void DialogManager::talk(FAWorld::Actor* npc)
    {
        auto npcId = npc->getNpcId();

        CharacterDialoguePopup* dialog = nullptr;

        if (npcId == "NPCsmith")
            dialog = new GriswoldDialog(mGuiManager, npc);
        else if (npcId == "NPCtavern")
            dialog = new OgdenDialog(mGuiManager, npc);
        else if (npcId == "NPCdrunk")
            dialog = new FarnhamDialog(mGuiManager, npc);
        else if (npcId == "NPCmaid")
            dialog = new GillianDialog(mGuiManager, npc);
        else if (npcId == "NPCboy")
            dialog = new WirtDialog(mGuiManager, npc);
        else if (npcId == "NPChealer")
            dialog = new PepinDialog(mGuiManager, npc);
        else if (npcId == "NPCwitch")
            dialog = new AdriaDialog(mGuiManager, npc);
        else if (npcId == "NPCstorytell")
            dialog = new CainDialog(mGuiManager, npc);

        if (dialog)
            pushDialog(dialog);
    }

    void DialogManager::update(struct nk_context* ctx)
    {
        if (!mDialogStack.empty())
        {
            CharacterDialoguePopup::UpdateResult result = mDialogStack[mDialogStack.size() - 1]->update(ctx);
            if (result == CharacterDialoguePopup::UpdateResult::PopDialog)
                popDialog();
        }
    }

    void DialogManager::pushDialog(CharacterDialoguePopup* dialog)
    {
        mDialogStack.emplace_back(dialog);

        const std::string& greetingPath = dialog->getGreetingPath();
        if (!greetingPath.empty())
            Engine::ThreadManager::get()->playSound(greetingPath);
    }

    void DialogManager::popDialog()
    {
        mDialogStack.pop_back();
        Engine::ThreadManager::get()->stopSound();
    }
}
