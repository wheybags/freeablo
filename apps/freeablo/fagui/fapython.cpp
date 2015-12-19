#include "fapython.h"

#include <Rocket/Core.h>

#include <input/common.h>

#include "../engine/input.h"
#include "../faworld/world.h"
#include "../fasavegame/savegamemanager.h"
#include "../engine/threadmanager.h"
#include "../engine/enginemain.h"
#include "guimanager.h"

namespace FAGui
{
    std::string cursorPath;
    uint32_t cursorFrame;

    void FAPythonFuncs::quitGame()
    {
        mEngine.stop();
    }

    void FAPythonFuncs::pauseGame()
    {
        mEngine.pause();
    }

    void FAPythonFuncs::unpauseGame()
    {
        mEngine.unPause();
    }

    bool FAPythonFuncs::canPlace(uint32_t toPara, uint32_t fromPara, uint32_t fromY, uint32_t fromX, uint32_t toY, uint32_t toX, uint32_t beltX)
    {
        FAWorld::Item::equipLoc to   = static_cast<FAWorld::Item::equipLoc>(toPara);
        FAWorld::Item::equipLoc from = static_cast<FAWorld::Item::equipLoc>(fromPara);

        FAWorld::Item item = mPlayerInv.getItemAt(from, fromY, fromX, beltX);
        return mPlayerInv.canPlaceItem(item, to, toY, toX, beltX);
    }

    std::string FAPythonFuncs::getInvClass()
    {
        return FAGui::GuiManager::invClass;
    }

    void FAPythonFuncs::showMainMenu()
    {
        mGuiManager.showMainMenu();
        Engine::ThreadManager::get()->playMusic("music/dintro.wav");
    }

    void FAPythonFuncs::startGame()
    {
        auto world = FAWorld::World::get();
        world->setLevel(0);
        auto level = world->getCurrentLevel();
        auto player = world->getCurrentPlayer();

        player->mPos = FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second);

