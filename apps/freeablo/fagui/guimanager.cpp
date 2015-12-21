#include "guimanager.h"

#include <Rocket/Core.h>

#include <input/hotkey.h>

#include "../farender/renderer.h"
#include "animateddecoratorinstancer.h"
#include "../engine/threadmanager.h"


namespace FAGui
{   
    Rocket::Core::ElementDocument* titleScreen = NULL;
    Rocket::Core::ElementDocument* ingameUi = NULL;
    Rocket::Core::ElementDocument* mainMenu = NULL;
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
        chooseClassMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_choose_class_menu.rml");
        enterNameMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_enter_name_menu.rml");
        invalidNameMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_invalid_name_menu.rml");
        selectHeroMenu = renderer->getRocketContext()->LoadDocument("resources/gui/creator_select_hero_menu.rml");
    }

    void GuiManager::showTitleScreen()
    {
        titleScreen->Show();
        titleScreen->PullToFront();

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
        Engine::ThreadManager& threadManager = *Engine::ThreadManager::get();
        threadManager.playMusic("music/dintro.wav");

        hideAllMenus();
        ingameUi->Hide();
        mainMenu->Show();

        mCurrentGuiType = MainMenu;
    }

    GuiManager::GuiType GuiManager::currentGuiType() const
    {
        return mCurrentGuiType;
    }

    void GuiManager::showSelectHeroMenu()
    {
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
        static const int WAIT_TIME = 7000;

        if(paused)
        {
            if(currentGuiType() == FAGui::GuiManager::TitleScreen)
            {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - mStartTime.time_since_epoch()).count();
                if(duration > WAIT_TIME)
                {
                    showMainMenu();
                }
            }
        }
    }
}
