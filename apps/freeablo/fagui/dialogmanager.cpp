#include "dialogmanager.h"
#include "../engine/enginemain.h"
#include "../engine/localinputhandler.h"
#include "../faworld/actor.h"
#include "../faworld/equiptarget.h"
#include "../faworld/player.h"
#include "../faworld/storedata.h"
#include "characterdialoguepopup.h"
#include "guimanager.h"
#include "mouseandclickmenu.h"
#include "nkhelpers.h"
#include "shopdialogs.h"
#include <misc/assert.h>
#include <utility>

namespace FAGui
{
    DialogManager::DialogManager(GuiManager& gui_manager, FAWorld::World& world) : mGuiManager(gui_manager), mWorld(world) {}

    DialogManager::~DialogManager() = default;

    class OgdenDialog : public CharacterDialoguePopup
    {
    public:
        OgdenDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader1"), td.at("introductionHeader2")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class FarnhamDialog : public CharacterDialoguePopup
    {
    public:
        FarnhamDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class AdriaDialog : public CharacterDialoguePopup
    {
    public:
        AdriaDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("buy")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("sell")}, [this]() {
                this->openSellDialog();
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({td.at("recharge")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        void openSellDialog()
        {
            auto dialog = new ShopSellDialog(mGuiManager, *mActor, adriaSellFilter);
            mGuiManager.mDialogManager.pushDialog(dialog);
        }

        static bool adriaSellFilter(const FAWorld::Item& item)
        {
            // TODO: add check for quest items
            return item.getType() == FAWorld::ItemType::misc || item.getType() == FAWorld::ItemType::staff;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class WirtDialog : public CharacterDialoguePopup
    {
    public:
        WirtDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader")};

            retval.addMenuOption({td.at("talk"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("introduction1"), td.at("introduction2"), td.at("introduction3"), ""},
                                 []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("look")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class PepinDialog : public CharacterDialoguePopup
    {
    public:
        PepinDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader1"), td.at("introductionHeader2")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("heal")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("buy")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class CainDialog : public CharacterDialoguePopup
    {
    public:
        CainDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("identify")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class GillianDialog : public CharacterDialoguePopup
    {
    public:
        GillianDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    class GriswoldDialog : public CharacterDialoguePopup
    {
    public:
        GriswoldDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {td.at("introductionHeader1"), td.at("introductionHeader2")};

            retval.addMenuOption({td.at("introduction"), ""}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({td.at("talk")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("buyBasic")}, [this]() {
                this->openBuyDialog();
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({td.at("buyPremium")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("sell")}, [this]() {
                this->openSellDialog();
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({td.at("repair")}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({td.at("quit")}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        void openBuyDialog()
        {
            auto dialog = new ShopBuyDialog(mGuiManager, *mActor, mGuiManager.mDialogManager.mWorld.getStoreData().griswoldBasicItems);
            mGuiManager.mDialogManager.pushDialog(dialog);
        }

        void openSellDialog()
        {
            auto dialog = new ShopSellDialog(mGuiManager, *mActor, griswoldSellFilter);
            mGuiManager.mDialogManager.pushDialog(dialog);
        }

        static bool griswoldSellFilter(const FAWorld::Item& item)
        {
            // TODO: add check for quest items
            return item.getType() != FAWorld::ItemType::misc && item.getType() != FAWorld::ItemType::staff;
        }

        const FAWorld::Actor* mActor = nullptr;
    };

    void DialogManager::talk(const FAWorld::Actor* npc)
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
                mDialogStack.pop_back();
        }
    }

    void DialogManager::pushDialog(CharacterDialoguePopup* dialog) { mDialogStack.emplace_back(dialog); }
}
