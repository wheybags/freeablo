#include "guimanager.h"

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>



#include "../farender/renderer.h"

#include <iostream>
#include <boost/python.hpp>

extern bool done; // TODO: handle this better
extern bool paused; // TODO: handle this better
extern int changeLevel; // TODO: handle this better

namespace FAGui
{
    void quitGame()
    {
        done = true;
    }

    void pauseGame()
    {
        paused = true;
    }

    void unpauseGame()
    {
        paused = false;
    }

    void startGame()
    {
        changeLevel = 1;
        paused = false;
        showIngameGui();
    }

    BOOST_PYTHON_MODULE(freeablo)
    {
        boost::python::def("quit", &quitGame);
        boost::python::def("pause", &pauseGame);
        boost::python::def("unpause", &unpauseGame);
        boost::python::def("startGame", &startGame);
    }
    
    Rocket::Core::ElementDocument* ingameUi = NULL;
    Rocket::Core::ElementDocument* mainMenu = NULL;
    void initGui()
    {
        initfreeablo();
        FARender::Renderer* renderer = FARender::Renderer::get();
        ingameUi = renderer->getRocketContext()->LoadDocument("resources/gui/base.rml");
        mainMenu = renderer->getRocketContext()->LoadDocument("resources/gui/mainmenu.rml");
    }

    void showIngameGui()
    {
        mainMenu->Hide();
        ingameUi->Show();
        ingameUi->PushToBack(); // base.rml is an empty sheet that covers the whole screen for
                                // detecting clicks outside the gui, push it to back so it doesn't
                                // block clicks on the real gui.
    }

    void showMainMenu()
    {
        ingameUi->Hide();
        mainMenu->Show();
    }
    
    void updateGui()
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        renderer->getRocketContext()->Update();
    }
}
