#include "guimanager.h"

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>



#include "../farender/renderer.h"

#include <iostream>
#include <boost/python.hpp>
#include <input/common.cpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

//#include <vector>

extern bool done; // TODO: handle this better
extern bool paused; // TODO: handle this better
extern int changeLevel; // TODO: handle this better
extern int quit_key[]; // TODO: handle this better
extern int noclip_key[]; // TODO: handle this better
extern int changelvldwn_key[]; // TODO: handle this better
extern int changelvlup_key[]; // TODO: handle this better

namespace bpt = boost::property_tree;

extern bpt::ptree hotkeypt;

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
    
    boost::python::list getHotkeyNames()
    {
        boost::python::list hotkeynames;
        
        hotkeynames.append(Input::getHotkeyName(quit_key));
        hotkeynames.append(Input::getHotkeyName(noclip_key));
        hotkeynames.append(Input::getHotkeyName(changelvlup_key));
        hotkeynames.append(Input::getHotkeyName(changelvldwn_key));
        return hotkeynames;
    }
    
    boost::python::list getHotkeys()
    {
        boost::python::list hotkeys;
        boost::python::list quit;
        boost::python::list noclip;
        boost::python::list changelvlup;
        boost::python::list changelvldwn;
        
        quit.append(Input::convertAsciiToRocketKey(quit_key[0]));
        noclip.append(Input::convertAsciiToRocketKey(noclip_key[0]));
        changelvlup.append(Input::convertAsciiToRocketKey(changelvlup_key[0]));
        changelvldwn.append(Input::convertAsciiToRocketKey(changelvldwn_key[0]));
        
        for (int i=1; i<5; i++)
        {
            
            quit.append(quit_key[i]);
            noclip.append(noclip_key[i]);
            changelvlup.append(changelvlup_key[i]);
            changelvldwn.append(changelvldwn_key[i]);
        }

        hotkeys.append(quit);
        hotkeys.append(noclip);
        hotkeys.append(changelvlup);
        hotkeys.append(changelvldwn);
        
        return hotkeys;
    }
    
    void setHotkey(boost::python::list hotkey)
    {
        std::string function = boost::python::extract<std::string>(hotkey[0]);
        boost::python::list pykeys = boost::python::extract<boost::python::list>(hotkey[1]);
        int keys [5];
        
        boost::python::ssize_t n = boost::python::len(pykeys);
        for(boost::python::ssize_t i=0;i<n;i++) 
        {
            keys[i] = boost::python::extract<int>(pykeys[i]);
            //std::cout  << keys[i] << std::endl;
        }
        keys[0] = Input::convertRocketKeyToAscii(keys[0]);
        //std::cout  << keys[0] << std::endl;
         
        if (function == "quit")
        {
            for (int i=0; i<5; i++)
            {
                quit_key[i] = keys[i];
            }
            hotkeypt.put("Quit.key", quit_key[0]);
            hotkeypt.put("Quit.shift", quit_key[1]);
            hotkeypt.put("Quit.ctrl", quit_key[2]);
            hotkeypt.put("Quit.alt", quit_key[3]);
            hotkeypt.put("Quit.super", quit_key[4]);
            bpt::write_ini("resources/hotkeys.ini", hotkeypt);
        }
        if (function == "noclip")
        {
            for (int i=0; i<5; i++)
            {
                noclip_key[i] = keys[i];
            }
            hotkeypt.put("Noclip.key", noclip_key[0]);
            hotkeypt.put("Noclip.shift", noclip_key[1]);
            hotkeypt.put("Noclip.ctrl", noclip_key[2]);
            hotkeypt.put("Noclip.alt", noclip_key[3]);
            hotkeypt.put("Noclip.super", noclip_key[4]);
            bpt::write_ini("resources/hotkeys.ini", hotkeypt);
        }
        if (function == "changelvlup")
        {
            for (int i=0; i<5 ; i++)
            {
                changelvlup_key[i] = keys[i];
            }
            hotkeypt.put("Changelvlup.key", changelvlup_key[0]);
            hotkeypt.put("Changelvlup.shift", changelvlup_key[1]);
            hotkeypt.put("Changelvlup.ctrl", changelvlup_key[2]);
            hotkeypt.put("Changelvlup.alt", changelvlup_key[3]);
            hotkeypt.put("Changelvlup.super", changelvlup_key[4]);
            bpt::write_ini("resources/hotkeys.ini", hotkeypt);
        }
        if (function == "changelvldwn")
        {
            for (int i=0; i<5; i++)
            {
                changelvldwn_key[i] = keys[i];
            }
            hotkeypt.put("Changelvldwn.key", changelvldwn_key[0]);
            hotkeypt.put("Changelvldwn.shift", changelvldwn_key[1]);
            hotkeypt.put("Changelvldwn.ctrl", changelvldwn_key[2]);
            hotkeypt.put("Changelvldwn.alt", changelvldwn_key[3]);
            hotkeypt.put("Changelvldwn.super", changelvldwn_key[4]);
            bpt::write_ini("resources/hotkeys.ini", hotkeypt);
        }
    }

    BOOST_PYTHON_MODULE(freeablo)
    {
        boost::python::def("quit", &quitGame);
        boost::python::def("pause", &pauseGame);
        boost::python::def("unpause", &unpauseGame);
        boost::python::def("startGame", &startGame);
        boost::python::def("getHotkeyNames", &getHotkeyNames);
        boost::python::def("getHotkeys", &getHotkeys);
        boost::python::def("setHotkey", &setHotkey);
    }
    
    Rocket::Core::ElementDocument* ingameUi = NULL;
    Rocket::Core::ElementDocument* mainMenu = NULL;
    void initGui()
    {
        initfreeablo();
        FARender::Renderer* renderer = FARender::Renderer::get();
        ingameUi = renderer->getRocketContext()->LoadDocument("resources/gui/bottommenu.rml");
        mainMenu = renderer->getRocketContext()->LoadDocument("resources/gui/mainmenu.rml");
    }

    void showIngameGui()
    {
        mainMenu->Hide();
        ingameUi->Show();
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

    void destroyGui()
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        renderer->getRocketContext()->UnloadAllDocuments();
    }
}
