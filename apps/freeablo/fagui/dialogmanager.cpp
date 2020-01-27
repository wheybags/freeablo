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
        OgdenDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/tavown36.wav"), mActor(actor)
        {
            mActor->getBeforeDungeonTalkData().talkAudioPath = "sfx/towners/Tavown00.wav";
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/tavown37.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/tavown40.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/tavown41.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/tavown39.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/tavown45.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/tavown38.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/tavown44.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/tavown42.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/tavown43.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader1"), TextColor::golden, false}, {td.at("introductionHeader2"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        FAWorld::Actor* mActor = nullptr;
    };

    class FarnhamDialog : public CharacterDialoguePopup
    {
    public:
        FarnhamDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Drunk27.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/drunk23.wav";

                else if (gossip.first == "general2")
                    gossip.second.talkAudioPath = "sfx/towners/drunk24.wav";

                else if (gossip.first == "general3")
                    gossip.second.talkAudioPath = "sfx/towners/drunk25.wav";

                else if (gossip.first == "general4")
                    gossip.second.talkAudioPath = "sfx/towners/drunk26.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/drunk29.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/drunk30.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/drunk28.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/drunk31.wav";

                else if (gossip.first == "odgen")
                    gossip.second.talkAudioPath = "sfx/towners/drunk35.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/drunk32.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/drunk33.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/drunk34.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        FAWorld::Actor* mActor = nullptr;
    };

    class AdriaDialog : public CharacterDialoguePopup
    {
    public:
        AdriaDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Witch38.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/witch39.wav";

                else if (gossip.first == "general2")
                    gossip.second.talkAudioPath = "sfx/towners/witch40.wav";

                else if (gossip.first == "general3")
                    gossip.second.talkAudioPath = "sfx/towners/witch41.wav";

                else if (gossip.first == "general4")
                    gossip.second.talkAudioPath = "sfx/towners/witch42.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/witch45.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/witch46.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/witch44.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/witch43.wav";

                else if (gossip.first == "ogden")
                    gossip.second.talkAudioPath = "sfx/towners/witch50.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/witch47.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/witch48.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/witch49.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("buy")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("sell")}}, [this]() {
                this->openSellDialog();
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("recharge")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

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

        FAWorld::Actor* mActor = nullptr;
    };

    class WirtDialog : public CharacterDialoguePopup
    {
    public:
        WirtDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Pegboy32.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy33.wav";

                else if (gossip.first == "general2")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy34.wav";

                else if (gossip.first == "general3")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy35.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy42.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy38.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy39.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy37.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy36.wav";

                else if (gossip.first == "ogden")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy43.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy40.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/pegboy41.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("introduction1")}, {td.at("introduction2")}, {td.at("introduction3")}, {}},
                                 []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("look")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        FAWorld::Actor* mActor = nullptr;
    };

    class PepinDialog : public CharacterDialoguePopup
    {
    public:
        PepinDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Healer37.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/healer38.wav";

                else if (gossip.first == "general2")
                    gossip.second.talkAudioPath = "sfx/towners/healer39.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/healer43.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/healer41.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/healer42.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/healer47.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/healer40.wav";

                else if (gossip.first == "ogden")
                    gossip.second.talkAudioPath = "sfx/towners/healer46.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/healer44.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/healer45.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader1"), TextColor::golden, false}, {td.at("introductionHeader2"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("heal")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("buy")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        FAWorld::Actor* mActor = nullptr;
    };

    class CainDialog : public CharacterDialoguePopup
    {
    public:
        CainDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/storyt25.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/storyt26.wav";

                else if (gossip.first == "general2")
                    gossip.second.talkAudioPath = "sfx/towners/storyt27.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/storyt31.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/storyt30.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/storyt35.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/storyt28.wav";

                else if (gossip.first == "ogden")
                    gossip.second.talkAudioPath = "sfx/towners/storyt29.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/storyt34.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/storyt32.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/storyt33.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("identify")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        FAWorld::Actor* mActor = nullptr;
    };

    class GillianDialog : public CharacterDialoguePopup
    {
    public:
        GillianDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Bmaid31.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid32.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid33.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid35.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid36.wav";

                else if (gossip.first == "griswold")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid34.wav";

                else if (gossip.first == "ogden")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid40.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid37.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid38.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/bmaid39.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

            return retval;
        }

        FAWorld::Actor* mActor = nullptr;
    };

    class GriswoldDialog : public CharacterDialoguePopup
    {
    public:
        GriswoldDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Bsmith44.wav"), mActor(actor)
        {
            auto& gossipData = mActor->getGossipData();
            for (auto& gossip : gossipData)
            {
                if (gossip.first == "general1")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith45.wav";

                else if (gossip.first == "general2")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith46.wav";

                else if (gossip.first == "general3")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith47.wav";

                else if (gossip.first == "general4")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith48.wav";

                else if (gossip.first == "adria")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith49.wav";

                else if (gossip.first == "cain")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith51.wav";

                else if (gossip.first == "farnham")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith52.wav";

                else if (gossip.first == "gillian")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith50.wav";

                else if (gossip.first == "ogden")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith56.wav";

                else if (gossip.first == "pepin")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith53.wav";

                else if (gossip.first == "priest")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith54.wav";

                else if (gossip.first == "wirt")
                    gossip.second.talkAudioPath = "sfx/towners/bsmith55.wav";
            }
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;
            auto& td = mActor->getTalkData();

            retval.introduction = {{td.at("introductionHeader1"), TextColor::golden, false}, {td.at("introductionHeader2"), TextColor::golden, false}};

            retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

            retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
                openTalkDialog(mActor);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("buyBasic")}}, [this]() {
                this->openBuyDialog();
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("buyPremium")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("sell")}}, [this]() {
                this->openSellDialog();
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
            retval.addMenuOption({{td.at("repair")}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
            retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

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

        FAWorld::Actor* mActor = nullptr;
    };

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
