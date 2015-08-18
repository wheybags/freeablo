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
std::string cursorPath;
uint32_t cursorFrame;
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

std::string getInvClass()
{
    return invClass;
}

void placeItem(uint32_t toPara,
               uint32_t fromPara,
               uint32_t fromY,
               uint32_t fromX,
               uint32_t toY,
               uint32_t toX,
               uint32_t beltX)
{

    if(fromX >= 10 || fromY >=4 || toX>=10 || toY>=4)
        return;
    FAWorld::Item::equipLoc to   = static_cast<FAWorld::Item::equipLoc>(toPara);
    FAWorld::Item::equipLoc from = static_cast<FAWorld::Item::equipLoc>(fromPara);
    FAWorld::Item item = inventory->getItemAt(
                from,
                fromY,
                fromX, beltX);
    if(item.isEmpty())
        return;
    if(!item.isReal() && !item.isEmpty())
    {
        item = inventory->getItemAt(
                    FAWorld::Item::eqINV,
                    item.getCornerCoords().second,
                    item.getCornerCoords().first);

        fromX = item.getInvCoords().first;
        fromY = item.getInvCoords().second;
    }
    if(to == FAWorld::Item::eqCURSOR)
    {

            if(inventory->getItemAt(FAWorld::Item::eqCURSOR).isEmpty())
            {

                if(inventory->putItem(item, FAWorld::Item::eqCURSOR, from, fromY, fromX, beltX))
                {

                    if(item.getGraphicValue() > 179)
                    {

                        cursorPath = "";
                        cursorFrame = 0;
                    }
                    else
                    {
                        cursorPath = "data/inv/objcurs.cel";
                        cursorFrame = item.getGraphicValue();

                    }
                }

            }

    }

    else if(to == FAWorld::Item::eqINV)
    {

        item = inventory->getItemAt(FAWorld::Item::eqCURSOR);
        if(inventory->putItem(item, to, FAWorld::Item::eqCURSOR, toY, toX))
        {
            cursorPath = "";
            cursorFrame = 0;
        }

    }
    else if(to == FAWorld::Item::eqLEFTHAND || to == FAWorld::Item::eqRIGHTHAND)
    {

        item = inventory->getItemAt(FAWorld::Item::eqCURSOR);
        if(inventory->putItem(item, to, FAWorld::Item::eqCURSOR, toY, toX))
        {
            cursorPath = "";
            cursorFrame = 0;
        }

    }
    else if(to == FAWorld::Item::eqLEFTRING || to == FAWorld::Item::eqRIGHTRING)
    {
        item = inventory->getItemAt(FAWorld::Item::eqCURSOR);
        if (inventory->putItem(item, to, FAWorld::Item::eqCURSOR, toY, toX))
        {
            cursorPath = "";
            cursorFrame = 0;
        }

    }

    else
    {

        item = inventory->getItemAt(from, fromY, fromX);
        if(inventory->putItem(item, to, from, toY, toX, beltX))
        {
            cursorPath = "";
            cursorFrame = 0;

        }

    }
    return;
}

