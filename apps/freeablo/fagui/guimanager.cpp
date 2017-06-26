#include "guimanager.h"

#include <string>

#include <Rocket/Core.h>

#include <input/hotkey.h>

#include "../faworld/world.h"
#include "../farender/renderer.h"
#include "animateddecoratorinstancer.h"
#include "../engine/threadmanager.h"
#include "../engine/enginemain.h"
#include "scrollbox.h"



namespace FAGui
{   
    std::map<std::string, Rocket::Core::ElementDocument*> menus;

    std::string GuiManager::invClass;

    GuiManager::GuiManager(FAWorld::Inventory & playerInventory, Engine::EngineMain& engine, std::string invClass)
        : mPythonFuncs(playerInventory, *this, engine),
          mDocument(nullptr),
          mCurrentGuiType(TitleScreen),
          mEngine(engine)
    {

        this->invClass = invClass;
        initializePython(mPythonFuncs);

        FARender::Renderer* renderer = FARender::Renderer::get();

        Rocket::Core::DecoratorInstancer* animInstancer = Rocket::Core::Factory::RegisterDecoratorInstancer("faanim", (Rocket::Core::DecoratorInstancer*)new AnimatedDecoratorInstancer(renderer->getRocketContext()->GetRenderInterface()));
        animInstancer->RemoveReference();

        menus["titleScreen"] = renderer->getRocketContext()->LoadDocument("resources/gui/titlescreen.rml");
        menus["ingameUi"] = renderer->getRocketContext()->LoadDocument("resources/gui/base.rml");
        menus["mainMenu"] = renderer->getRocketContext()->LoadDocument("resources/gui/mainmenu.rml");
        menus["credits"] = renderer->getRocketContext()->LoadDocument("resources/gui/credits.rml");
        menus["chooseClassMenu"] = renderer->getRocketContext()->LoadDocument("resources/gui/creator/choose_class_menu.rml");
        menus["enterNameMenu"] = renderer->getRocketContext()->LoadDocument("resources/gui/creator/enter_name_menu.rml");
        menus["invalidNameMenu"] = renderer->getRocketContext()->LoadDocument("resources/gui/creator/invalid_name_menu.rml");
        menus["selectHeroMenu"] = renderer->getRocketContext()->LoadDocument("resources/gui/creator/select_hero_menu.rml");
        menus["saveFileExistsMenu"] = renderer->getRocketContext()->LoadDocument("resources/gui/creator/save_file_exists_menu.rml");
    }

    void GuiManager::openDialogue(const std::string& document)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        if(mDocument != nullptr)
            mDocument->Close();

