#include "commandtranslator.h"

#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>

namespace Script
{

using namespace boost;

void CommandTranslator::addTranslation(const std::string & key, const std::string & value)
{
    mTranslationMap[key] = value;
}

std::string CommandTranslator::translate(const std::string & command)
{
    if(command.empty())
        return command;

    char_separator<char> separator(" ");
    tokenizer<char_separator<char>> tokens(command, separator);
    std::string firstToken = *(tokens.begin());

    auto iterator = mTranslationMap.find(firstToken);
    if(iterator == mTranslationMap.end())
        return command;

    std::string args = "";
    for(auto tokenIterator = ++tokens.begin(); tokenIterator != tokens.end() ; tokenIterator++)
    {
        args += *tokenIterator + ",";
    }

    args = args.substr(0, args.size()-1);
    std::string translatedCommand = iterator->second + "(" + args + ")";

    return translatedCommand;
}
}
