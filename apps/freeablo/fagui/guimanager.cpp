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
    std::map<std::string, Rocket::Core::ElementDocument*> menus;

    std::string GuiManager::invClass;

    GuiManager::GuiManager(FAWorld::Inventory & playerInventory, Engine::EngineMain& engine, std::string invClass)
        : mPythonFuncs(playerInventory, *this, engine),
          mDocument(nullptr),
          mCurrentGuiType(TitleScreen)
    {

        this->invClass = invClass;
        initPython(mPythonFuncs);

        Input::Hotkey::initpythonwrapper();

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
