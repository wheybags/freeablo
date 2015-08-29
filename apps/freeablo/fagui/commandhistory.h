#ifndef COMMAND_HISTORY_H
#define COMMAND_HISTORY_H

#include <list>
#include <vector>
#include <string>
#include <iterator>

namespace FAGui
{

typedef std::vector<std::string> HistoryCommandsContainer;

class CommandHistory
{
public:
    CommandHistory();

    void push(const std::string & command);
    std::string undo();
    std::string redo();
    void resetToHead();

private:

    HistoryCommandsContainer mCommands;
    int mPosition;
    int mLastSize;
};

}

#endif
