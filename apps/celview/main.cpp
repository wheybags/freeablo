#include "../components/settings/settings.h"
#include <cel/celfile.h>
#include <chrono>
#include <faio/fafileobject.h>
#include <fmt/format.h>
#include <input/inputmanager.h>
#include <misc/stringops.h>
#include <nfd.h>
#include <nuklearmisc/inputfwd.h>
#include <nuklearmisc/standaloneguispritehandler.h>
#include <nuklearmisc/widgets.h>
#include <render/render.h>
#include <render/spritegroup.h>

int main(int argc, char** argv)
{
    Misc::saveArgv0(argv[0]);

    if (argc > 2)
        message_and_abort_fmt("Usage: %s [filename]", argv[0]);

    Cel::CelDecoder::loadConfigFiles();

    Render::RenderSettings renderSettings = {};
    renderSettings.windowWidth = 800;
    renderSettings.windowHeight = 600;
    renderSettings.fullscreen = false;

    NuklearMisc::StandaloneGuiHandler guiHandler("Cel Viewer", renderSettings);

    nk_context* ctx = guiHandler.getNuklearContext();

    Settings::Settings settings;
    settings.loadFromFile(Misc::getResourcesPath().str() + "/celview.ini");

    bool faioInitDone = false;
    auto listFile = settings.get<std::string>("celview", "listFile", "Diablo I.txt");
    auto mpqFile = settings.get<std::string>("celview", "mpqFile", "DIABDAT.MPQ");

    std::vector<std::string> celFiles;

    std::string selectedImage;
    std::unique_ptr<Render::SpriteGroup> image;

    std::unique_ptr<Render::SpriteGroup> nextImage;

    int animate = false;
    int32_t frame = 0;

    float rowHeight = 30;
    auto lastFrame = std::chrono::high_resolution_clock::now();

    bool quit = false;
    while (!quit)
    {
        auto now = std::chrono::high_resolution_clock::now();

        if (nextImage)
            image = std::unique_ptr<Render::SpriteGroup>(nextImage.release());

        renderSettings = Render::getWindowSize();

        if (nk_begin(ctx, "main_window", nk_rect(0, 0, renderSettings.windowWidth, renderSettings.windowHeight), NK_WINDOW_NO_SCROLLBAR))
        {
            struct nk_rect bounds = nk_window_get_content_region(ctx);

            nk_layout_row_dynamic(ctx, bounds.h, 2);

            if (nk_group_begin(ctx, "image", 0))
            {
                nk_layout_row_dynamic(ctx, rowHeight, 1);

                std::string label = selectedImage;

                if (selectedImage.empty())
                    label = "No image selected";

                nk_label(ctx, label.c_str(), NK_TEXT_CENTERED);

                nk_checkbox_label(ctx, "Animate", &animate);

                if (image)
                {
                    nk_label(ctx, fmt::format("Number of Frames: {}", image->size()).c_str(), NK_TEXT_LEFT);
                    nk_label(ctx, fmt::format("Width: {}", image->getNkImage(frame).w).c_str(), NK_TEXT_LEFT);
                    nk_label(ctx, fmt::format("Height: {}", image->getNkImage(frame).h).c_str(), NK_TEXT_LEFT);
                    frame = nk_propertyi(ctx, "Frame", 0, frame, image->size(), 1, 0.2f);

                    if (nk_button_label(ctx, "save as png"))
                    {
                        nfdchar_t* outPath = nullptr;
                        nfdresult_t result = NFD_SaveDialog("png", nullptr, &outPath);
                        if (result == NFD_OKAY)
                        {
                            Cel::CelFile cel(selectedImage);
                            std::vector<Image> images = cel.decode();

                            int32_t sumWidth = 0;
                            int32_t maxHeight = 0;
                            for (int32_t i = 0; i < int32_t(images.size()); i++)
                            {
                                sumWidth += images[i].width();
                                if (images[i].height() > maxHeight)
                                    maxHeight = images[i].height();
                            }

                            Image tmp(sumWidth, maxHeight);

                            int32_t x = 0;
                            for (int32_t i = 0; i < int32_t(images.size()); i++)
                            {
                                images[i].blitTo(tmp, x, 0);
                                x += images[i].width();
                            }

                            Image::saveToPng(tmp, outPath);
                        }
                        free(outPath);
                    }

                    if (nk_button_label(ctx, "save as gif"))
                    {
                        nfdchar_t* outPath = nullptr;
                        nfdresult_t result = NFD_SaveDialog("gif", nullptr, &outPath);
                        if (result == NFD_OKAY)
                        {
                            Cel::CelDecoder cel(selectedImage);
                            Image::saveToGif(cel.decode(), outPath);
                        }
                        free(outPath);
                    }

                    auto msSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrame).count();
                    if (animate && msSinceLastFrame > 100)
                    {
                        lastFrame = now;
                        frame++;
                    }

                    if (frame >= image->size())
                        frame = 0;

                    int32_t w = image->getNkImage(frame).w;
                    int32_t h = image->getNkImage(frame).h;

                    nk_layout_space_begin(ctx, NK_STATIC, h, 1);
                    {
                        nk_layout_space_push(ctx, nk_rect(0, 0, w, h));

                        auto canvas = nk_window_get_canvas(ctx);

                        struct nk_rect imageRect = {};
                        nk_widget(&imageRect, ctx);
                        nk_fill_rect(canvas, imageRect, 0.0, nk_rgb(0, 255, 0));

                        auto img = image->getNkImage(frame);
                        nk_draw_image(canvas, imageRect, &img, nk_rgb(255, 255, 255));
                    }
                    nk_layout_space_end(ctx);
                }

                nk_group_end(ctx);
            }

            if (nk_group_begin(ctx, "file list", 0))
            {
                if (!faioInitDone)
                {
                    nk_layout_row_dynamic(ctx, rowHeight * 2, 1);

                    NuklearMisc::nk_file_pick(ctx, "DIABDAT.MPQ", mpqFile, "mpq,MPQ", rowHeight);
                    NuklearMisc::nk_file_pick(ctx, "Diablo listfile", listFile, "txt", rowHeight);

                    if (nk_button_label(ctx, "load"))
                    {
                        FAIO::init(mpqFile, listFile);
                        celFiles = FAIO::listMpqFiles("*.cel");
                        auto tmp = FAIO::listMpqFiles("*.cl2");
                        celFiles.insert(celFiles.end(), tmp.begin(), tmp.end());

                        std::sort(celFiles.begin(), celFiles.end(), [](const std::string& l, const std::string& r) {
                            return Misc::StringUtils::toLower(l) < Misc::StringUtils::toLower(r);
                        });

                        settings.set<std::string>("celview", "listFile", listFile);
                        settings.set<std::string>("celview", "mpqFile", mpqFile);
                        settings.save();

                        faioInitDone = true;

                        if (argc > 1)
                        {
                            selectedImage = argv[1];
                            frame = 0;

                            Cel::CelFile cel(selectedImage);
                            nextImage = std::make_unique<Render::SpriteGroup>(cel.decode());
                        }
                    }
                }

                nk_layout_row_dynamic(ctx, rowHeight, 1);

                for (auto& celFile : celFiles)
                {
                    auto buttonStyle = ctx->style.button;

                    if (selectedImage == celFile)
                        buttonStyle.normal = buttonStyle.hover;

                    if (nk_button_label_styled(ctx, &buttonStyle, celFile.c_str()))
                    {
                        selectedImage = celFile;
                        frame = 0;

                        Cel::CelFile cel(selectedImage);
                        nextImage = std::make_unique<Render::SpriteGroup>(cel.decode());
                    }
                }

                nk_group_end(ctx);
            }
        }
        nk_end(ctx);

        quit = guiHandler.update();
    }

    FAIO::quit();

    return 0;
}
