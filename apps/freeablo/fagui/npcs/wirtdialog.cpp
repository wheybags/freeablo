#include "wirtdialog.h"

namespace FAGui
{
    WirtDialog::WirtDialog(GuiManager& guiManager, FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false, "sfx/towners/Pegboy32.wav"), mActor(actor)
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

    CharacterDialoguePopup::DialogData WirtDialog::getDialogData()
    {
        DialogData retval;
        auto& td = mActor->getMenuTalkData();

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
}
