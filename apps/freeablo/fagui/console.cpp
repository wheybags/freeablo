#include "console.h"

#include <boost/algorithm/string/find.hpp>
#include <algorithm>

namespace FAGui
{

    using namespace std;

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


        vector<pair<string, string> > commands = {
            make_pair("quit","freeablo.quit"),
            make_pair("pause","freeablo.pause"),
            make_pair("unpause","freeablo.unpause"),
            make_pair("save","freeablo.saveGame"),
            make_pair("load","freeablo.loadGame")
        };

        Script::ScriptContext & scriptContext = Script::ScriptContext::getInstance();

        for(unsigned int i = 0; i < commands.size() ; i++)
        {
            scriptContext.getTranslator().addTranslation(commands[i].first, commands[i].second);
            mAutoComplete.insertWord(commands[i].first);
        }
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
            if(key_identifier != Rocket::Core::Input::KI_TAB &&
                key_identifier != Rocket::Core::Input::KI_END &&
                key_identifier != Rocket::Core::Input::KI_HOME &&
                key_identifier != Rocket::Core::Input::KI_LEFT &&
                key_identifier != Rocket::Core::Input::KI_RIGHT)
            {
                mLastPrefix = "";
            }

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
            else if(key_identifier == Rocket::Core::Input::KI_TAB)
            {
                this->autoComplete();
            }
        }
    }

    void Console::submitCommand()
    {
        string command = getInput();

        insertCommandToHistory(command);
        setInput("");

        Script::ScriptContext & scriptContext = Script::ScriptContext::getInstance();
        string result = scriptContext.exec(command);

        if(!result.empty())
            insertOutputToVisibleArea(result);
    }

    void Console::insertCommandToHistory(const string& command)
    {
        insertCommandToContainers(command);
        removeVisibleCommandsIfFull();
    }

    void Console::insertCommandToContainers(const string& command)
    {
        if(command != "")
        {
            mCommandHistory.push(command);
        }

        insertCommandToVisibleArea(command,true);
    }

    void Console::insertCommandToVisibleArea(const string& command, bool addNewLine)
    {
        string oldValue = mDoc->GetElementById("console")->GetAttribute< Rocket::Core::String >("value", "").CString();
        string newValue = string(oldValue) + command + (addNewLine ? "\n" : "");

        setConsoleOutput(newValue);
        mVisibleCommands.push(command);

        removeVisibleCommandsIfFull();
    }

    void Console::insertOutputToVisibleArea(const string &command)
    {
        insertCommandToVisibleArea(command, false);
    }

    void Console::removeVisibleCommandsIfFull()
    {
        string oldValue = getConsoleOutput();
        size_t n = count(oldValue.begin(), oldValue.end(), '\n');
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

            string newValue = string(oldValue.begin() + i, oldValue.end());
            setConsoleOutput(newValue);
        }
    }

    void Console::setInput(const string & value)
    {
        mInput->SetAttribute<const char*>("value", value.c_str());
    }

    string Console::getInput()
    {
        return mInput->GetValue().CString();
    }

    void Console::setConsoleOutput(const string & value)
    {
        mDoc->GetElementById("console")->SetAttribute<const char*>("value", value.c_str());
    }

    string Console::getConsoleOutput()
    {
        return mDoc->GetElementById("console")->GetAttribute< Rocket::Core::String >("value", "").CString();
    }

    void Console::showPreviousCommandInInput()
    {
         string command = mCommandHistory.undo();
         setInput(command);
    }

    void Console::showNextCommandInInput()
    {
        string command = mCommandHistory.redo();
        setInput(command);
    }

    void Console::autoComplete()
    {
        string input = getInput();
        if(mLastPrefix == "")
            mLastPrefix = input;

        input = mAutoComplete.getNextWord(mLastPrefix);
        setInput(input);
    }

}

