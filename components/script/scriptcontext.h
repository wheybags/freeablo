#ifndef SCRIPTCONTEXT_H
#define SCRIPTCONTEXT_H

#include <misc/boost_python.h>
#include <string>
#include "commandtranslator.h"

namespace Script
{

class ScriptContext
{
public:
    static ScriptContext & getInstance();
    CommandTranslator & getTranslator() { return mCommandTranslator; }
    std::string exec(const std::string& command);

private:
    ScriptContext();

    PyObject * mModule;
    CommandTranslator mCommandTranslator;
};

}
#endif
