#include <fstream>
#include <iostream>
#include <faio/faio.h>
#include "settings.h"

namespace Settings
{

using namespace boost::property_tree;

const std::string Settings::DEFAULT_PATH = "resources/settings-default.ini";
const std::string Settings::USER_PATH = "resources/settings-user.ini";
const std::string Settings::USER_DIR = "resources/";

Settings::Settings()
    : mMode(STANDARD_FILE)
{
}


bool Settings::loadUserSettings()
{    
    clearSettings();

    mMode = STANDARD_FILE;

    // Load defaults

    if(!boost::filesystem::exists(DEFAULT_PATH))
        return false;

    boost::property_tree::ini_parser::read_ini(DEFAULT_PATH, mDefaultsPropertyTree);

    // Load user settings

    mPath = USER_PATH;

    if(!boost::filesystem::exists(USER_PATH))
    {
        createFile(USER_PATH);
        return true;
    }

    try
    {
        boost::property_tree::ini_parser::read_ini(mPath, mUserPropertyTree);
    }
    catch(std::exception & e)
    {
        std::cout << "Loading INI exception: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool Settings::loadFromFile(const std::string & path)
{
    clearSettings();

    mMode = STANDARD_FILE;

    if(!boost::filesystem::exists(path))
    {
        std::cout << "Settings file \"" << path << "\" does not exists. Creating file..." << std::endl;
        createFile(path);
    }

    mPath = path;

    try
    {
        boost::property_tree::ini_parser::read_ini(path, mUserPropertyTree);
    }
    catch(std::exception & e)
    {
        std::cout << "Loading INI exception: " << e.what() << std::endl;
        return false;
    }

    return true;
}

void Settings::createFile(const std::string & path)
{
    std::ofstream file(path.c_str());
    file << "";
    file.close();
}

bool Settings::save()
{
    if(mMode == FA_FILE)
    {
        std::cout << "Cannot save FAFile." << std::endl;
        return false;
    }

    if(mPath.empty())
    {
        std::cout << "Settings file is not set." << std::endl;
        return false;
    }

    boost::property_tree::ini_parser::write_ini(mPath, mUserPropertyTree);
    return true;
}

void Settings::clearSettings()
{
    mUserPropertyTree.clear();
    mDefaultsPropertyTree.clear();
}

Container Settings::getSections()
{
    Container sections;
    const boost::property_tree::ptree & pt = mUserPropertyTree;

    for(boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); ++it)
    {
        sections.push_back(it->first);
    }

    return sections;
}

Container Settings::getPropertiesInSection(const std::string & section)
{
    Container properties;
    const boost::property_tree::ptree & pt = mUserPropertyTree;
    boost::property_tree::ptree::const_assoc_iterator assocIterator = pt.find(section);

    if(assocIterator != pt.not_found())
    {
        for(boost::property_tree::ptree::const_iterator key = assocIterator->second.begin(); key != assocIterator->second.end(); ++key)
        {
            properties.push_back(key->first);
        }
    }

    return properties;
}

bool Settings::isSectionExists(const std::string & section)
{
    const boost::property_tree::ptree & pt = mUserPropertyTree;
    boost::property_tree::ptree::const_assoc_iterator assocIterator = pt.find(section);
    return assocIterator != pt.not_found();
}

}
