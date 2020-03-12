#include "mouseandclickmenu.h"

namespace FAGui
{
    MouseAndClickMenu::MouseAndClickMenu()
    {
        mPentagramAnimation.playAnimation(FARender::Renderer::get()->loadImage("data/pentspn2.cel"),
                                          FAWorld::World::getTicksInPeriod(FixedPoint("0.1")),
                                          FARender::AnimationPlayer::AnimationType::Looped);
    }

    MouseAndClickMenu::Result MouseAndClickMenu::update(nk_context* ctx, std::vector<std::vector<MenuEntry>>& options, struct nk_scroll& scroll)
    {
        mSelection = Misc::clamp(mSelection, 0, int32_t(options.size()));

        float lineHeight = 15;

        Misc::ScopedSetter<nk_style_button>(ctx->style.button);
        ctx->style.button.normal = nk_style_item_hide();
        ctx->style.button.border = 0;
        ctx->style.button.hover = ctx->style.button.normal;
        ctx->style.button.active = ctx->style.button.normal;

        auto renderer = FARender::Renderer::get();
        auto pentagram = renderer->loadImage("data/pentspn2.cel");
        nk_layout_row_template_begin(ctx, lineHeight);
        {
            nk_layout_row_template_push_static(ctx, pentagram->getWidth());
            nk_layout_row_template_push_dynamic(ctx);
            nk_layout_row_template_push_static(ctx, pentagram->getWidth());
        }
        nk_layout_row_template_end(ctx);

        auto canvas = nk_window_get_canvas(ctx);

        mPentagramAnimation.update();
        auto tmp = mPentagramAnimation.getCurrentFrame();
        struct nk_image pentagramFrame = tmp.first->getNkImage(tmp.second);

        auto drawPentagram = [&]() {
            struct nk_rect pentagramRect;
            nk_widget(&pentagramRect, ctx);

            pentagramRect.y += (pentagramRect.h / 2.0) - pentagramFrame.h / 2;
            pentagramRect.h = pentagramFrame.h;

            nk_draw_image(canvas, pentagramRect, &pentagramFrame, nk_rgba(255, 255, 255, 255));
        };

        Result result = Result::None;

        for (int32_t i = 0; i < int32_t(options.size()); i++)
        {
            struct nk_rect dummy;
            auto& opt = options.at(i);

            for (size_t j = 0; j < opt.size(); j++)
            {
                MenuEntry& entry = opt.at(j);

                if (mSelection == i && entry.clickable)
                    drawPentagram();
                else
                    nk_widget(&dummy, ctx);

                if (nk_widget_is_hovered(ctx) && (ctx->input.mouse.pos.x != mLastMousePosition.x || ctx->input.mouse.pos.y != mLastMousePosition.y) &&
                    entry.clickable)
                    mSelection = i;

                nk_color color = getNkColor(entry.textColor);
                nk_style_push_color(ctx, &ctx->style.button.text_normal, color);
                nk_style_push_color(ctx, &ctx->style.button.text_hover, color);
                nk_style_push_color(ctx, &ctx->style.button.text_active, color);

                if (nk_button_label(ctx, entry.entry.c_str()))
                    result = Result::Activated;

                nk_style_pop_color(ctx);
                nk_style_pop_color(ctx);
                nk_style_pop_color(ctx);

                if (mSelection == i && entry.clickable)
                    drawPentagram();
                else
                    nk_widget(&dummy, ctx);
            }
        }

        float areaHeight = nk_window_get_content_region(ctx).h;
        float fullLineHeight = lineHeight + ctx->style.window.spacing.y;

        auto getEntryYPosition = [&](int32_t entryIndex) {
            int32_t lineCount = 0;
            int32_t i = 0;
            for (; i < entryIndex && i < int32_t(options.size()); i++)
                lineCount += options[i].size();

            lineCount += entryIndex - i;

            return lineCount * fullLineHeight;
        };

        if (nk_input_is_key_down(&ctx->input, NK_KEY_DOWN) || nk_input_is_key_down(&ctx->input, NK_KEY_UP))
        {
            FAWorld::Tick firstWait = FAWorld::World::getTicksInPeriod(FixedPoint("0.5"));
            FAWorld::Tick repeatWait = FAWorld::World::getTicksInPeriod(FixedPoint("0.05"));

            if (mArrowKeyRepeatTimer > (mArrowKeyMovesGeneratedSinceKeydown < 2 ? firstWait : repeatWait))
            {
                if (nk_input_is_key_down(&ctx->input, NK_KEY_DOWN))
                    mSelection = (mSelection + 1) % int32_t(options.size());
                if (nk_input_is_key_down(&ctx->input, NK_KEY_UP))
                    mSelection = mSelection - 1 >= 0 ? mSelection - 1 : int32_t(options.size()) - 1;

                if (getEntryYPosition(mSelection) < scroll.y)
                    scroll.y = getEntryYPosition(mSelection);

                if (getEntryYPosition(mSelection + 1) > areaHeight + scroll.y)
                    scroll.y = getEntryYPosition(mSelection + 1) + ctx->style.window.group_padding.y - areaHeight;

                mArrowKeyRepeatTimer = 0;
                mArrowKeyMovesGeneratedSinceKeydown++;
            }

            mArrowKeyRepeatTimer++;
        }
        else
        {
            mArrowKeyRepeatTimer = std::numeric_limits<int32_t>::max();
            mArrowKeyMovesGeneratedSinceKeydown = 0;
        }

        if (nk_input_is_key_pressed(&ctx->input, NK_KEY_ENTER))
            result = Result::Activated;

        mLastMousePosition = ctx->input.mouse.pos;

        return result;
    }
}
