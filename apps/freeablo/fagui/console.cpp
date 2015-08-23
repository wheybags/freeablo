#include "console.h"

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

        for(int i = 0 ; i < MAX_VISIBLE_COMMANDS ; i++)
            insertCommandToHistory("");
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
            insertCommandToVisibleArea(result);
    }

    void Console::insertCommandToHistory(const std::string& command)
    {
        insertCommandToContainers(command);
        removeFirstVisibleCommandIfFull();
    }

    void Console::insertCommandToContainers(const std::string& command)
    {
        if(command != "")
        {
            mCommandHistory.push(command);
        }

        insertCommandToVisibleArea(command);
    }

    void Console::insertCommandToVisibleArea(const std::string& command)
    {
        Rocket::Core::Element * span = mDoc->CreateElement("span");
        Rocket::Core::ElementText * text = mDoc->CreateTextNode(command.c_str());
        span->AppendChild(text);
        mDoc->GetElementById("console")->AppendChild(span);

        mVisibleCommands.push(command);
        mVisibleCommandsElements.push(span);

        removeFirstVisibleCommandIfFull();
    }

    void Console::removeFirstVisibleCommandIfFull()
    {
        if(mVisibleCommands.size() > MAX_VISIBLE_COMMANDS)
        {
            mVisibleCommands.pop();
            mDoc->GetElementById("console")->RemoveChild(mVisibleCommandsElements.front());
            mVisibleCommandsElements.pop();
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

