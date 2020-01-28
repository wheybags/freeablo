#pragma once

#include "adriadialog.h"
#include "../guimanager.h"
#include "../shopdialogs.h"

namespace FAGui
{
    AdriaDialog::AdriaDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Witch38.wav"), mActor(actor)
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

    CharacterDialoguePopup::DialogData AdriaDialog::getDialogData()
    {
        DialogData retval;
        auto& td = mActor->getMenuTalkData();

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

    void AdriaDialog::openSellDialog()
    {
        auto dialog = new ShopSellDialog(mGuiManager, *mActor, adriaSellFilter);
        mGuiManager.mDialogManager.pushDialog(dialog);
    }

    bool AdriaDialog::adriaSellFilter(const FAWorld::Item& item)
    {
        // TODO: add check for quest items
        return item.getType() == FAWorld::ItemType::misc || item.getType() == FAWorld::ItemType::staff;
    }
}
