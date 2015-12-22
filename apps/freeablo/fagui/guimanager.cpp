#include "guimanager.h"

#include <string>

#include <Rocket/Core.h>

#include <input/hotkey.h>

#include "../faworld/world.h"
#include "../farender/renderer.h"
#include "animateddecoratorinstancer.h"
#include "../engine/threadmanager.h"
#include "scrollbox.h"



namespace FAGui
{   
    Rocket::Core::ElementDocument* titleScreen = NULL;
    Rocket::Core::ElementDocument* ingameUi = NULL;
    Rocket::Core::ElementDocument* mainMenu = NULL;
    Rocket::Core::ElementDocument* credits = NULL;
    Rocket::Core::ElementDocument* chooseClassMenu = NULL;
    Rocket::Core::ElementDocument* enterNameMenu = NULL;
    Rocket::Core::ElementDocument* invalidNameMenu = NULL;
    Rocket::Core::ElementDocument* selectHeroMenu = NULL;

    std::string GuiManager::invClass;

    GuiManager::GuiManager(FAWorld::Inventory & playerInventory, Engine::EngineMain& engine, std::string invClass) : mPythonFuncs(playerInventory, *this, engine), mCurrentGuiType(TitleScreen)
    {

        this->invClass = invClass;
        initPython(mPythonFuncs);

        Input::Hotkey::initpythonwrapper();

        FARender::Renderer* renderer = FARender::Renderer::get();

        Rocket::Core::DecoratorInstancer* animInstancer = Rocket::Core::Factory::RegisterDecoratorInstancer("faanim", (Rocket::Core::DecoratorInstancer*)new AnimatedDecoratorInstancer(renderer->getRocketContext()->GetRenderInterface()));
        animInstancer->RemoveReference();

        titleScreen = renderer->getRocketContext()->LoadDocument("resources/gui/titlescreen.rml");
        ingameUi = renderer->getRocketContext()->LoadDocument("resources/gui/base.rml");
        mainMenu = renderer->getRocketContext()->LoadDocument("resources/gui/mainmenu.rml");
        credits = renderer->getRocketContext()->LoadDocument("resources/gui/credits.rml");
        chooseClassMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_choose_class_menu.rml");
        enterNameMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_enter_name_menu.rml");
        invalidNameMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_invalid_name_menu.rml");
        selectHeroMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_select_hero_menu.rml");
    }

    void GuiManager::showTitleScreen()
    {
        titleScreen->Show();
        titleScreen->PullToFront();

        mFadeCurrentDocument = titleScreen;
        mStartTime = std::chrono::system_clock::now();
        mCurrentGuiType = TitleScreen;
    }

    void GuiManager::showIngameGui()
    {
        hideAllMenus();
        ingameUi->Show();
        ingameUi->PushToBack(); // base.rml is an empty sheet that covers the whole screen for
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
        mFadeCurrentDocument = mainMenu;

        Engine::ThreadManager& threadManager = *Engine::ThreadManager::get();
        threadManager.playMusic("music/dintro.wav");

        hideAllMenus();
        ingameUi->Hide();
        mainMenu->Show();
        startFadeIn(mainMenu);

        mCurrentGuiType = MainMenu;
    }

    void GuiManager::showCredits()
    {
        startFadeOut(&GuiManager::showCreditsCallback);
    }

    void GuiManager::showCreditsCallback()
    {
        mFadeCurrentDocument = credits;

        mCreditsScrollBox = std::make_shared<ScrollBox>(credits);

        hideAllMenus();
        credits->Show();
        startFadeIn(credits);

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
        mFadeCurrentDocument = selectHeroMenu;

        hideAllMenus();
        selectHeroMenu->Show();
        startFadeIn(selectHeroMenu);
    }

    void GuiManager::showSelectHeroMenuNoFadeCallback()
    {
        mFadeCurrentDocument = selectHeroMenu;

        hideAllMenus();
        selectHeroMenu->Show();
    }

    void GuiManager::showChooseClassMenu()
    {
        hideAllMenus();
        chooseClassMenu->Show();
    }

    void GuiManager::showEnterNameMenu(int classNumber)
    {
        hideAllMenus();
        enterNameMenu->SetAttribute<int>("selectedClass", classNumber);
        enterNameMenu->Show();
    }

    void GuiManager::showInvalidNameMenu(int classNumber)
    {
        hideAllMenus();
        invalidNameMenu->SetAttribute<int>("selectedClass", classNumber);
        invalidNameMenu->Show();
    }

    GuiManager::GuiType GuiManager::currentGuiType() const
    {
        return mCurrentGuiType;
    }

    void GuiManager::hideAllMenus()
    {
        titleScreen->Hide();
        mainMenu->Hide();
        chooseClassMenu->Hide();
        invalidNameMenu->Hide();
        enterNameMenu->Hide();
        selectHeroMenu->Hide();
    }

    void GuiManager::update(bool paused)
    {
        updateGui(paused);

        FARender::Renderer* renderer = FARender::Renderer::get();

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
