#pragma once
#include <memory>
#include <misc/misc.h>
#include <string>
#include <vector>

namespace Settings
{
    using Container = std::vector<std::string>;

    class Settings
    {
    public:
        Settings(const std::string& defaultPath = Misc::getResourcesPath().str() + "/settings-default.ini",
                 const std::string& userPath = Misc::getResourcesPath().str() + "/settings-user.ini");
        ~Settings();
        Settings(const Settings&) = delete;
        Settings& operator=(const Settings&) = delete;

        bool loadUserSettings();
        bool loadFromFile(const std::string& path);
        bool save();

        bool isSectionExists(const std::string& section);
        Container getSections();
        Container getPropertiesInSection(const std::string& section);

        template <class T> T get(const std::string& section, const std::string& name, T defaultValue = T()) const;
        template <class T> void set(const std::string& section, const std::string& name, T value);

    private:
        struct Impl;
        std::unique_ptr<Impl> mImpl;
    };
}
