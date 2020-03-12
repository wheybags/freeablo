#include "ogdendialog.h"

namespace FAGui
{
    OgdenDialog::OgdenDialog(GuiManager& guiManager, FAWorld::Actor* actor)
        : CharacterDialoguePopup(guiManager, false, "sfx/towners/tavown36.wav"), mActor(actor)
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

    CharacterDialoguePopup::DialogData OgdenDialog::getDialogData()
    {
        DialogData retval;
        auto& td = mActor->getMenuTalkData();

        retval.introduction = {{td.at("introductionHeader1"), TextColor::golden, false}, {td.at("introductionHeader2"), TextColor::golden, false}};

        retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

        retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
            openTalkDialog(mActor);
            return CharacterDialoguePopup::UpdateResult::DoNothing;
        });
        retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }
}
