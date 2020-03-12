#pragma once
#include <iterator>
#include <list>
#include <string>
#include <vector>

namespace FAGui
{

    typedef std::vector<std::string> HistoryCommandsContainer;

    class CommandHistory
    {
    public:
        CommandHistory();

        void push(const std::string& command);
        std::string undo();
        std::string redo();
        void resetToHead();

    private:
        HistoryCommandsContainer mCommands;
        int mPosition;
        int mLastSize;
    };
}
