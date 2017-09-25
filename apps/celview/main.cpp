//#include "mainwindow.h"
//#include <QApplication>

#include <render/render.h>
#include <nuklearmisc/inputfwd.h>
#include <input/inputmanager.h>
#include <faio/fafileobject.h>
#include <chrono>

#include <nfd.h>

struct CVSprite
{
    CVSprite(Render::SpriteGroup* sprite, uint32_t cacheIndex) : sprite(sprite), cacheIndex(cacheIndex)
    {
        for(uint32_t i = 0; i < sprite->size(); i++)
        {
            id id;
            id.cacheIndex = cacheIndex;
            id.frameIndex = i;

            frameIds.push_back(id);
        }
    }

    Render::SpriteGroup* sprite;
    uint32_t cacheIndex;

    struct id
    {
        uint32_t cacheIndex;
        uint32_t frameIndex;
    };

    std::vector<id> frameIds;

    struct nk_image getNkImage(int32_t frame)
    {
        return nk_image_handle(nk_handle_ptr(&frameIds[frame]));
    }
};

uint32_t nextSpriteId = 1;
std::map<uint32_t, CVSprite*> sprites;

CVSprite* getCVSprite(Render::SpriteGroup* sprite)
{
    auto retval = new CVSprite(sprite, nextSpriteId);
    sprites[nextSpriteId] = retval;
    nextSpriteId++;
    return retval;
}

class CVSpriteCache : public Render::SpriteCacheBase
{
    public:
    virtual Render::SpriteGroup* get(uint32_t key)
    {
        return sprites[key]->sprite;
    }

    virtual void setImmortal(uint32_t, bool)
    {

    }
};

void nk_fa_font_stash_begin(nk_font_atlas& atlas)
{
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
}

CVSprite* aaaa;

nk_handle nk_fa_font_stash_end(nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    CVSprite* sprite = getCVSprite(Render::loadSprite((uint8_t*)image, w, h));
    aaaa = sprite;
    //FASpriteGroup* sprite = spriteManager.getFromRaw((uint8_t*)image, w, h);
    //spriteManager.setImmortal(sprite->getCacheIndex(), true);

    nk_handle handle = sprite->getNkImage(0).handle;
    nk_font_atlas_end(&atlas, handle, &nullTex);

    if (atlas.default_font)
        nk_style_set_font(ctx, &atlas.default_font->handle);

    return handle;
}

float rowHeight = 30;

bool nk_file_pick(nk_context* ctx, const std::string& label, std::string& path, const std::string& filter, float labelWidth = 120)
{
    // TODO: replace nasty static buffer, we could use the more advanced api, but I'm not bothered right now
    char buf[4096];

    assert(path.size()+1 < 4096);
    memcpy(buf, path.c_str(), path.size()+1);

    bool retval = false;

    auto groupName = std::string("file_pick_group_") + std::to_string(size_t(&path));
    if(nk_group_begin(ctx, groupName.c_str(), 0))
    {
        nk_layout_row_template_begin(ctx, rowHeight);
        {
            nk_layout_row_template_push_static(ctx, labelWidth);
            nk_layout_row_template_push_variable(ctx, 80);
            nk_layout_row_template_push_static(ctx, 40);
        }
        nk_layout_row_template_end(ctx);

        nk_label(ctx, label.c_str(), NK_TEXT_LEFT);

        if(nk_edit_string_zero_terminated(ctx, NK_EDIT_SIMPLE, buf, 4096, nk_filter_default) == NK_EDIT_COMMITED)
            retval = true;

        if(nk_button_label(ctx, "pick"))
        {
            nfdchar_t *outPath = NULL;
            nfdresult_t result = NFD_OpenDialog(filter.c_str(), NULL, &outPath);
            if(result == NFD_OKAY)
            {
                path = outPath;
                free(outPath);
                retval = true;
            }
        }
        else
        {
            path = buf;
        }

        nk_group_end(ctx);
    }

    return retval;
}

