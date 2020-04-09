#pragma once
#include <serial/loader.h>
#include <string>
#include <vector>

namespace DiabloExe
{
    struct TalkData
    {
        std::string text;
        std::string talkAudioPath;

        void save(Serial::Saver& saver) const
        {
            saver.save(text);
            saver.save(talkAudioPath);
        }

        void load(Serial::Loader& loader)
        {
            text = loader.load<std::string>();
            talkAudioPath = loader.load<std::string>();
        }

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
