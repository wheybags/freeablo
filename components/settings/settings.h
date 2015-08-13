#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace Settings
{

typedef std::vector<std::string> Container;

class Settings
{
public:

    static const std::string DEFAULT_PATH;
    static const std::string USER_PATH;
    static const std::string USER_DIR;

    Settings();
    bool loadUserSettings();
    bool loadFromFile(const std::string & path);
    bool loadFromFAFile(const std::string & path);
    bool save();

    bool isSectionExists(const std::string & section);
    Container getSections();
    Container getPropertiesInSection(const std::string & section);

    template<class T>
    T get(const std::string & section, const std::string & name, T defaultValue = T())
    {
        std::string connector = ".";
        if(section.empty())
            connector = "";

        std::string fullName = section + connector + name;

        boost::optional< T > child = mUserPropertyTree.get_optional<T>( fullName.c_str() );
        if(!child)
        {
            T value;
            try
            {
                value = mDefaultsPropertyTree.get<T>( fullName.c_str() );
            }
            catch(...)
            {
                value = defaultValue;
            }

            return value;
        }

        return child.get();
    }

    template<class T>
    void set(const std::string & section, const std::string & name, T value)
    {
        std::string connector = ".";
        if(section.empty())
            connector = "";

        std::string fullName = section + connector + name;
        mUserPropertyTree.put(fullName.c_str(), value);
    }

private:

    bool loadFile(const std::string & path, boost::property_tree::ptree & pt);
    void createFile(const std::string & path);
    void clearSettings();

    enum Mode
    {
        STANDARD_FILE,
        FA_FILE
    };

    Mode mMode;
    std::string mPath;
    boost::property_tree::ptree mDefaultsPropertyTree;
    boost::property_tree::ptree mUserPropertyTree;
};

}

#endif // SETTINGS_H
