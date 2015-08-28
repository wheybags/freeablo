#ifndef CONSOLE_H
#define CONSOLE_H

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <queue>
#include <memory>

#include "commandhistory.h"
#include "autocomplete.h"
#include <script/scriptcontext.h>

namespace FAGui
{

class Console : public Rocket::Core::EventListener
{
public:

    static Console& getInstance(Rocket::Core::Context* context);

    void toggle();
    void show();
    void hide();
    bool isVisible() const;

private:
    Console(Rocket::Core::Context*);
    Console(const Console &);
    ~Console() { }

    void ProcessEvent(Rocket::Core::Event& event);
    void submitCommand();
    void insertCommandToHistory(const std::string & value);
    void insertCommandToContainers(const std::string &command);
    void insertCommandToVisibleArea(const std::string& command, bool addNewLine);
    void insertOutputToVisibleArea(const std::string& command);
    void removeVisibleCommandsIfFull();
    void setInput(const std::string & value);
    std::string getInput();
    void setConsoleOutput(const std::string & value);
    std::string getConsoleOutput();


    void showPreviousCommandInInput();
    void showNextCommandInInput();
    void autoComplete();

    static const int MAX_VISIBLE_COMMANDS = 36;

    Rocket::Core::Context* mRocketContext;
    Rocket::Core::ElementDocument* mDoc;
    Rocket::Controls::ElementFormControlInput* mInput;
    std::queue<std::string> mVisibleCommands;
    std::queue<Rocket::Core::Element*> mVisibleCommandsElements;
    CommandHistory mCommandHistory;
    AutoComplete mAutoComplete;
    std::string mLastPrefix;
};

}

#endif
