#include <Rocket/Core.h>
#include <input/common.h>
#include "../faworld/world.h"
#include "../fasavegame/savegamemanager.h"
#include "../engine/threadmanager.h"
#include "../engine/enginemain.h"
#include "guimanager.h"
#include "fapython.h"

namespace FAGui
{
    std::string cursorPath;
    uint32_t cursorFrame;

    void FAPythonFuncs::openDialogue(const char* document)
    {
        mGuiManager.openDialogue(document);
    }

    void FAPythonFuncs::closeDialogue()
    {
        mGuiManager.closeDialogue();
    }

    void FAPythonFuncs::openDialogueScrollbox(const char* document)
    {
        mGuiManager.openDialogueScrollbox(document);
    }

    void FAPythonFuncs::closeDialogueScrollbox()
    {
        mGuiManager.closeDialogueScrollbox();
    }

    void FAPythonFuncs::showChooseClassMenu()
    {
        mGuiManager.showChooseClassMenu();
    }

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
    }

    void FAPythonFuncs::showCredits()
    {
        mGuiManager.showCredits();
    }

    void FAPythonFuncs::showSelectHeroMenu()
    {
        mGuiManager.showSelectHeroMenu(true);
    }

    void FAPythonFuncs::showSelectHeroMenuNoFade()
    {
        mGuiManager.showSelectHeroMenu(false);
    }

    void FAPythonFuncs::showEnterNameMenu(int classNumber)
    {
        mGuiManager.showEnterNameMenu(classNumber);
    }

    void FAPythonFuncs::showInvalidNameMenu(int classNumber)
    {
        mGuiManager.showInvalidNameMenu(classNumber);
    }

    void FAPythonFuncs::showSaveFileExistsMenu(int classNumber)
    {
        mGuiManager.showSaveFileExistsMenu(classNumber);
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

    void FAPythonFuncs::stopSound()
    {
        Engine::ThreadManager::get()->stopSound();
    }

    void FAPythonFuncs::playClickButtonSound()
    {
        playSound("sfx/items/titlslct.wav");
    }

    boost::python::list FAPythonFuncs::getHotkeyNames()
    {
        boost::python::list hotkeyNames;

        auto& inputManager = mEngine.inputManager();
        auto hotkeys = inputManager.getHotkeys();

        for(const auto& hotkey : hotkeys)
        {
            hotkeyNames.append(hotkey.name());
        }

        return hotkeyNames;
    }

    boost::python::list FAPythonFuncs::getHotkeys()
    {
        auto& inputManager = mEngine.inputManager();

        boost::python::list pythonHotkeys;
        auto hotkeys = inputManager.getHotkeys();

        for(const auto& hotkey : hotkeys)
        {
            Input::Hotkey pythonHotkey = hotkey;
            pythonHotkey.key = Input::convertAsciiToRocketKey(hotkey.key);
            pythonHotkeys.append(pythonHotkey);
        }

        return pythonHotkeys;
    }

    void FAPythonFuncs::setHotkey(std::string function, boost::python::list pyhotkey)
    {
        Input::Hotkey hotkey;
        hotkey.key = Input::convertRocketKeyToAscii(boost::python::extract<int>(pyhotkey[0]));
        hotkey.shift = boost::python::extract<bool>(pyhotkey[1]);
        hotkey.ctrl = boost::python::extract<bool>(pyhotkey[2]);
        hotkey.alt = boost::python::extract<bool>(pyhotkey[3]);

        auto& inputManager = mEngine.inputManager();

        if (function == "quit")
        {
            inputManager.setHotkey(Engine::QUIT, hotkey);
        }
        if (function == "noclip")
        {
            inputManager.setHotkey(Engine::NOCLIP, hotkey);
        }
        if (function == "changelvlup")
        {
            inputManager.setHotkey(Engine::CHANGE_LEVEL_UP, hotkey);
        }
        if (function == "changelvldwn")
        {
            inputManager.setHotkey(Engine::CHANGE_LEVEL_DOWN, hotkey);
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

// horrible platform speceific bullshit to make boost::python::def accept a lamda as a parameter
// if this makes you throw up in your mouth a little bit and you have a better solution PLEASE FIX THIS
// for some reason, msvc won't handle the code that works for gcc or clang, and vice-versa, so in a moment of desparation I wrote this hideous monstrosity
#ifdef WIN32
}

// crazy magic to make boost shut up and accept std::functions as arguments to boost::python::def http://stackoverflow.com/a/25281985
namespace boost {
    namespace python {
        namespace detail {

            template <class T, class... Args>
            inline boost::mpl::vector<T, Args...> get_signature(std::function<T(Args...)>, void* = 0)
            {
                return boost::mpl::vector<T, Args...>();
            }
        }
    }
}

namespace FAGui
{


// horrible macro for a horrible task - turns lambdas into std::functions and passes em to boost::python::def
// eg: 
//        DEF_FUNC("openDialogue", void, (const char* document), { funcs->openDialogue(document); });
//        expands to:
//        boost::python::def("openDialogue", std::function<void (const char* document)>([] (const char* document) { funcs->openDialogue(document); } ));
//
// why not use http://stackoverflow.com/a/30791447 ? because the +[] syntax generates a syntax error on msvc... (???)
#define DEF_FUNC(name, ret_type, params, code) boost::python::def(name, std::function<ret_type params>([] params code ))

#else // WIN32

#define DEF_FUNC(name, ret_type, params, code) boost::python::def(name, +[] params code)

#endif // WIN32


    FAPythonFuncs* funcs = NULL;
    BOOST_PYTHON_MODULE(freeablo)
    {

        DEF_FUNC("openDialogue",                void,                   (const char* document),                                 { funcs->openDialogue(document); }              );
        DEF_FUNC("closeDialogue",               void,                   (void),                                                 { funcs->closeDialogue(); }                     );
        DEF_FUNC("openDialogueScrollbox",       void,                   (const char* document),                                 { funcs->openDialogueScrollbox(document); }     );
        DEF_FUNC("closeDialogueScrollbox",      void,                   (void),                                                 { funcs->closeDialogueScrollbox(); }            );
        DEF_FUNC("showMainMenu",                void,                   (void),                                                 { funcs->showMainMenu(); }                      );
        DEF_FUNC("showCredits",                 void,                   (void),                                                 { funcs->showCredits(); }                       );
        DEF_FUNC("showSelectHeroMenu",          void,                   (void),                                                 { funcs->showSelectHeroMenu(); }                );
        DEF_FUNC("showSelectHeroMenuNoFade",    void,                   (void),                                                 { funcs->showSelectHeroMenuNoFade(); }          );
        DEF_FUNC("showChooseClassMenu",         void,                   (void),                                                 { funcs->showChooseClassMenu(); }               );
        DEF_FUNC("showEnterNameMenu",           void,                   (int classNumber),                                      { funcs->showEnterNameMenu(classNumber); }      );
        DEF_FUNC("showInvalidNameMenu",         void,                   (int classNumber),                                      { funcs->showInvalidNameMenu(classNumber); }    );
        DEF_FUNC("showSaveFileExistsMenu",      void,                   (int classNumber),                                      { funcs->showSaveFileExistsMenu(classNumber); } );
        DEF_FUNC("quit",                        void,                   (void),                                                 { funcs->quitGame(); }                          );
        DEF_FUNC("pause",                       void,                   (void),                                                 { funcs->pauseGame(); }                         );
        DEF_FUNC("unpause",                     void,                   (void),                                                 { funcs->unpauseGame(); }                       );
        DEF_FUNC("startGame",                   void,                   (void),                                                 { funcs->startGame(); }                         );
        DEF_FUNC("playSound",                   void,                   (const std::string& path),                              { funcs->playSound(path); }                     );
        DEF_FUNC("stopSound",                   void,                   (void),                                                 { funcs->stopSound(); }                         );
        DEF_FUNC("playClickButtonSound",        void,                   (void),                                                 { funcs->playClickButtonSound(); }              );
        DEF_FUNC("getHotkeyNames",              boost::python::list,    (void),                                                 { return funcs->getHotkeyNames(); }             );
        DEF_FUNC("getHotkeys",                  boost::python::list,    (void),                                                 { return funcs->getHotkeys(); }                 );
        DEF_FUNC("setHotkey",                   void,                   (std::string function, boost::python::list pyhotkey),   { funcs->setHotkey(function, pyhotkey); }       );
        DEF_FUNC("saveGame",                    void,                   (void),                                                 { funcs->saveGame(); }                          );
        DEF_FUNC("loadGame",                    void,                   (void),                                                 { funcs->loadGame(); }                          );
        DEF_FUNC("getInvClass",                 std::string,            (void),                                                 { return funcs->getInvClass(); }                );
        DEF_FUNC("updateInventory",             boost::python::dict,    (void),                                                 { return funcs->updateInventory(); }            );
        
        // these handled separately because they're too long to align
        DEF_FUNC("canPlaceItem",    bool, (uint32_t toPara, uint32_t fromPara, uint32_t fromY, uint32_t fromX, uint32_t toY, uint32_t toX, uint32_t beltX), 
            { return funcs->canPlace(toPara, fromPara, fromY, fromX, toY, toX, beltX); }    );

        DEF_FUNC("placeItem",       void, (uint32_t toPara, uint32_t fromPara, uint32_t fromY, uint32_t fromX, uint32_t toY, uint32_t toX, uint32_t beltX),
            { funcs->placeItem(toPara, fromPara, fromY, fromX, toY, toX, beltX); }          );
    }

    void initializePython(FAPythonFuncs& _funcs)
    {
        funcs = &_funcs;
        initfreeablo();
        Input::Hotkey::initializePythonWrapper();
    }
}
