#ifndef GUIMANAGER_H
#define GUIMANAGER_H
#include "../faworld/itemmanager.h"
#include <string>
#include <chrono>
#include <queue>
#include <functional>


#include "fapython.h"

namespace Rocket
{
namespace Core
{
class ElementDocument;
}
}

namespace FAGui
{

    class ScrollBox;
    class GuiManager
    {
        public:

        enum GuiType
        {
            TitleScreen,
            MainMenu,
            Credits,
            IngameMenu,
            Other
        };

        GuiManager(FAWorld::Inventory &playerInventory, Engine::EngineMain& engine, std::string invClass);

        void openDialogue(const std::string& document);
        void closeDialogue();
        bool isDialogueOpened() const;
        void openDialogueScrollbox(const std::string& document);
        void closeDialogueScrollbox();
        bool isDialogueScrollboxOpened() const;
        void showTitleScreen();
        void showIngameGui();
        void showMainMenu();
        void showCredits();
        void showSelectHeroMenu(bool fade);
        void showChooseClassMenu();
        void showSaveFileExistsMenu(int classNumber);
        void showEnterNameMenu(int classNumber);
        void showInvalidNameMenu(int classNumber);
        void update(bool paused);
        GuiType currentGuiType() const;
        void setStatusBarText(std::string text);
        static std::string invClass;

        FAPythonFuncs mPythonFuncs;

    private:

        enum State
        {
            FadeIn,
            FadeOut,
        };

        void showMainMenuCallback();
        void showCreditsCallback();
        void showSelectHeroMenuCallback();
        void showSelectHeroMenuNoFadeCallback();

        void startFadeIn(Rocket::Core::ElementDocument *);
        void startFadeOut(std::function<void(GuiManager&)> callback);

        void updateFadeIn();
        void updateFadeOut();

        void stopFadeIn();
        void stopFadeOut();

        void hideFadeElement();
        void showFadeElement();

        void computeFadeDelta();

        void updateGui(bool paused);
        void hideAllMenus();

        Rocket::Core::ElementDocument * mDocument;
        std::function<void(GuiManager&)> mFadeOutCallback;
        float mFadeDelta;
        float mFadeValue;
        Rocket::Core::ElementDocument * mFadeCurrentDocument = nullptr;
        std::queue<State> mStateQueue;
        GuiType mCurrentGuiType;
        std::chrono::system_clock::time_point mStartTime;
        std::shared_ptr<ScrollBox> mCreditsScrollBox;
        std::shared_ptr<ScrollBox> mDialogueScrollBox;
    };

    extern std::string cursorPath;

    extern uint32_t cursorFrame;
}

#endif
