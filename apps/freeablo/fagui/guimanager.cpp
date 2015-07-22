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


void placeItem(uint32_t to,
               uint32_t from,
               uint32_t fromY,
               uint32_t fromX,
               uint32_t toY,
               uint32_t toX)
{
    if(fromX >= 10 || fromY >=4 || toX>=10 || toY>=4)
        return;
    Level::Item item = inventory->getItemAt(
                Level::Item::eqINV,
                fromY,
                fromX);
    if(!item.isReal() && !item.isEmpty())
    {
        item = inventory->getItemAt(
                    Level::Item::eqINV,
                    item.getCornerCoords().second,
                    item.getCornerCoords().first);

        fromX = item.getInvCoords().first;
        fromY = item.getInvCoords().second;
    }
    if(to == Level::Item::eqCURSOR)
    {
        switch(from)
        {
        case Level::Item::eqINV:
            if(inventory->getItemAt(Level::Item::eqCURSOR).isEmpty())
            {
                inventory->putItem(item, Level::Item::eqCURSOR, Level::Item::eqINV, fromY, fromX);

                return;
            }
            else
                return;
            break;
        case Level::Item::eqLEFTHAND:
            item = inventory->getItemAt(Level::Item::eqLEFTHAND);
            inventory->putItem(item, Level::Item::eqCURSOR, Level::Item::eqLEFTHAND , toY, toX);
            //inventory->dump();

            break;
        default:
            break;
        }
    }
    else if(to == Level::Item::eqINV)
    {


        item = inventory->getItemAt(Level::Item::eqCURSOR);
        inventory->putItem(
                    item,
                    Level::Item::eqINV,
                    Level::Item::eqCURSOR,
                    toY,
                    toX);
        return;


    }
    else if(to == Level::Item::eqLEFTHAND)
    {

        item = inventory->getItemAt(Level::Item::eqCURSOR);
        inventory->putItem(item, Level::Item::eqLEFTHAND, Level::Item::eqCURSOR, toY, toX);
        //inventory->dump();
        return;

    }
    return;
}

