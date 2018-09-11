#pragma once
#include <vector>
#include <string>
#include <fa_nuklear.h>
#include "../farender/animationplayer.h"

namespace FAGui
{
    class MouseAndClickMenu
    {
    public:
        MouseAndClickMenu();

        enum class Result
        {
            Activated,
            None
        };

        Result update(nk_context* ctx, std::vector<std::vector<std::string>>& options, struct nk_scroll& scroll);
        int32_t getSelectedIndex() { return mSelection; }

    private:
        int32_t mSelection = 0;
        int32_t mArrowKeyRepeatTimer = std::numeric_limits<int32_t>::max();
        int32_t mArrowKeyMovesGeneratedSinceKeydown = 0;
        FARender::AnimationPlayer mPentagramAnimation;
        struct nk_vec2 mLastMousePosition = {};
    };
}