boost::python::dict updateInventory()
{
    boost::python::dict dict, cursorDict, headDict, amuletDict, leftHandDict, rightHandDict, leftRingDict, rightRingDict, bodyDict, itemDict;

    FAWorld::Item cursor = inventory->getItemAt(FAWorld::Item::eqCURSOR);
    FAWorld::Item head = inventory->getItemAt(FAWorld::Item::eqHEAD);
    FAWorld::Item amulet = inventory->getItemAt(FAWorld::Item::eqAMULET);
    FAWorld::Item leftHand = inventory->getItemAt(FAWorld::Item::eqLEFTHAND);
    FAWorld::Item rightHand = inventory->getItemAt(FAWorld::Item::eqRIGHTHAND);
    FAWorld::Item leftRing = inventory->getItemAt(FAWorld::Item::eqLEFTRING);
    FAWorld::Item rightRing = inventory->getItemAt(FAWorld::Item::eqRIGHTRING);
    FAWorld::Item body = inventory->getItemAt(FAWorld::Item::eqBODY);

    dict["cursor"] = cursorDict;

    cursorDict["graphic"] = cursor.getGraphicValue();
    cursorDict["empty"] = cursor.isEmpty();
    cursorDict["invX"] = cursor.getInvCoords().first;
    cursorDict["invY"] = cursor.getInvCoords().second;
    cursorDict["real"] = cursor.isReal();
    cursorDict["sizeX"] = cursor.getInvSize().first;
    cursorDict["sizeY"] = cursor.getInvSize().second;
    cursorDict["cornerX"] = cursor.getCornerCoords().first;
    cursorDict["cornerY"] = cursor.getCornerCoords().second;

    dict["head"] = headDict;

    headDict["graphic"] = head.getGraphicValue();
    headDict["empty"] = head.isEmpty();
    headDict["invX"] = head.getInvCoords().first;
    headDict["invY"] = head.getInvCoords().second;
    headDict["real"] = head.isReal();
    headDict["sizeX"] = head.getInvSize().first;
    headDict["sizeY"] = head.getInvSize().second;
    headDict["cornerX"] = head.getCornerCoords().first;
    headDict["cornerY"] = head.getCornerCoords().second;

    dict["body"] = bodyDict;

    bodyDict["graphic"] = body.getGraphicValue();
    bodyDict["empty"] = body.isEmpty();
    bodyDict["invX"] = body.getInvCoords().first;
    bodyDict["invY"] = body.getInvCoords().second;
    bodyDict["real"] = body.isReal();
    bodyDict["sizeX"] = body.getInvSize().first;
    bodyDict["sizeY"] = body.getInvSize().second;
    bodyDict["cornerX"] = body.getCornerCoords().first;
    bodyDict["cornerY"] = body.getCornerCoords().second;

    dict["leftHand"] = leftHandDict;

    leftHandDict["graphic"] = leftHand.getGraphicValue();
    leftHandDict["empty"] = leftHand.isEmpty();
    leftHandDict["invX"] = leftHand.getInvCoords().first;
    leftHandDict["invY"] = leftHand.getInvCoords().second;
    leftHandDict["real"] = leftHand.isReal();
    leftHandDict["sizeX"] = leftHand.getInvSize().first;
    leftHandDict["sizeY"] = leftHand.getInvSize().second;
    leftHandDict["cornerX"] = leftHand.getCornerCoords().first;
    leftHandDict["cornerY"] = leftHand.getCornerCoords().second;

    dict["rightHand"] = rightHandDict;

    rightHandDict["graphic"] = rightHand.getGraphicValue();
    rightHandDict["empty"] = rightHand.isEmpty();
    rightHandDict["invX"] = rightHand.getInvCoords().first;
    rightHandDict["invY"] = rightHand.getInvCoords().second;
    rightHandDict["real"] = rightHand.isReal();
    rightHandDict["sizeX"] = rightHand.getInvSize().first;
    rightHandDict["sizeY"] = rightHand.getInvSize().second;
    rightHandDict["cornerX"] = rightHand.getCornerCoords().first;
    rightHandDict["cornerY"] = rightHand.getCornerCoords().second;

    dict["leftRing"] = leftRingDict;

    leftRingDict["graphic"] = leftRing.getGraphicValue();
    leftRingDict["empty"] = leftRing.isEmpty();
    leftRingDict["invX"] = leftRing.getInvCoords().first;
    leftRingDict["invY"] = leftRing.getInvCoords().second;
    leftRingDict["real"] = leftRing.isReal();
    leftRingDict["sizeX"] = leftRing.getInvSize().first;
    leftRingDict["sizeY"] = leftRing.getInvSize().second;
    leftRingDict["cornerX"] = leftRing.getCornerCoords().first;
    leftRingDict["cornerY"] = leftRing.getCornerCoords().second;

    dict["rightRing"] = rightRingDict;

    rightRingDict["graphic"] = rightRing.getGraphicValue();
    rightRingDict["empty"] = rightRing.isEmpty();
    rightRingDict["invX"] = rightRing.getInvCoords().first;
    rightRingDict["invY"] = rightRing.getInvCoords().second;
    rightRingDict["real"] = rightRing.isReal();
    rightRingDict["sizeX"] = rightRing.getInvSize().first;
    rightRingDict["sizeY"] = rightRing.getInvSize().second;
    rightRingDict["cornerX"] = rightRing.getCornerCoords().first;
    rightRingDict["cornerY"] = rightRing.getCornerCoords().second;

    dict["amulet"] = amuletDict;

    amuletDict["graphic"] = amulet.getGraphicValue();
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
            FAWorld::Item item = inventory->getItemAt(FAWorld::Item::eqINV, i, j);
            if(!item.isEmpty())
            {
                itemDict["graphic"] = item.getGraphicValue();
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
    FAWorld::Item beltItem;
    for(uint8_t i=0;i<8;i++)
    {
        boost::python::dict beltDict;
        beltItem = inventory->getItemAt(FAWorld::Item::eqBELT, 0, 0, i);
        if(!beltItem.isEmpty())
        {
            beltDict["graphic"] = beltItem.getGraphicValue();
            beltDict["empty"] = false;
            beltDict["invX"] = beltItem.getInvCoords().first;
            beltDict["invY"] = beltItem.getInvCoords().second;
            beltDict["real"] = beltItem.isReal();
            beltDict["sizeX"] = beltItem.getInvSize().first;
            beltDict["sizeY"] = beltItem.getInvSize().second;
            beltDict["cornerX"] = beltItem.getCornerCoords().first;
            beltDict["cornerY"] = beltItem.getCornerCoords().second;
        }
        else
        {
            beltDict["graphic"] = 0;
            beltDict["empty"] = true;
            beltDict["real"] = false;
            beltDict["invX"] = beltItem.getInvCoords().first;
            beltDict["invY"] = beltItem.getInvCoords().second;
        }
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
    boost::python::def("updateInventory", &updateInventory);
    boost::python::def("placeItem", &placeItem);
    boost::python::def("getInvClass", &getInvClass);
}

Rocket::Core::ElementDocument* ingameUi = NULL;
Rocket::Core::ElementDocument* mainMenu = NULL;

void initGui(FAWorld::Inventory & playerInventory, std::string invclass)
{
    inventory = &playerInventory;
    invClass = invclass;
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
