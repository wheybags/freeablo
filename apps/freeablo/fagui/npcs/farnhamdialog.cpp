#include "farnhamdialog.h"

namespace FAGui
{
    FarnhamDialog::FarnhamDialog(GuiManager& guiManager, FAWorld::Actor* actor)
        : CharacterDialoguePopup(guiManager, false, "sfx/towners/Drunk27.wav"), mActor(actor)
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

    CharacterDialoguePopup::DialogData FarnhamDialog::getDialogData()
    {
        DialogData retval;
        auto& td = mActor->getMenuTalkData();

        retval.introduction = {{td.at("introductionHeader"), TextColor::golden, false}};

        retval.addMenuOption({{td.at("introduction"), TextColor::golden, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });

        retval.addMenuOption({{td.at("talk"), TextColor::blue}}, [this]() {
            openTalkDialog(mActor);
            return CharacterDialoguePopup::UpdateResult::DoNothing;
        });
        retval.addMenuOption({{td.at("quit")}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }
}
