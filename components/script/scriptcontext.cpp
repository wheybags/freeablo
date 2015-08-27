#include "scriptcontext.h"

namespace Script
{

using namespace boost::python;

ScriptContext::ScriptContext()
{
    Py_Initialize();
    mModule = PyImport_AddModule("__main__");
    PyRun_SimpleString("import freeablo");
}

ScriptContext & ScriptContext::getInstance()
{
    static ScriptContext ctx;
    return ctx;
}

std::string ScriptContext::exec(const std::string& command)
{
    static const std::string forwardPythonStdoutToCPP =
"import sys\n\
class CatchOutErr:\n\
    def __init__(self):\n\
        self.value = ''\n\
    def write(self, txt):\n\
        self.value += txt\n\
catchOutErr = CatchOutErr()\n\
sys.stdout = catchOutErr\n\
sys.stderr = catchOutErr\n\
";

    std::string result = "";
    try
    {
        std::string translatedCommand = mCommandTranslator.translate(command);

        PyRun_SimpleString(forwardPythonStdoutToCPP.c_str());
        PyRun_SimpleString(translatedCommand.c_str());
        PyObject *catcher = PyObject_GetAttrString(mModule,"catchOutErr");
        PyErr_Print();

        PyObject *output = PyObject_GetAttrString(catcher,"value");
        result = PyString_AsString(output);
    }
    catch (error_already_set)
    {
        PyErr_Print();
    }

    return result;
}
}
