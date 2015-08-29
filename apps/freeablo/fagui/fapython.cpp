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

    void FAPythonFuncs::startGame()
    {
        auto world = FAWorld::World::get();
        world->setLevel(0);
        auto level = world->getCurrentLevel();
        auto player = world->getPlayer();

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

        hotkeynames.append(Input::getHotkeyName(Engine::quit_key));
        hotkeynames.append(Input::getHotkeyName(Engine::noclip_key));
        hotkeynames.append(Input::getHotkeyName(Engine::changelvlup_key));
        hotkeynames.append(Input::getHotkeyName(Engine::changelvldwn_key));
        return hotkeynames;
    }

    boost::python::list FAPythonFuncs::getHotkeys()
    {
        boost::python::list hotkeys;
        Input::Hotkey pquit_key = Engine::quit_key;
        Input::Hotkey pnoclip_key = Engine::noclip_key;
        Input::Hotkey pchangelvlup_key = Engine::changelvlup_key;
        Input::Hotkey pchangelvldwn_key = Engine::changelvldwn_key;

        pquit_key.key = Input::convertAsciiToRocketKey(Engine::quit_key.key);
        pnoclip_key.key = Input::convertAsciiToRocketKey(Engine::noclip_key.key);
        pchangelvlup_key.key = Input::convertAsciiToRocketKey(Engine::changelvlup_key.key);
        pchangelvldwn_key.key = Input::convertAsciiToRocketKey(Engine::changelvldwn_key.key);

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

        if (function == "quit")
        {
            Engine::quit_key = hotkey;
            Engine::quit_key.save("Quit");
        }
        if (function == "noclip")
        {
            Engine::noclip_key = hotkey;
            Engine::noclip_key.save("Noclip");
        }
        if (function == "changelvlup")
        {
            Engine::changelvlup_key = hotkey;
            Engine::changelvlup_key.save("Changelvlup");
        }
        if (function == "changelvldwn")
        {
            Engine::changelvldwn_key = hotkey;
            Engine::changelvldwn_key.save("Changelvldwn");
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
        Level::Item::equipLoc to   = static_cast<Level::Item::equipLoc>(toPara);
        Level::Item::equipLoc from = static_cast<Level::Item::equipLoc>(fromPara);
        Level::Item item = mPlayerInv.getItemAt(
                    from,
                    fromY,
                    fromX, beltX);
        if(item.isEmpty())
            return;
        if(!item.isReal() && !item.isEmpty())
        {
            item = mPlayerInv.getItemAt(
                        Level::Item::eqINV,
                        item.getCornerCoords().second,
                        item.getCornerCoords().first);

            fromX = item.getInvCoords().first;
            fromY = item.getInvCoords().second;
        }
        if(to == Level::Item::eqCURSOR)
        {

                if(mPlayerInv.getItemAt(Level::Item::eqCURSOR).isEmpty())
                {

                    if(mPlayerInv.putItem(item, Level::Item::eqCURSOR, from, fromY, fromX, beltX))
                    {

                        if(item.mItem.graphicValue > 179)
                        {

                            cursorPath = "";
                            cursorFrame = 0;
                        }
                        else
                        {
                            cursorPath = "data/inv/objcurs.cel";
                            cursorFrame = item.mItem.graphicValue;

                        }
                    }

                }

        }

        else if(to == Level::Item::eqINV)
        {
            item = mPlayerInv.getItemAt(Level::Item::eqCURSOR);
            if(mPlayerInv.putItem(item, to, Level::Item::eqCURSOR, toY, toX))
            {
                cursorPath = "";
                cursorFrame = 0;
            }

        }
        else if(to == Level::Item::eqLEFTHAND || to == Level::Item::eqRIGHTHAND)
        {

            item = mPlayerInv.getItemAt(Level::Item::eqCURSOR);
            if(mPlayerInv.putItem(item, to, Level::Item::eqCURSOR, toY, toX))
            {
                cursorPath = "";
                cursorFrame = 0;
            }

        }
        else if(to == Level::Item::eqLEFTRING || to == Level::Item::eqRIGHTRING)
        {
            item = mPlayerInv.getItemAt(Level::Item::eqCURSOR);
            if (mPlayerInv.putItem(item, to, Level::Item::eqCURSOR, toY, toX))
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

        Level::Item cursor = mPlayerInv.getItemAt(Level::Item::eqCURSOR);
        Level::Item head = mPlayerInv.getItemAt(Level::Item::eqHEAD);
        Level::Item amulet = mPlayerInv.getItemAt(Level::Item::eqAMULET);
        Level::Item leftHand = mPlayerInv.getItemAt(Level::Item::eqLEFTHAND);
        Level::Item rightHand = mPlayerInv.getItemAt(Level::Item::eqRIGHTHAND);
        Level::Item leftRing = mPlayerInv.getItemAt(Level::Item::eqLEFTRING);
        Level::Item rightRing = mPlayerInv.getItemAt(Level::Item::eqRIGHTRING);
        Level::Item body = mPlayerInv.getItemAt(Level::Item::eqBODY);

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
                Level::Item item = mPlayerInv.getItemAt(Level::Item::eqINV, i, j);
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
            beltItem = mPlayerInv.getItemAt(Level::Item::eqBELT, 0, 0, i);
            if(!beltItem.isEmpty())
            {
                beltDict["graphic"] = beltItem.mItem.graphicValue;
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

        boost::python::def("updateInventory", +[](){return funcs->updateInventory();});
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
