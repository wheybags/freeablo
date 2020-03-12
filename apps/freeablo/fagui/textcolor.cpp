#include "textcolor.h"
#include <fa_nuklear.h>

namespace FAGui
{
    nk_color getNkColor(TextColor color)
    {
        // Warning: These colors just placeholder similar colors (except white obviously),
        // To achieve real Diablo palette coloring of smaltext.cel we need to apply palette shift
        // which could not be represented as color multiplication so it's bettter to
        // probably generate separate textures in the time of decoding.
        switch (color)
        {
            case TextColor::white:
                return nk_color{255, 255, 255, 255};
            case TextColor::blue:
                return nk_color{170, 170, 255, 255};
            case TextColor::golden:
                return nk_color{225, 225, 155, 255};
            case TextColor::red:
                return nk_color{255, 128, 128, 255};
        }

        return nk_color{255, 255, 255, 255};
    }
}
