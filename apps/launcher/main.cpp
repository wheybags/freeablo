/*#include <QApplication>
#include <QDir>
#include <QStyle>
#include <QDesktopWidget>
#include <SDL.h>*

#include "mainwindow.h"*/

#include <nuklearmisc/standaloneguispritehandler.h>
#include <nuklearmisc/widgets.h>
#include <faio/faio.h>
#include <climits>

int main(int, char** argv)
{
    Render::RenderSettings renderSettings;
    renderSettings.windowWidth = 800;
    renderSettings.windowHeight = 600;
    renderSettings.fullscreen = false;

    bool runFreeablo = false;

    {
        NuklearMisc::StandaloneGuiHandler guiHandler("Freeablo Launcher", renderSettings);

        nk_context* ctx = guiHandler.getNuklearContext();

        FAIO::init("");

        std::unique_ptr<NuklearMisc::GuiSprite> banner(guiHandler.getSprite(Render::loadNonCelSprite("resources/launcher/banner.png")));
        std::unique_ptr<NuklearMisc::GuiSprite> graphicsHeader(guiHandler.getSprite(Render::loadNonCelSprite("resources/launcher/graphics.png")));
        std::unique_ptr<NuklearMisc::GuiSprite> playHeader(guiHandler.getSprite(Render::loadNonCelSprite("resources/launcher/play.png")));


        int32_t bannerW, bannerH;
        Render::spriteSize(banner->getSprite()->operator[](0), bannerW, bannerH);

        renderSettings.windowWidth = bannerW;
        renderSettings.windowHeight = bannerH * 5;
        Render::setWindowSize(renderSettings);

        nk_style_button headerButtonStyle = ctx->style.button;
        headerButtonStyle.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
        assert(headerButtonStyle.hover.type == NK_STYLE_ITEM_COLOR);
        headerButtonStyle.hover.data.color.a = 180;
        assert(headerButtonStyle.active.type == NK_STYLE_ITEM_COLOR);
        headerButtonStyle.active.data.color.a = 230;
        headerButtonStyle.border = 0;

        enum class TabType
        {
            Play,
            Graphics
        };

        TabType currentTab = TabType::Play;
        float rowHeight = 30;

        Settings::Settings settings;
        settings.loadUserSettings();

        std::string diabloExePath = settings.get<std::string>("Game", "PathEXE");
        std::string mpqPath = settings.get<std::string>("Game", "PathMPQ");

        ctx->style.window.padding = nk_vec2(0, 0);
        ctx->style.window.group_padding = nk_vec2(0, 0);

        bool quit = false;
        while(!quit)
        {
            renderSettings = Render::getWindowSize();

            if(nk_begin(ctx, "main_window", nk_rect(0, 0, renderSettings.windowWidth, renderSettings.windowHeight), NK_WINDOW_NO_SCROLLBAR))
            {
                struct nk_rect bounds = nk_window_get_content_region(ctx);

                float scale = bounds.w / ((float)bannerW);
                float bannerHeightNow = scale * bannerH;

                nk_layout_row_dynamic(ctx, bannerHeightNow, 1);

                nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(banner->getNkImage(0)));
                {
                    if(nk_group_begin(ctx, "mode_selector", NK_WINDOW_NO_SCROLLBAR))
                    {
                        auto headerBounds = nk_window_get_content_region(ctx);

                        nk_layout_space_begin(ctx, NK_STATIC, headerBounds.h, INT_MAX);
                        {
                            float buttonSize = 0.7f;
                            nk_color selectedColor = nk_rgb(255, 255, 255);

                            nk_layout_space_push(ctx, nk_rect(0.1*headerBounds.h, 0.1*headerBounds.h, buttonSize*headerBounds.h, buttonSize*headerBounds.h));
                            if(nk_button_image_styled(ctx, &headerButtonStyle, playHeader->getNkImage(0)))
                                currentTab = TabType::Play;

                            nk_layout_space_push(ctx, nk_rect(0.1*headerBounds.h,
                                                              0.1*headerBounds.h + buttonSize*headerBounds.h,
                                                              buttonSize*headerBounds.h,
                                                              0.25*headerBounds.h));

                            nk_label_colored(ctx, "PLAY", NK_TEXT_CENTERED, currentTab == TabType::Play ? selectedColor : ctx->style.text.color);


                            nk_layout_space_push(ctx, nk_rect((buttonSize+0.2)*headerBounds.h,
                                                              0.1*headerBounds.h,
                                                              buttonSize*headerBounds.h,
                                                              buttonSize*headerBounds.h));

                            if(nk_button_image_styled(ctx, &headerButtonStyle, graphicsHeader->getNkImage(0)))
                                currentTab = TabType::Graphics;

                            nk_layout_space_push(ctx, nk_rect((buttonSize+0.2)*headerBounds.h,
                                                              0.1*headerBounds.h + buttonSize*headerBounds.h,
                                                              buttonSize*headerBounds.h,
                                                              0.25*headerBounds.h));

                            nk_label_colored(ctx, "GRAPHICS", NK_TEXT_CENTERED, currentTab == TabType::Graphics ? selectedColor : ctx->style.text.color);

                        }
                        nk_layout_space_end(ctx);

                        nk_group_end(ctx);
                    }
                }
                nk_style_pop_style_item(ctx);

                nk_layout_row_dynamic(ctx, bounds.h - bannerHeightNow, 1);

                nk_style_push_vec2(ctx, &ctx->style.window.group_padding, nk_vec2(10, 10));
                if(nk_group_begin(ctx, "bottom section", NK_WINDOW_NO_SCROLLBAR))
                {
                    if(currentTab == TabType::Play)
                    {
                        nk_layout_row_dynamic(ctx, rowHeight, 1);
                        NuklearMisc::nk_file_pick(ctx, "Diablo.exe", diabloExePath, "exe", rowHeight);
                        NuklearMisc::nk_file_pick(ctx, "DIABDAT.MPQ", mpqPath, "mpq,MPQ", rowHeight);
                    }
                    else if (currentTab == TabType::Graphics)
                    {
                        nk_layout_row_dynamic(ctx, rowHeight, 1);
                        nk_label(ctx, "TODO: reimplement this pane", NK_TEXT_CENTERED);
                    }

                    nk_panel* panel = nk_window_get_panel(ctx);

                    float currentY = panel->at_y + panel->row.height;
                    float remainingSpace = bounds.h - currentY;

                    struct nk_rect bottomSectionBounds = nk_window_get_content_region(ctx);

                    nk_layout_space_begin(ctx, NK_STATIC, remainingSpace, INT_MAX);
                    {
                        nk_layout_space_push(ctx, nk_rect(bottomSectionBounds.w - bottomSectionBounds.w/4.0, remainingSpace - rowHeight - 10, bottomSectionBounds.w/4.0, rowHeight));
                        nk_style_push_vec2(ctx, &ctx->style.window.group_padding, nk_vec2(0, 0));

                        if(nk_group_begin(ctx, "bottom_menu", NK_WINDOW_NO_SCROLLBAR))
                        {
                            nk_layout_row_dynamic(ctx, rowHeight, 2);

                            if(nk_button_label(ctx, "Play"))
                            {
                                quit = true;
                                runFreeablo = true;
                            }

                            if(nk_button_label(ctx, "Close"))
                                quit = true;

                            nk_group_end(ctx);
                        }

                        nk_style_pop_vec2(ctx);
                    }
                    nk_layout_space_end(ctx);

                    nk_group_end(ctx);
                }
                nk_style_pop_vec2(ctx);
            }
            nk_end(ctx);

            if(guiHandler.update())
               quit = true;
        }

        settings.set<std::string>("Game", "PathEXE", diabloExePath);
        settings.set<std::string>("Game", "PathMPQ", mpqPath);

        settings.save();

        FAIO::quit();
    }

    if(runFreeablo)
        system((boost::filesystem::system_complete(argv[0]).parent_path() / "freeablo").string().c_str());

    return 0;
}
