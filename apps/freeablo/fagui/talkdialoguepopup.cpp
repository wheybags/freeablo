#include "talkdialoguepopup.h"
#include "guimanager.h"
#include <render/spritegroup.h>

namespace FAGui
{
    TalkDialoguePopup::TalkDialoguePopup(GuiManager& guiManager, DiabloExe::TalkData talk) : CharacterDialoguePopup(guiManager, true), mTalkData(talk) {}

    CharacterDialoguePopup::UpdateResult TalkDialoguePopup::update(struct nk_context* ctx)
    {
        auto renderer = FARender::Renderer::get();

        Render::SpriteGroup* boxTex = renderer->mSpriteLoader.getSprite(renderer->mSpriteLoader.mGuiSprites.textbox);
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        nk_flags flags = NK_WINDOW_NO_SCROLLBAR;
        // any bigger than 75 offset for width seems to expose artifacts of the background sprite
        auto dialogRectangle = nk_rect(
            screenW / 2.0f - (boxTex->getWidth() / 2.0f), screenH / 2.0f - (boxTex->getHeight() / 2.0f), boxTex->getWidth() + 75, boxTex->getHeight() + 50);

        auto textScrollRectangle = nk_rect(dialogRectangle.x + 30, dialogRectangle.y + 20, boxTex->getWidth(), boxTex->getHeight());

        UpdateResult result = UpdateResult::DoNothing;

        mGuiManager.nk_fa_begin_image_window(
            ctx, "talkPopup", dialogRectangle, flags, boxTex->getNkImage(), [&]() { drawBackgroundCheckerboard(renderer, ctx, dialogRectangle); }, true);

        // creating another window allows for texts to scroll without going out of the textured background frame
        mGuiManager.nk_fa_begin_window(
            ctx,
            "textScroll",
            textScrollRectangle,
            flags,
            [&]() {
                // fill the rest of the window
                struct nk_rect bounds = nk_widget_bounds(ctx);
                struct nk_rect panelSize = nk_window_get_bounds(ctx);
                float contentHeight = panelSize.h + panelSize.y - bounds.y;
                nk_layout_row_dynamic(ctx, contentHeight, 1);

                ctx->style.window.fixed_background.data.color = nk_color{0, 0, 0, 0};

                auto wrapText = [](nk_context* ctx, const char* text, TextColor color) {
                    FARender::Renderer* renderer = FARender::Renderer::get();
                    nk_style_push_font(ctx, renderer->bigTGoldFont());
                    nk_style_push_color(ctx, &ctx->style.text.color, getNkColor(color));
                    nk_label_wrap(ctx, text);
                    nk_style_pop_color(ctx);
                    nk_style_pop_font(ctx);
                };

                auto& world = mGuiManager.mDialogManager.mWorld;
                static auto startTime = world.getCurrentTick();
                wrapText(ctx, mTalkData.text.c_str(), TextColor::white);
                auto currentTime = world.getCurrentTick();
                if (currentTime - startTime >= world.getTicksInPeriod("0.1"))
                {
                    ctx->active->scrollbar.y++;
                    startTime = currentTime;
                }
            },
            true);

        return result;
    }

    CharacterDialoguePopup::DialogData TalkDialoguePopup::getDialogData()
    {
        DialogData retval;

        retval.introduction = {};

        return retval;
    }

    const DiabloExe::TalkData& TalkDialoguePopup::getTalkData() const { return mTalkData; }
}