        mDocument = renderer->getRocketContext()->LoadDocument(document.c_str());
        mDocument->Show();
    }

    void GuiManager::closeDialogue()
    {
        if(mDocument != nullptr)
            mDocument->Close();
        mDocument = nullptr;
    }

    bool GuiManager::isDialogueOpened() const
    {
        return mDocument != nullptr;
    }

    void GuiManager::openDialogueScrollbox(const std::string& document)
    {
        openDialogue(document);
        mDialogueScrollBox = std::make_shared<ScrollBox>(mDocument);
    }

    void GuiManager::closeDialogueScrollbox()
    {
        mDialogueScrollBox = nullptr;
        closeDialogue();
    }

    bool GuiManager::isDialogueScrollboxOpened() const
    {
        return isDialogueOpened();
    }

    void GuiManager::showTitleScreen()
    {
        menus["titleScreen"]->Show();
        menus["titleScreen"]->PullToFront();

        mFadeCurrentDocument = menus["titleScreen"];
        mStartTime = std::chrono::system_clock::now();
        mCurrentGuiType = TitleScreen;

        startFadeIn(menus["titleScreen"]);
    }

    void GuiManager::showIngameGui()
    {
        hideAllMenus();
        menus["ingameUi"]->Show();
        menus["ingameUi"]->PushToBack(); // base.rml is an empty sheet that covers the whole screen for
        // detecting clicks outside the gui, push it to back so it doesn't
        // block clicks on the real gui.

        mCurrentGuiType = IngameMenu;
    }


    void GuiManager::showMainMenu()
    {
        startFadeOut(&GuiManager::showMainMenuCallback);
    }

    void GuiManager::showMainMenuCallback()
    {
        mFadeCurrentDocument = menus["mainMenu"];

        Engine::ThreadManager& threadManager = *Engine::ThreadManager::get();
        threadManager.playMusic("music/dintro.wav");

        hideAllMenus();
        menus["mainMenu"]->Show();
        startFadeIn(menus["mainMenu"]);

        mCurrentGuiType = MainMenu;
    }

    void GuiManager::showCredits()
    {
        startFadeOut(&GuiManager::showCreditsCallback);
    }

    void GuiManager::showCreditsCallback()
    {
        mFadeCurrentDocument = menus["credits"];

        mCreditsScrollBox = std::make_shared<ScrollBox>(menus["credits"]);

        hideAllMenus();
        menus["credits"]->Show();
        startFadeIn(menus["credits"]);

        mCurrentGuiType = Credits;
    }

    void GuiManager::showSelectHeroMenu(bool fade)
    {
        if(fade)
        {
            startFadeOut(&GuiManager::showSelectHeroMenuCallback);
        }
        else
        {
            showSelectHeroMenuNoFadeCallback();
        }
    }

    void GuiManager::showSelectHeroMenuCallback()
    {
        mFadeCurrentDocument = menus["selectHeroMenu"];

        hideAllMenus();
        menus["selectHeroMenu"]->Show();
        startFadeIn(menus["selectHeroMenu"]);
    }

    void GuiManager::showSelectHeroMenuNoFadeCallback()
    {
        mFadeCurrentDocument = menus["selectHeroMenu"];

        hideAllMenus();
        menus["selectHeroMenu"]->Show();
    }

    void GuiManager::showChooseClassMenu()
    {
        hideAllMenus();
        menus["chooseClassMenu"]->Show();
    }

    void GuiManager::showEnterNameMenu(int classNumber)
    {
        hideAllMenus();
        menus["enterNameMenu"]->SetAttribute<int>("selectedClass", classNumber);
        menus["enterNameMenu"]->Show();
    }

    void GuiManager::showInvalidNameMenu(int classNumber)
    {
        hideAllMenus();
        menus["invalidNameMenu"]->SetAttribute<int>("selectedClass", classNumber);
        menus["invalidNameMenu"]->Show();
    }

    void GuiManager::showSaveFileExistsMenu(int classNumber)
    {
        hideAllMenus();
        menus["saveFileExistsMenu"]->SetAttribute<int>("selectedClass", classNumber);
        menus["saveFileExistsMenu"]->Show();
    }

    GuiManager::GuiType GuiManager::currentGuiType() const
    {
        return mCurrentGuiType;
    }

    void GuiManager::hideAllMenus()
    {
        auto it = menus.begin();
        for(;it != menus.end(); it++)
        {
            it->second->Hide();
        }
    }

    void nk_fa_begin_image_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void(void)> action)
    {
        nk_style_item tmpBg = ctx->style.window.fixed_background;
        struct nk_vec2 tmpPadding = ctx->style.window.padding;

        ctx->style.window.fixed_background = nk_style_item_image(background);
        ctx->style.window.padding = nk_vec2(0, 0);

        if (nk_begin(ctx, title, bounds, flags))
            action();

        nk_end(ctx);

        ctx->style.window.fixed_background = tmpBg;
        ctx->style.window.padding = tmpPadding;
    }

    void pauseMenu(nk_context* ctx, Engine::EngineMain& engine)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));

        if (nk_begin(ctx, "pause menu", nk_rect(0, 0, screenW, screenH), 0))
        {
            nk_layout_row_dynamic(ctx, 30, 1);
            
            nk_label(ctx, "PAUSED", NK_TEXT_CENTERED);

            if (nk_button_label(ctx, "Resume"))
                engine.unPause();

            if (nk_button_label(ctx, "Quit"))
                engine.stop();
        }
        nk_end(ctx);

        nk_style_pop_style_item(ctx);
    }

    void bottomMenu(nk_context* ctx)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        // The bottom menu is made of two sprites: panel8.cel, which is the background,
        // and panel8bu.cel, which contains overlays for each button. It's pretty primitive,
        // the buttons are baked into the background image.
        FARender::FASpriteGroup* bottomMenuTex = renderer->loadImage("ctrlpan/panel8.cel");
        FARender::FASpriteGroup* bottomMenuButtonsTex = renderer->loadImage("ctrlpan/panel8bu.cel");

        int32_t bottomMenuWidth = bottomMenuTex->getWidth();
        int32_t bottomMenuHeight = bottomMenuTex->getHeight();

        // Magic numbers defining the positions of the buttons on the background
        int32_t buttonLeftIndent    = 10;
        int32_t buttonRightIndent   = 560;

        int32_t buttonWidth         = 71;
        int32_t buttonHeight        = 19;

        int32_t buttonRow1TopIndent = 25;
        int32_t buttonRow2TopIndent = 52;
        int32_t buttonRow3TopIndent = 92;
        int32_t buttonRow4TopIndent = 118;

        // indices into panel8bu.cel
        int32_t charButtonFrame     = 0;
        int32_t questButtonFrame    = 1;
        int32_t mapButtonFrame      = 2;
        int32_t menuButtonFrame     = 3;
        int32_t invButtonFrame      = 4;
        int32_t spellsButtonFrame   = 5;

        // Centre the bottom menu on the bottom of the screen
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        struct nk_rect dims = nk_rect((screenW / 2) - (bottomMenuWidth / 2), screenH - bottomMenuHeight, bottomMenuWidth, bottomMenuHeight);

        nk_fa_begin_image_window(ctx, "bottom_menu", dims, NK_WINDOW_NO_SCROLLBAR, bottomMenuTex->getNkImage(), [&]()
        {
            nk_layout_space_begin(ctx, NK_STATIC, buttonHeight, INT_MAX);

            nk_style_button buttonStyle = ctx->style.button;
            // The "unpressed" version of the button is baked into the background image, so just draw nothing
            buttonStyle.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
            buttonStyle.hover  = nk_style_item_color(nk_rgba(0, 0, 0, 0));

            auto bottomMenuButton = [&](int32_t topIndent, int32_t leftIndent, int32_t frame)
            {
                nk_layout_space_push(ctx, nk_rect(leftIndent, topIndent, buttonWidth, buttonHeight));
                buttonStyle.active = nk_style_item_image(bottomMenuButtonsTex->getNkImage(frame));
                return nk_button_label_styled(ctx, &buttonStyle, "");
            };

            // CHAR button
            if (bottomMenuButton(buttonRow1TopIndent, buttonLeftIndent, charButtonFrame))
                std::cout << "pressed CHAR" << std::endl;

            // QUEST button
            if (bottomMenuButton(buttonRow2TopIndent, buttonLeftIndent, questButtonFrame))
                std::cout << "pressed QUEST" << std::endl;

            // MAP button
            if (bottomMenuButton(buttonRow3TopIndent, buttonLeftIndent, mapButtonFrame))
                std::cout << "pressed MAP" << std::endl;

            // MENU button
            if (bottomMenuButton(buttonRow4TopIndent, buttonLeftIndent, menuButtonFrame))
                std::cout << "pressed MENU" << std::endl;

            // INV button
            if (bottomMenuButton(buttonRow1TopIndent, buttonRightIndent, invButtonFrame))
                std::cout << "pressed INV" << std::endl;

            // SPELLS button
            if (bottomMenuButton(buttonRow2TopIndent, buttonRightIndent, spellsButtonFrame))
                std::cout << "pressed SPELLS" << std::endl;

            nk_layout_space_end(ctx);
        });
    }

    void GuiManager::update(bool paused, nk_context* ctx)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        bottomMenu(ctx);

        if (paused)
            pauseMenu(ctx, mEngine);

        updateGui(paused);
        renderer->getRocketContext()->Update();
    }

    void GuiManager::updateGui(bool paused)
    {
        if(paused)
        {
            if(currentGuiType() == TitleScreen)
            {
                static const int WAIT_TIME = 7000;

                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - mStartTime.time_since_epoch()).count();
                if(duration > WAIT_TIME)
                {
                    showMainMenu();
                    mCurrentGuiType = Other;
                }
            }
            else if(currentGuiType() == Credits)
            {
                mCreditsScrollBox->update();
                if(mCreditsScrollBox->isFinished())
                {
                    showMainMenu();
                    mCurrentGuiType = Other;
                }
            }

            // Process event from queue

            if(!mStateQueue.empty())
            {
                switch(mStateQueue.front())
                {
                    case FadeIn:
                        updateFadeIn();
                        break;
                    case FadeOut:
                        updateFadeOut();
                        break;
                }
            }

        }

        if(mDialogueScrollBox)
        {
            mDialogueScrollBox->update();
            if(mDialogueScrollBox->isFinished())
            {
                closeDialogueScrollbox();
            }
        }
    }

    void GuiManager::startFadeIn(Rocket::Core::ElementDocument * document)
    {
        mFadeCurrentDocument = document;
        mFadeValue = 255.0f;
        mStateQueue.push(FadeIn);

        showFadeElement();
        computeFadeDelta();
    }

    void GuiManager::startFadeOut(std::function<void(GuiManager&)> callback)
    {
        mFadeValue = 0.0f;
        mFadeOutCallback = callback;
        mStateQueue.push(FadeOut);

        showFadeElement();
        computeFadeDelta();
    }

    void GuiManager::updateFadeIn()
    {
        if(mFadeValue <= 0.0f)
        {
            stopFadeIn();
            return;
        }

        mFadeValue -= mFadeDelta;
        auto element = mFadeCurrentDocument->GetElementById("fade");
        if(element)
        {
            auto value = std::to_string(int(mFadeValue));
            auto rgba = "rgba(0,0,0," + value + ")";
            element->SetProperty("background-color", rgba.c_str() );
        }
    }

    void GuiManager::updateFadeOut()
    {
        if(mFadeValue >= 255.0f || mFadeCurrentDocument == NULL)
        {
            stopFadeOut();
            return;
        }

        mFadeValue += mFadeDelta;
        auto element = mFadeCurrentDocument->GetElementById("fade");
        if(element)
        {
            auto value = std::to_string(int(mFadeValue));
            auto rgba = "rgba(0,0,0," + value + ")";
            element->SetProperty("background-color", rgba.c_str() );
        }
    }

    void GuiManager::stopFadeIn()
    {
        hideFadeElement();
        mStateQueue.pop();
    }

    void GuiManager::stopFadeOut()
    {
        hideFadeElement();
        mStateQueue.pop();
        mFadeOutCallback(*this);
    }

    void GuiManager::hideFadeElement()
    {
        if(mFadeCurrentDocument)
        {
            auto element = mFadeCurrentDocument->GetElementById("fade");
            if(element)
            {
                element->SetProperty("display", "none");
            }
        }
    }

    void GuiManager::showFadeElement()
    {
        if(mFadeCurrentDocument)
        {
            auto element = mFadeCurrentDocument->GetElementById("fade");
            if(element)
            {
                element->SetProperty("display", "block");
                element->SetProperty("background-color", "black");
            }
        }
    }

    void GuiManager::computeFadeDelta()
    {
        if(mFadeCurrentDocument)
        {
            auto element = mFadeCurrentDocument->GetElementById("fade");
            if(element)
            {
                 float duration = element->GetProperty("duration")->Get<float>();
                 mFadeDelta = 255.0f / FAWorld::World::ticksPerSecond / duration;
            }
        }

    }
}