        mEngine.unPause();
        mGuiManager.showIngameGui();
    }

    void FAPythonFuncs::saveGame()
    {
        FASaveGame::SaveGameManager manager(FAWorld::World::get());
        manager.save();
    }

    void FAPythonFuncs::loadGame()
    {
         FASaveGame::SaveGameManager manager(FAWorld::World::get());
         manager.load();
    }

    void FAPythonFuncs::playSound(const std::string& path)
    {
        Engine::ThreadManager::get()->playSound(path);
    }

    boost::python::list FAPythonFuncs::getHotkeyNames()
    {
        boost::python::list hotkeynames;

        Engine::EngineInputManager* inputManager = mEngine.getInputManager();

        hotkeynames.append(Input::getHotkeyName(inputManager->quit_key));
        hotkeynames.append(Input::getHotkeyName(inputManager->noclip_key));
        hotkeynames.append(Input::getHotkeyName(inputManager->changelvlup_key));
        hotkeynames.append(Input::getHotkeyName(inputManager->changelvldwn_key));
        return hotkeynames;
    }

    boost::python::list FAPythonFuncs::getHotkeys()
    {
        auto inputManager = mEngine.getInputManager();

        boost::python::list hotkeys;
        Input::Hotkey pquit_key = inputManager->quit_key;
        Input::Hotkey pnoclip_key = inputManager->noclip_key;
        Input::Hotkey pchangelvlup_key = inputManager->changelvlup_key;
        Input::Hotkey pchangelvldwn_key = inputManager->changelvldwn_key;

        pquit_key.key = Input::convertAsciiToRocketKey(inputManager->quit_key.key);
        pnoclip_key.key = Input::convertAsciiToRocketKey(inputManager->noclip_key.key);
        pchangelvlup_key.key = Input::convertAsciiToRocketKey(inputManager->changelvlup_key.key);
        pchangelvldwn_key.key = Input::convertAsciiToRocketKey(inputManager->changelvldwn_key.key);

        hotkeys.append(pquit_key);
        hotkeys.append(pnoclip_key);
        hotkeys.append(pchangelvlup_key);
        hotkeys.append(pchangelvldwn_key);

        return hotkeys;
    }

    void FAPythonFuncs::setHotkey(std::string function, boost::python::list pyhotkey)
    {
        Input::Hotkey hotkey;
        hotkey.key = Input::convertRocketKeyToAscii(boost::python::extract<int>(pyhotkey[0]));
        hotkey.shift = boost::python::extract<bool>(pyhotkey[1]);
        hotkey.ctrl = boost::python::extract<bool>(pyhotkey[2]);
        hotkey.alt = boost::python::extract<bool>(pyhotkey[3]);

        auto inputManager = mEngine.getInputManager();

        if (function == "quit")
        {
            inputManager->quit_key = hotkey;
            inputManager->quit_key.save("Quit");
        }
        if (function == "noclip")
        {
            inputManager->noclip_key = hotkey;
            inputManager->noclip_key.save("Noclip");
        }
        if (function == "changelvlup")
        {
            inputManager->changelvlup_key = hotkey;
            inputManager->changelvlup_key.save("Changelvlup");
        }
        if (function == "changelvldwn")
        {
            inputManager->changelvldwn_key = hotkey;
            inputManager->changelvldwn_key.save("Changelvldwn");
        }
    }


    void FAPythonFuncs::placeItem(uint32_t toPara,
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
        FAWorld::Item item = mPlayerInv.getItemAt(
                    from,
                    fromY,
                    fromX, beltX);
        if(item.isEmpty())
            return;
        if(!item.isReal() && !item.isEmpty())
        {
            item = mPlayerInv.getItemAt(
                        FAWorld::Item::eqINV,
                        item.getCornerCoords().second,
                        item.getCornerCoords().first);

            fromX = item.getInvCoords().first;
            fromY = item.getInvCoords().second;
        }
        if(to == FAWorld::Item::eqCURSOR)
        {

                if(mPlayerInv.getItemAt(FAWorld::Item::eqCURSOR).isEmpty())
                {

                    if(mPlayerInv.putItem(item, FAWorld::Item::eqCURSOR, from, fromY, fromX, beltX))
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
            item = mPlayerInv.getItemAt(FAWorld::Item::eqCURSOR);
            if(mPlayerInv.putItem(item, to, FAWorld::Item::eqCURSOR, toY, toX))
            {
                cursorPath = "";
                cursorFrame = 0;
            }

        }
        else if(to == FAWorld::Item::eqLEFTHAND || to == FAWorld::Item::eqRIGHTHAND)
        {

            item = mPlayerInv.getItemAt(FAWorld::Item::eqCURSOR);
            if(mPlayerInv.putItem(item, to, FAWorld::Item::eqCURSOR, toY, toX))
            {
                cursorPath = "";
                cursorFrame = 0;
            }

        }
        else if(to == FAWorld::Item::eqLEFTRING || to == FAWorld::Item::eqRIGHTRING)
        {
            item = mPlayerInv.getItemAt(FAWorld::Item::eqCURSOR);
            if (mPlayerInv.putItem(item, to, FAWorld::Item::eqCURSOR, toY, toX))
            {
                cursorPath = "";
                cursorFrame = 0;
            }

        }
        else
        {

            item = mPlayerInv.getItemAt(from, fromY, fromX);
            if(mPlayerInv.putItem(item, to, from, toY, toX, beltX))
            {
                cursorPath = "";
                cursorFrame = 0;
            }

        }
        return;
    }

    boost::python::dict FAPythonFuncs::updateInventory()
    {
        boost::python::dict dict, cursorDict, headDict, amuletDict, leftHandDict, rightHandDict, leftRingDict, rightRingDict, bodyDict, itemDict;

        FAWorld::Item cursor = mPlayerInv.getItemAt(FAWorld::Item::eqCURSOR);
        FAWorld::Item head = mPlayerInv.getItemAt(FAWorld::Item::eqHEAD);
        FAWorld::Item amulet = mPlayerInv.getItemAt(FAWorld::Item::eqAMULET);
        FAWorld::Item leftHand = mPlayerInv.getItemAt(FAWorld::Item::eqLEFTHAND);
        FAWorld::Item rightHand = mPlayerInv.getItemAt(FAWorld::Item::eqRIGHTHAND);
        FAWorld::Item leftRing = mPlayerInv.getItemAt(FAWorld::Item::eqLEFTRING);
        FAWorld::Item rightRing = mPlayerInv.getItemAt(FAWorld::Item::eqRIGHTRING);
        FAWorld::Item body = mPlayerInv.getItemAt(FAWorld::Item::eqBODY);

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
                FAWorld::Item item = mPlayerInv.getItemAt(FAWorld::Item::eqINV, i, j);
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
            beltItem = mPlayerInv.getItemAt(FAWorld::Item::eqBELT, 0, 0, i);
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

    FAPythonFuncs* funcs = NULL;
    BOOST_PYTHON_MODULE(freeablo)
    {
        boost::python::def("showMainMenu", +[](){funcs->showMainMenu();});
        boost::python::def("quit", +[](){funcs->quitGame();});
        boost::python::def("pause", +[](){funcs->pauseGame();});
        boost::python::def("unpause", +[](){funcs->unpauseGame();});
        boost::python::def("startGame", +[](){funcs->startGame();});
        boost::python::def("playSound", +[](const std::string& path){funcs->playSound(path);});
        boost::python::def("getHotkeyNames", +[](){return funcs->getHotkeyNames();});
        boost::python::def("getHotkeys", +[](){return funcs->getHotkeys();});
        boost::python::def("setHotkey", +[](std::string function, boost::python::list pyhotkey){funcs->setHotkey(function, pyhotkey);});
        boost::python::def("saveGame", +[](){funcs->saveGame();});
        boost::python::def("loadGame", +[](){funcs->loadGame();});
        boost::python::def("getInvClass", +[](){return funcs->getInvClass();});
        boost::python::def("updateInventory", +[](){return funcs->updateInventory();});
        boost::python::def("canPlaceItem", +[](uint32_t toPara, uint32_t fromPara, uint32_t fromY,
                           uint32_t fromX, uint32_t toY, uint32_t toX, uint32_t beltX){return funcs->canPlace(toPara, fromPara, fromY, fromX, toY, toX, beltX);});
        boost::python::def("placeItem", +[](uint32_t toPara, uint32_t fromPara, uint32_t fromY,
                                            uint32_t fromX, uint32_t toY, uint32_t toX, uint32_t beltX)
                                            { funcs->placeItem(toPara, fromPara, fromY, fromX, toY, toX, beltX); });
    }

    void initPython(FAPythonFuncs& _funcs)
    {
        funcs = &_funcs;
        initfreeablo();
    }
}