boost::python::dict updateInventory()
{
    boost::python::dict dict, cursorDict, headDict, amuletDict, leftHandDict, rightHandDict, leftRingDict, rightRingDict, bodyDict, itemDict;

    Level::Item cursor = inventory->getItemAt(Level::Item::eqCURSOR);
    Level::Item head = inventory->getItemAt(Level::Item::eqHEAD);
    Level::Item amulet = inventory->getItemAt(Level::Item::eqAMULET);
    Level::Item leftHand = inventory->getItemAt(Level::Item::eqLEFTHAND);
    Level::Item rightHand = inventory->getItemAt(Level::Item::eqRIGHTHAND);
    Level::Item leftRing = inventory->getItemAt(Level::Item::eqLEFTRING);
    Level::Item rightRing = inventory->getItemAt(Level::Item::eqRIGHTRING);
    Level::Item body = inventory->getItemAt(Level::Item::eqBODY);

    dict["cursor"] = cursorDict;

    cursorDict["graphic"] = cursor.mItem.graphicValue;
    cursorDict["empty"] = cursor.isEmpty();
    cursorDict["invX"] = cursor.getInvCoords().first;
    cursorDict["invY"] = cursor.getInvCoords().second;
    cursorDict["real"] = cursor.isReal();
    cursorDict["sizeX"] = cursor.getInvSize().first;
    cursorDict["sizeY"] = cursor.getInvSize().second;
    cursorDict["cornerX"] = cursor.getCornerCoords().first;
    cursorDict["cornerY"] = cursor.getCornerCoords().second;

    dict["head"] = headDict;

    headDict["graphic"] = head.mItem.graphicValue;
    headDict["empty"] = head.isEmpty();
    headDict["invX"] = head.getInvCoords().first;
    headDict["invY"] = head.getInvCoords().second;
    headDict["real"] = head.isReal();
    headDict["sizeX"] = head.getInvSize().first;
    headDict["sizeY"] = head.getInvSize().second;
    headDict["cornerX"] = head.getCornerCoords().first;
    headDict["cornerY"] = head.getCornerCoords().second;

    dict["body"] = bodyDict;

    bodyDict["graphic"] = body.mItem.graphicValue;
    bodyDict["empty"] = body.isEmpty();
    bodyDict["invX"] = body.getInvCoords().first;
    bodyDict["invY"] = body.getInvCoords().second;
    bodyDict["real"] = body.isReal();
    bodyDict["sizeX"] = body.getInvSize().first;
    bodyDict["sizeY"] = body.getInvSize().second;
    bodyDict["cornerX"] = body.getCornerCoords().first;
    bodyDict["cornerY"] = body.getCornerCoords().second;

    dict["leftHand"] = leftHandDict;

    leftHandDict["graphic"] = leftHand.mItem.graphicValue;
    leftHandDict["empty"] = leftHand.isEmpty();
    leftHandDict["invX"] = leftHand.getInvCoords().first;
    leftHandDict["invY"] = leftHand.getInvCoords().second;
    leftHandDict["real"] = leftHand.isReal();
    leftHandDict["sizeX"] = leftHand.getInvSize().first;
    leftHandDict["sizeY"] = leftHand.getInvSize().second;
    leftHandDict["cornerX"] = leftHand.getCornerCoords().first;
    leftHandDict["cornerY"] = leftHand.getCornerCoords().second;

    dict["rightHand"] = rightHandDict;

    rightHandDict["graphic"] = rightHand.mItem.graphicValue;
    rightHandDict["empty"] = rightHand.isEmpty();
    rightHandDict["invX"] = rightHand.getInvCoords().first;
    rightHandDict["invY"] = rightHand.getInvCoords().second;
    rightHandDict["real"] = rightHand.isReal();
    rightHandDict["sizeX"] = rightHand.getInvSize().first;
    rightHandDict["sizeY"] = rightHand.getInvSize().second;
    rightHandDict["cornerX"] = rightHand.getCornerCoords().first;
    rightHandDict["cornerY"] = rightHand.getCornerCoords().second;

    dict["leftRing"] = leftRingDict;

    leftRingDict["graphic"] = leftRing.mItem.graphicValue;
    leftRingDict["empty"] = leftRing.isEmpty();
    leftRingDict["invX"] = leftRing.getInvCoords().first;
    leftRingDict["invY"] = leftRing.getInvCoords().second;
    leftRingDict["real"] = leftRing.isReal();
    leftRingDict["sizeX"] = leftRing.getInvSize().first;
    leftRingDict["sizeY"] = leftRing.getInvSize().second;
    leftRingDict["cornerX"] = leftRing.getCornerCoords().first;
    leftRingDict["cornerY"] = leftRing.getCornerCoords().second;

    dict["rightRing"] = rightRingDict;

    rightRingDict["graphic"] = rightRing.mItem.graphicValue;
    rightRingDict["empty"] = rightRing.isEmpty();
    rightRingDict["invX"] = rightRing.getInvCoords().first;
    rightRingDict["invY"] = rightRing.getInvCoords().second;
    rightRingDict["real"] = rightRing.isReal();
    rightRingDict["sizeX"] = rightRing.getInvSize().first;
    rightRingDict["sizeY"] = rightRing.getInvSize().second;
    rightRingDict["cornerX"] = rightRing.getCornerCoords().first;
    rightRingDict["cornerY"] = rightRing.getCornerCoords().second;

    dict["amulet"] = amuletDict;

    amuletDict["graphic"] = amulet.mItem.graphicValue;
    amuletDict["empty"] = amulet.isEmpty();
    amuletDict["invX"] = amulet.getInvCoords().first;
    amuletDict["invY"] = amulet.getInvCoords().second;
    amuletDict["real"] = amulet.isReal();
    amuletDict["sizeX"] = amulet.getInvSize().first;
    amuletDict["sizeY"] = amulet.getInvSize().second;
    amuletDict["cornerX"] = amulet.getCornerCoords().first;
    amuletDict["cornerY"] = amulet.getCornerCoords().second;

    boost::python::list inventoryList, beltList;
    dict["inventoryBox"] = inventoryList;
    for(uint8_t i=0;i<4;i++)
    {
        for(uint8_t j=0;j<10;j++)
        {
            boost::python::dict itemDict;
            Level::Item item = inventory->getItemAt(Level::Item::eqINV, i, j);
            if(!item.isEmpty())
            {
                itemDict["graphic"] = item.mItem.graphicValue;
                itemDict["empty"] = false;
                itemDict["invX"] = item.getInvCoords().first;
                itemDict["invY"] = item.getInvCoords().second;
                itemDict["real"] = item.isReal();
                itemDict["sizeX"] = item.getInvSize().first;
                itemDict["sizeY"] = item.getInvSize().second;
                itemDict["cornerX"] = item.getCornerCoords().first;
                itemDict["cornerY"] = item.getCornerCoords().second;
            }
            else
            {
                itemDict["graphic"] = 0;
                itemDict["empty"] = true;
                itemDict["invX"] = item.getInvCoords().first;
                itemDict["invY"] = item.getInvCoords().second;
            }
            inventoryList.append(itemDict);
        }
    }
    Level::Item beltItem;
    for(uint8_t i=0;i<8;i++)
    {
        boost::python::dict beltDict;
        beltItem = inventory->getItemAt(Level::Item::eqBELT, 0, 0, i);
        beltDict["graphic"] = beltItem.mItem.graphicValue;
        beltDict["empty"] = true;
        beltDict["invX"] = beltItem.getInvCoords().first;
        beltDict["invY"] = beltItem.getInvCoords().second;
        beltDict["real"] = beltItem.isReal();
        beltDict["sizeX"] = beltItem.getInvSize().first;
        beltDict["sizeY"] = beltItem.getInvSize().second;
        beltDict["cornerX"] = beltItem.getCornerCoords().first;
        beltDict["cornerY"] = beltItem.getCornerCoords().second;
        beltList.append(beltDict);
    }
    dict["belt"] = beltList;
    return dict;
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
    boost::python::def("updateInventory", &updateInventory);
    boost::python::def("placeItem", &placeItem);
}

Rocket::Core::ElementDocument* ingameUi = NULL;
Rocket::Core::ElementDocument* mainMenu = NULL;
//    Rocket::Core::ElementDocument* cursor   = NULL;
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
    //        cursor   = renderer->getRocketContext()->LoadDocument("resources/gui/cursor.rml");
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
