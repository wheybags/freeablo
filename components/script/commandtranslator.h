#ifndef COMMANDTRANSLATOR_H
#define COMMANDTRANSLATOR_H

#include <string>
#include <map>

namespace Script
{

class CommandTranslator
{
public:
    CommandTranslator();
    std::string translate(const std::string & command);

private:
    std::map<std::string, std::string> mTranslationMap;
};

}
#endif
