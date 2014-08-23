#include "guimanager.h"

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>



#include "../farender/renderer.h"

#include <iostream>
#include <boost/python.hpp>

extern bool done; // TODO: handle this better

namespace FAGui
{
    void quitGame()
    {
        done = true;
    }

    BOOST_PYTHON_MODULE(freeablo)
    {
        boost::python::def("quit", &quitGame);
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
