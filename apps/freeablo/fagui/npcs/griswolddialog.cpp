#include "griswolddialog.h"
#include "../../faworld/storedata.h"
#include "../guimanager.h"
#include "../shopdialogs.h"

namespace FAGui
{
    GriswoldDialog::GriswoldDialog(GuiManager& guiManager, FAWorld::Actor* actor)
        : CharacterDialoguePopup(guiManager, false, "sfx/towners/Bsmith44.wav"), mActor(actor)
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

        auto& questData = mActor->getQuestTalkData();
        for (auto& quest : questData)
        {
            if (quest.first == "anvilOfFury")
            {
            }

            else if (quest.first == "archbishopLazarus")
            {
            }

            else if (quest.first == "blackMushroom")
            {
            }

            else if (quest.first == "hallsOfTheBlind")
            {
            }

            else if (quest.first == "lachdanan")
            {
            }

            else if (quest.first == "ogdensSign")
            {
            }

            else if (quest.first == "poisonedWaterSupply")
            {
            }

            else if (quest.first == "theButcher")
            {
            }

            else if (quest.first == "theChamberOfBone")
            {
            }

            else if (quest.first == "theCurseOfKingLeoric")
            {
            }

            else if (quest.first == "theMagicRock")
            {
            }

            else if (quest.first == "valor")
            {
            }

            else if (quest.first == "warlordOfBlood")
            {
            }
        }
    }

    CharacterDialoguePopup::DialogData GriswoldDialog::getDialogData()
    {
        DialogData retval;
        auto& td = mActor->getMenuTalkData();

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

    void GriswoldDialog::openBuyDialog()
    {
        auto dialog = new ShopBuyDialog(mGuiManager, *mActor, mGuiManager.mDialogManager.mWorld.getStoreData().griswoldBasicItems);
        mGuiManager.mDialogManager.pushDialog(dialog);
    }

    void GriswoldDialog::openSellDialog()
    {
        auto dialog = new ShopSellDialog(mGuiManager, *mActor, griswoldSellFilter);
        mGuiManager.mDialogManager.pushDialog(dialog);
    }

    bool GriswoldDialog::griswoldSellFilter(const FAWorld::Item& item)
    {
        // TODO: add check for quest items
        return item.getType() != FAWorld::ItemType::misc && item.getType() != FAWorld::ItemType::staff;
    }
}
