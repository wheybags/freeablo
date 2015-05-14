#include "settings.h"

#include <boost/foreach.hpp>
#include <fstream>

namespace Settings
{

using namespace boost::property_tree;

const std::string Settings::DEFAULT_PATH = "resources/settings-default.ini";
const std::string Settings::USER_PATH = "resources/settings-user.ini";
const std::string Settings::USER_DIR = "resources/";

Settings::Settings()
{
}


bool Settings::loadSettings()
{
    // Load defaults

    if(!boost::filesystem::exists(DEFAULT_PATH))
        return false;

    boost::property_tree::ini_parser::read_ini(DEFAULT_PATH, mDefaultsPropertyTree);

    // Load user settings

    if(!boost::filesystem::exists(USER_PATH))
    {
        std::ofstream file(USER_PATH.c_str());
        file << "";
        file.close();

        return true;
    }

    boost::property_tree::ini_parser::read_ini(USER_PATH, mUserPropertyTree);
    return true;
}

template<class T>
T Settings::get(const char* name)
{
    boost::optional< T > child = mUserPropertyTree.get_optional<T>( name );
    if(!child)
    {
        T value;
        try
        {
            value = mDefaultsPropertyTree.get<T>( name );
        }
        catch(...)
        {
            value = T();
        }

        return value;
    }

    return child.get();
}

template<class T>
void Settings::set(const char* name, T value)
{
    mUserPropertyTree.put(name, value);
}

bool Settings::saveSettingsToFile()
{
    boost::property_tree::ini_parser::write_ini(USER_PATH, mUserPropertyTree);
    return true;
}


}
