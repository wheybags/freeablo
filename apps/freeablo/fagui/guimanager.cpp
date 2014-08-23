#include "guimanager.h"

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>



#include "../farender/renderer.h"

#include <iostream>
#include <boost/python.hpp>

extern bool done; // TODO: handle this better
extern bool paused; // TODO: handle this better

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

    BOOST_PYTHON_MODULE(freeablo)
    {
        boost::python::def("quit", &quitGame);
        boost::python::def("pause", &pauseGame);
        boost::python::def("unpause", &unpauseGame);
    }

    void initGui()
    {
        initfreeablo();

        FARender::Renderer* renderer = FARender::Renderer::get();
        Rocket::Core::ElementDocument* doc = renderer->getRocketContext()->LoadDocument("resources/gui/bottommenu.rml");
        doc->Show();
    }

    void updateGui()
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        renderer->getRocketContext()->Update();
    }

    void destroyGui()
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        renderer->getRocketContext()->UnloadAllDocuments();
    }
}
