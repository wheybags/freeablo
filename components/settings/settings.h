#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace Settings
{

class Settings
{
public:

    static const std::string DEFAULT_PATH;
    static const std::string USER_PATH;
    static const std::string USER_DIR;

    Settings();

    bool loadSettings();
    bool saveSettingsToFile();

    template<class T>
    T get(const char* name);

    template<class T>
    void set(const char* name, T value);

private:

    bool loadFile(const std::string & path, boost::property_tree::ptree & pt);

    std::string mPath;
    boost::property_tree::ptree mDefaultsPropertyTree;
    boost::property_tree::ptree mUserPropertyTree;


};

}

#endif // SETTINGS_H
