#include "console.h"

#include <boost/algorithm/string/find.hpp>
#include <algorithm>

namespace FAGui
{

    Console& Console::getInstance(Rocket::Core::Context* context)
    {
        static Console console(context);
        return console;
    }

    Console::Console(Rocket::Core::Context* ctx) : mRocketContext(ctx)
    {
        mDoc = ctx->LoadDocument("resources/gui/console.rml");
        mInput = (Rocket::Controls::ElementFormControlInput*)mDoc->GetElementById("input");
        mInput->AddEventListener("keydown", this);

        Script::ScriptContext & scriptContext = Script::ScriptContext::getInstance();
        scriptContext.getTranslator().addTranslation("quit","freeablo.quit");
        scriptContext.getTranslator().addTranslation("pause","freeablo.pause");
        scriptContext.getTranslator().addTranslation("unpause","freeablo.unpause");
        scriptContext.getTranslator().addTranslation("save","freeablo.saveGame");
        scriptContext.getTranslator().addTranslation("load","freeablo.loadGame");
    }

    void Console::toggle()
    {
        isVisible() ? hide() : show();
    }

    void Console::show()
    {
        mDoc->PullToFront();
        mDoc->Show();
        setInput("");
    }

    void Console::hide()
    {
        mDoc->PushToBack();
        mDoc->Hide();
    }

    bool Console::isVisible() const
    {
        return mDoc->IsVisible();
    }

    void Console::ProcessEvent(Rocket::Core::Event& event)
    {
        bool key_down = event == "keydown";
        Rocket::Core::Input::KeyIdentifier key_identifier = (Rocket::Core::Input::KeyIdentifier) event.GetParameter< int >("key_identifier", 0);
        if (key_down)
        {
            if (key_identifier == Rocket::Core::Input::KI_RETURN)
            {
                this->submitCommand();
            }
            else if(key_identifier == Rocket::Core::Input::KI_UP)
            {
                this->showPreviousCommandInInput();
            }
            else if(key_identifier == Rocket::Core::Input::KI_DOWN)
            {
                this->showNextCommandInInput();
            }
        }
    }

    void Console::submitCommand()
    {
        std::string command = getInput();

        insertCommandToHistory(command);
        setInput("");

        Script::ScriptContext & scriptContext = Script::ScriptContext::getInstance();
        std::string result = scriptContext.exec(command);

        if(!result.empty())
            insertOutputToVisibleArea(result);
    }

    void Console::insertCommandToHistory(const std::string& command)
    {
        insertCommandToContainers(command);
        removeVisibleCommandsIfFull();
    }

    void Console::insertCommandToContainers(const std::string& command)
    {
        if(command != "")
        {
            mCommandHistory.push(command);
        }

        insertCommandToVisibleArea(command,true);
    }

    void Console::insertCommandToVisibleArea(const std::string& command, bool addNewLine)
    {
        std::string oldValue = mDoc->GetElementById("console")->GetAttribute< Rocket::Core::String >("value", "").CString();
        std::string newValue = std::string(oldValue) + command + (addNewLine ? "\n" : "");

        setConsoleOutput(newValue);
        mVisibleCommands.push(command);

        removeVisibleCommandsIfFull();
    }

    void Console::insertOutputToVisibleArea(const std::string &command)
    {
        insertCommandToVisibleArea(command, false);
    }

    void Console::removeVisibleCommandsIfFull()
    {
        std::string oldValue = getConsoleOutput();
        size_t n = std::count(oldValue.begin(), oldValue.end(), '\n');
        size_t nRowsToRemove = n - MAX_VISIBLE_COMMANDS;

        if(n > MAX_VISIBLE_COMMANDS)
        {
            size_t counter = 0;
            size_t i = 0;
            size_t size = oldValue.size();
            while(i < size && counter < nRowsToRemove)
            {
                i++;
                if(oldValue[i] == '\n')
                    counter++;
            }

            std::string newValue = std::string(oldValue.begin() + i, oldValue.end());
            setConsoleOutput(newValue);
        }
    }

    void Console::setInput(const std::string & value)
    {
        mInput->SetAttribute<const char*>("value", value.c_str());
    }

    std::string Console::getInput()
    {
        return mInput->GetValue().CString();
    }

    void Console::setConsoleOutput(const std::string & value)
    {
        mDoc->GetElementById("console")->SetAttribute<const char*>("value", value.c_str());
    }

    std::string Console::getConsoleOutput()
    {
        return mDoc->GetElementById("console")->GetAttribute< Rocket::Core::String >("value", "").CString();
    }

    void Console::showPreviousCommandInInput()
    {
         std::string command = mCommandHistory.undo();
         setInput(command);
    }

    void Console::showNextCommandInInput()
    {
        std::string command = mCommandHistory.redo();
        setInput(command);
    }

}

