#pragma once
#include <string>
#include <vector>

namespace DiabloExe
{
    struct TalkData
    {
        std::string text;
        std::string talkAudioPath;

        bool empty() const { return text.empty(); }
    };

    struct QuestTalkData
    {
        TalkData activation;
        std::vector<TalkData> returned;
        TalkData completion;

        TalkData info;
    };
}
