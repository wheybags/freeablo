#include "commandhistory.h"

namespace FAGui
{
CommandHistory::CommandHistory()
{
    mPosition = 0;
    mLastSize = mCommands.size();
}

void CommandHistory::push(const std::string & command)
{
    mCommands.push_back(command);
}

std::string CommandHistory::undo()
{
    int size = (int)mCommands.size();

    if(size == 0)
        return "";

    if(mLastSize != size)
        resetToHead();

    mPosition++;
    if(mPosition > size)
        mPosition--;

    mLastSize = size;
    return mCommands[size - mPosition];
}

std::string CommandHistory::redo()
{
    int size = (int)mCommands.size();

    if(size == 0 )
        return "";

    if(mLastSize != size)
        resetToHead();

    mLastSize = size;

    mPosition--;
    if(mPosition <= 0)
    {
        mPosition = 0;
        return "";
    }


    return mCommands[size - mPosition];
}

void CommandHistory::resetToHead()
{
    mPosition = 0;
}

}
