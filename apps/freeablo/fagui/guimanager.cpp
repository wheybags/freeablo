#include "guimanager.h"

#include <Rocket/Core.h>

#include <input/hotkey.h>

#include "../farender/renderer.h"
#include "animateddecoratorinstancer.h"


namespace FAGui
{   
    Rocket::Core::ElementDocument* titleScreen = NULL;
    Rocket::Core::ElementDocument* ingameUi = NULL;
    Rocket::Core::ElementDocument* mainMenu = NULL;

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

    }

    void GuiManager::showTitleScreen()
    {
        titleScreen->Show();
        titleScreen->PullToFront();

        mCurrentGuiType = TitleScreen;
    }

    void GuiManager::showIngameGui()
    {
        mainMenu->Hide();
        ingameUi->Show();
        ingameUi->PushToBack(); // base.rml is an empty sheet that covers the whole screen for
        // detecting clicks outside the gui, push it to back so it doesn't
        // block clicks on the real gui.

        mCurrentGuiType = IngameMenu;
    }


    void GuiManager::showMainMenu()
    {
        titleScreen->Hide();
        ingameUi->Hide();
        mainMenu->Show();

        mCurrentGuiType = MainMenu;
    }

    GuiManager::GuiType GuiManager::currentGuiType() const
    {
        return mCurrentGuiType;
    }


    void GuiManager::updateGui()
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        renderer->getRocketContext()->Update();
    }
}
