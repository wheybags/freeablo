#include "guimanager.h"

#include <misc/disablewarn.h>
    #include <Rocket/Core.h>
#include <misc/enablewarn.h>

#include "animateddecoratorinstancer.h"

#include "../farender/renderer.h"
#include "../engine/threadmanager.h"
#include "../faworld/inventory.h"

#include <iostream>
#include <boost/python.hpp>
#include <input/common.h>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "input/hotkey.h"

extern bool done; // TODO: handle this better
extern bool paused; // TODO: handle this better
extern int changeLevel; // TODO: handle this better

extern Input::Hotkey quit_key; // TODO: handle this better
extern Input::Hotkey noclip_key; // TODO: handle this better
extern Input::Hotkey changelvldwn_key; // TODO: handle this better
extern Input::Hotkey changelvlup_key; // TODO: handle this better

namespace bpt = boost::property_tree;

extern bpt::ptree hotkeypt;

namespace FAGui
{
    FAWorld::Inventory* inventory;

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

    void playSound(const std::string& path)
    {
        Engine::ThreadManager::get()->playSound(path);
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
        Input::Hotkey pquit_key = quit_key;
        Input::Hotkey pnoclip_key = noclip_key;
        Input::Hotkey pchangelvlup_key = changelvlup_key;
        Input::Hotkey pchangelvldwn_key = changelvldwn_key;

        pquit_key.key = Input::convertAsciiToRocketKey(quit_key.key);
        pnoclip_key.key = Input::convertAsciiToRocketKey(noclip_key.key);
        pchangelvlup_key.key = Input::convertAsciiToRocketKey(changelvlup_key.key);
        pchangelvldwn_key.key = Input::convertAsciiToRocketKey(changelvldwn_key.key);
        
        hotkeys.append(pquit_key);
        hotkeys.append(pnoclip_key);
        hotkeys.append(pchangelvlup_key);
        hotkeys.append(pchangelvldwn_key);
        
        return hotkeys;
    }
    
    void setHotkey(std::string function, boost::python::list pyhotkey)
    {
        Input::Hotkey hotkey;
        hotkey.key = Input::convertRocketKeyToAscii(boost::python::extract<int>(pyhotkey[0]));
        hotkey.shift = boost::python::extract<bool>(pyhotkey[1]);
        hotkey.ctrl = boost::python::extract<bool>(pyhotkey[2]);
        hotkey.alt = boost::python::extract<bool>(pyhotkey[3]);
        
        bpt::write_ini("resources/hotkeys.ini", hotkeypt);
        
        if (function == "quit")
        {
            quit_key = hotkey;
            quit_key.save("Quit", hotkeypt);
        }
        if (function == "noclip")
        {
            noclip_key = hotkey;
            noclip_key.save("Noclip", hotkeypt);
        }
        if (function == "changelvlup")
        {
            changelvlup_key = hotkey;
            changelvlup_key.save("Changelvlup", hotkeypt);
        }
        if (function == "changelvldwn")
        {
            changelvldwn_key = hotkey;
            changelvldwn_key.save("Changelvldwn", hotkeypt);
        }
    }
    boost::python::list inventoryRML()
    {
        boost::python::list list;

        for(uint8_t i=0;i<=3;i++)
        {
            for(uint8_t j=0;j<=9;j++)
            {
                Level::Item boxitem = inventory->getItemAt(Level::Item::eqINV, i, j);

                if(!boxitem.isEmpty())
                {                   
                    boost::python::list attrList;
                    attrList.append(boxitem.mItem.graphicValue);
                    attrList.append(boxitem.isEmpty());
                    attrList.append(boxitem.getInvSize().first);
                    attrList.append(boxitem.getInvSize().second);
                    attrList.append(boxitem.mIsReal);
                    list.append(attrList);

                }
                else
                {
                    boost::python::list attrList;
                    attrList.append(0);
                    attrList.append(true);
                    list.append(attrList);


                }

            }
        }
        return list;

    }

    BOOST_PYTHON_MODULE(freeablo)
    {
        boost::python::def("quit", &quitGame);
        boost::python::def("pause", &pauseGame);
        boost::python::def("unpause", &unpauseGame);
        boost::python::def("startGame", &startGame);
        boost::python::def("playSound", &playSound);
        boost::python::def("getHotkeyNames", &getHotkeyNames);
        boost::python::def("getHotkeys", &getHotkeys);
        boost::python::def("setHotkey", &setHotkey);
       // boost::python::class_<FAWorld::Inventory>("Inventory")
        boost::python::def("getItemBox", &inventoryRML);




    }
    
    Rocket::Core::ElementDocument* ingameUi = NULL;
    Rocket::Core::ElementDocument* mainMenu = NULL;
    void initGui(FAWorld::Inventory & playerInventory)
    {
        inventory = &playerInventory;
        initfreeablo();
        Input::Hotkey::initpythonwrapper();

        FARender::Renderer* renderer = FARender::Renderer::get();

        Rocket::Core::DecoratorInstancer* animInstancer = Rocket::Core::Factory::RegisterDecoratorInstancer("faanim", (Rocket::Core::DecoratorInstancer*)new AnimatedDecoratorInstancer(renderer->getRocketContext()->GetRenderInterface()));
        animInstancer->RemoveReference();

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