int main(int, char**)
{
    /*QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();*/

    Render::RenderSettings renderSettings;
    renderSettings.windowWidth = 800;
    renderSettings.windowHeight = 600;
    renderSettings.fullscreen = false;


    Render::NuklearGraphicsContext nuklearGraphicsContext;
    nk_context ctx;

    nk_init_default(&ctx, nullptr);
    ctx.clip.copy = nullptr;// nk_sdl_clipbard_copy;
    ctx.clip.paste = nullptr;// nk_sdl_clipbard_paste;
    ctx.clip.userdata = nk_handle_ptr(0);

    Render::init(renderSettings, nuklearGraphicsContext, &ctx);

    // Load Cursor: if you uncomment cursor loading please hide the cursor
    {
        nk_fa_font_stash_begin(nuklearGraphicsContext.atlas);
        //struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
        //struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
        //struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
        //struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
        //struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
        //struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
        nuklearGraphicsContext.dev.font_tex = nk_fa_font_stash_end(&ctx, nuklearGraphicsContext.atlas, nuklearGraphicsContext.dev.null);
        //nk_style_load_all_cursors(ctx, atlas->cursors);
        //nk_style_set_font(ctx, &roboto->handle);
    }

    NuklearFrameDump nuklearData(nuklearGraphicsContext.dev);

    Input::InputManager input(
        [&] (Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(&ctx, true, key, input.getModifiers()); },
        [&] (Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(&ctx, false, key, input.getModifiers()); },
        [&] (int32_t x, int32_t y, Input::Key key, bool isDoubleClick) { NuklearMisc::handleNuklearMouseEvent(&ctx, x, y, key, true, isDoubleClick); },
        [&] (int32_t x, int32_t y, Input::Key key) { NuklearMisc::handleNuklearMouseEvent(&ctx, x, y, key, false, false); },
        [&] (int32_t x, int32_t y, int32_t xrel, int32_t yrel) { NuklearMisc::handleNuklearMouseMoveEvent(&ctx, x, y, xrel, yrel); },
        [&] (int32_t x, int32_t y) { NuklearMisc::handleNuklearMouseWheelEvent(&ctx, x, y); },
        [&] (std::string inp) { NuklearMisc::handleNuklearTextInputEvent(&ctx, inp); }
    );


    Settings::Settings settings;
    settings.loadFromFile("resources/celview.ini");

    bool faioInitDone = false;
    std::string listFile = settings.get<std::string>("celview", "listFile", "Diablo I.txt");
    std::string mpqFile = settings.get<std::string>("celview", "mpqFile", "DIABDAT.MPQ");

    std::vector<std::string> celFiles;

    std::string selectedImage = "";
    std::unique_ptr<CVSprite> image;

    std::unique_ptr<CVSprite> nextImage;

    int animate = false;
    int32_t frame = 0;

    auto lastFrame = std::chrono::high_resolution_clock::now();

    while(true)
    {
        auto now = std::chrono::high_resolution_clock::now();

        if(nextImage)
            image = std::unique_ptr<CVSprite>(nextImage.release());

        input.poll();

        nk_input_begin(&ctx);
        input.processInput(false);
        nk_input_end(&ctx);

        Render::clear();

        renderSettings = Render::getWindowSize();

        if(nk_begin(&ctx, "main_window", nk_rect(0, 0, renderSettings.windowWidth, renderSettings.windowHeight), NK_WINDOW_NO_SCROLLBAR))
        {
            struct nk_rect bounds = nk_window_get_content_region(&ctx);

            nk_layout_row_dynamic(&ctx, bounds.h, 2);

            if(nk_group_begin(&ctx, "image", 0))
            {
                nk_layout_row_dynamic(&ctx, rowHeight, 1);

                std::string label = selectedImage;

                if(selectedImage == "")
                    label = "No image selected";

                nk_label(&ctx, label.c_str(), NK_TEXT_CENTERED);

                nk_checkbox_label(&ctx, "Animate", &animate);

                if(image)
                {
                    frame = nk_propertyi(&ctx, "Frame", 0, frame, image.get()->sprite->size(), 1, 0.2);

                    if(nk_button_label(&ctx, "save as png"))
                    {
                        nfdchar_t *outPath = NULL;
                        nfdresult_t result = NFD_SaveDialog("png", NULL, &outPath);
                        if(result == NFD_OKAY)
                        {
                            Render::SpriteGroup::toPng(selectedImage, outPath);
                            free(outPath);
                        }
                    }

                    auto msSinceLastFrame = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrame).count();
                    if(animate && msSinceLastFrame > 100)
                    {
                        lastFrame = now;
                        frame++;
                    }

                    if(frame >= (int32_t)image.get()->sprite->size())
                        frame = 0;

                    Render::Sprite sprite = image.get()->sprite->operator[](frame);

                    int32_t w, h;
                    Render::spriteSize(sprite, w, h);


                    nk_layout_space_begin(&ctx, NK_STATIC, h, 1);
                    {
                        nk_layout_space_push(&ctx, nk_rect(0, 0, w, h));

                        auto canvas = nk_window_get_canvas(&ctx);

                        struct nk_rect imageRect;
                        nk_widget(&imageRect, &ctx);
                        nk_fill_rect(canvas, imageRect, 0.0, nk_rgb(0, 255, 0));

                        auto img = image.get()->getNkImage(frame);
                        nk_draw_image(canvas, imageRect, &img, nk_rgb(255, 255, 255));
                    }
                    nk_layout_space_end(&ctx);
                }


                nk_group_end(&ctx);
            }

            if(nk_group_begin(&ctx, "file list", 0))
            {
                if(!faioInitDone)
                {
                    nk_layout_row_dynamic(&ctx, rowHeight*2, 1);

                    nk_file_pick(&ctx, "DIABDAT.MPQ", mpqFile, "mpq,MPQ");
                    nk_file_pick(&ctx, "Diablo listfile", listFile, "txt");

                    if(nk_button_label(&ctx, "load"))
                    {
                        FAIO::init(mpqFile, listFile);
                        celFiles = FAIO::listMpqFiles("*.cel");
                        auto tmp = FAIO::listMpqFiles("*.cl2");
                        celFiles.insert(celFiles.end(), tmp.begin(), tmp.end());

                        std::sort(celFiles.begin(), celFiles.end());

                        settings.set<std::string>("celview", "listFile", listFile);
                        settings.set<std::string>("celview", "mpqFile", mpqFile);
                        settings.save();

                        faioInitDone = true;
                    }
                }

                nk_layout_row_dynamic(&ctx, rowHeight, 1);

                for(size_t i = 0; i < celFiles.size(); i++)
                {
                    auto buttonStyle = ctx.style.button;

                    if(selectedImage == celFiles[i])
                        buttonStyle.normal = buttonStyle.hover;

                    if(nk_button_label_styled(&ctx, &buttonStyle, celFiles[i].c_str()))
                    {
                        selectedImage = celFiles[i];
                        frame = 0;
                        nextImage = std::unique_ptr<CVSprite>(getCVSprite(new Render::SpriteGroup(selectedImage)));
                    }
                }

                nk_group_end(&ctx);
            }
        }
        nk_end(&ctx);

        nuklearData.fill(&ctx);

        CVSpriteCache cache;
        Render::drawGui(nuklearData, &cache);
        nk_clear(&ctx);

        Render::draw();
    }


    return 0;
}
