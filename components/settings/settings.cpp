#include "settings.h"
#include <SimpleIni.h>
#include <cstdint>
#include <faio/faio.h>
#include <filesystem/path.h>
#include <fstream>
#include <iostream>

namespace Settings
{
    enum class Mode
    {
        STANDARD_FILE,
        FA_FILE
    };

    struct Settings::Impl
    {
        Impl(const std::string& defaultPath, const std::string& userPath);

        void createFile(const std::string& path);
        void clearSettings();
        bool isKeyExists(const std::string& section, const std::string& key);

        const std::string mDefaultPath;
        const std::string mUserPath;
        Mode mMode;
        std::string mPath;
        CSimpleIniCaseA mDefaultsIni;
        CSimpleIniCaseA mUserIni;
    };

    Settings::Impl::Impl(const std::string& defaultPath, const std::string& userPath)
        : mDefaultPath(defaultPath), mUserPath(userPath), mMode(Mode::STANDARD_FILE)
    {
    }

    Settings::Settings(const std::string& defaultPath, const std::string& userPath) : mImpl(new Settings::Impl(defaultPath, userPath)) {}

    Settings::~Settings() = default;

    bool Settings::loadUserSettings()
    {
        mImpl->clearSettings();

        mImpl->mMode = Mode::STANDARD_FILE;

        // Load defaults

        if (!filesystem::exists(mImpl->mDefaultPath))
            return false;

        mImpl->mDefaultsIni.LoadFile(mImpl->mDefaultPath.c_str());

        // Load user settings

        mImpl->mPath = mImpl->mUserPath;

        if (!filesystem::exists(mImpl->mUserPath))
        {
            mImpl->createFile(mImpl->mUserPath);
            return true;
        }

        SI_Error success = mImpl->mUserIni.LoadFile(mImpl->mPath.c_str());
        if (success < 0)
        {
            std::cout << "Error loading INI file" << std::endl;
            return false;
        }

        return true;
    }

    bool Settings::loadFromFile(const std::string& path)
    {
        mImpl->clearSettings();

        mImpl->mMode = Mode::STANDARD_FILE;

        if (!filesystem::exists(path))
        {
            std::cout << "Settings file \"" << path << "\" does not exists. Creating file..." << std::endl;
            mImpl->createFile(path);
        }

        mImpl->mPath = path;

        SI_Error success = mImpl->mUserIni.LoadFile(mImpl->mPath.c_str());
        if (success < 0)
        {
            std::cout << "Error loading INI file" << std::endl;
            return false;
        }

        return true;
    }

    bool Settings::save()
    {
        if (mImpl->mMode == Mode::FA_FILE)
        {
            std::cout << "Cannot save FAFile." << std::endl;
            return false;
        }

        if (mImpl->mPath.empty())
        {
            std::cout << "Settings file is not set." << std::endl;
            return false;
        }

        return mImpl->mUserIni.SaveFile(mImpl->mPath.c_str());
    }

    Container Settings::getSections()
    {
        CSimpleIniCaseA::TNamesDepend loadedSections;
        mImpl->mUserIni.GetAllSections(loadedSections);

        Container sectionContainer;
        for (const CSimpleIniCaseA::Entry& section : loadedSections)
        {
            sectionContainer.push_back(section.pItem);
        }

        return sectionContainer;
    }

    Container Settings::getPropertiesInSection(const std::string& section)
    {
        CSimpleIniCaseA::TNamesDepend loadedKeys;
        mImpl->mUserIni.GetAllKeys(section.c_str(), loadedKeys);

        Container properties;
        for (const CSimpleIniCaseA::Entry& key : loadedKeys)
        {
            properties.push_back(key.pItem);
        }
        return properties;
    }

    bool Settings::isSectionExists(const std::string& section)
    {
        const int sectionSize = mImpl->mUserIni.GetSectionSize(section.c_str());
        return -1 < sectionSize;
    }

    template <> std::string Settings::get<std::string>(const std::string& section, const std::string& name, std::string defaultValue) const
    {
        if (!mImpl->isKeyExists(section, name))
        {
            return mImpl->mDefaultsIni.GetValue(section.c_str(), name.c_str(), defaultValue.c_str());
        }

        return mImpl->mUserIni.GetValue(section.c_str(), name.c_str(), defaultValue.c_str());
    }

    // Why not just use int64_t?
    // Well... on some platforms (*cough* osx) size_t and uint64_t are both 64 bit, but one is a long, and the other a long long, so their types
    // don't match for a template. This is a pain in the ass, so in here we just explicitly instantiate for both long long and long, so it should
    // work either way. We don't assert the size of long, because on 32 bit machines it's actually still 4 bytes.
    static_assert(sizeof(long long int) == 8, "");
    template <> long long int Settings::get<long long int>(const std::string& section, const std::string& name, long long int defaultValue) const
    {
        /// Do not rely on CSimpleIniCaseA::GetLongValue(), as it has limited range.
        /// Hexadecimal numbers are auto-detected by std::stoll().
        const std::string textValue = get<std::string>(section, name, std::to_string(defaultValue));
        return std::stoll(textValue, nullptr, 0);
    }

    template <> long int Settings::get<long int>(const std::string& section, const std::string& name, long int defaultValue) const
    {
        return get<long long int>(section, name, defaultValue);
    }

    template <> int32_t Settings::get<int32_t>(const std::string& section, const std::string& name, int32_t defaultValue) const
    {
        return get<int64_t>(section, name, defaultValue);
    }

    template <> int16_t Settings::get<int16_t>(const std::string& section, const std::string& name, int16_t defaultValue) const
    {
        return get<int64_t>(section, name, defaultValue);
    }

    template <> int8_t Settings::get<int8_t>(const std::string& section, const std::string& name, int8_t defaultValue) const
    {
        return get<int64_t>(section, name, defaultValue);
    }

    static_assert(sizeof(long long unsigned int) == 8, "");
    template <>
    long long unsigned int Settings::get<long long unsigned int>(const std::string& section, const std::string& name, long long unsigned int defaultValue) const
    {
        const std::string textValue = get<std::string>(section, name, std::to_string(defaultValue));
        return std::stoull(textValue, nullptr, 0);
    }

    template <> long unsigned int Settings::get<long unsigned int>(const std::string& section, const std::string& name, long unsigned int defaultValue) const
    {
        return get<long long unsigned int>(section, name, defaultValue);
    }

    template <> uint32_t Settings::get<uint32_t>(const std::string& section, const std::string& name, uint32_t defaultValue) const
    {
        return get<uint64_t>(section, name, defaultValue);
    }

    template <> uint16_t Settings::get<uint16_t>(const std::string& section, const std::string& name, uint16_t defaultValue) const
    {
        return get<uint64_t>(section, name, defaultValue);
    }

    template <> uint8_t Settings::get<uint8_t>(const std::string& section, const std::string& name, uint8_t defaultValue) const
    {
        return get<uint64_t>(section, name, defaultValue);
    }

    template <> long double Settings::get<long double>(const std::string& section, const std::string& name, long double defaultValue) const
    {
        /// Do not rely on CSimpleIniCaseA::GetDoubleValue(), as it has limited range.
        /// Hexadecimal numbers are auto-detected by std::stold().
        const std::string textValue = get<std::string>(section, name, std::to_string(defaultValue));
        return std::stold(textValue, nullptr);
    }

    template <> double Settings::get<double>(const std::string& section, const std::string& name, double defaultValue) const
    {
        return get<long double>(section, name, defaultValue);
    }

    template <> float Settings::get<float>(const std::string& section, const std::string& name, float defaultValue) const
    {
        return get<double>(section, name, defaultValue);
    }

    template <> bool Settings::get<bool>(const std::string& section, const std::string& name, bool defaultValue) const
    {
        if (!mImpl->isKeyExists(section, name))
        {
            return mImpl->mDefaultsIni.GetBoolValue(section.c_str(), name.c_str(), defaultValue);
        }

        return mImpl->mUserIni.GetBoolValue(section.c_str(), name.c_str(), defaultValue);
    }

    template <> void Settings::set<std::string>(const std::string& section, const std::string& name, std::string value)
    {
        mImpl->mUserIni.SetValue(section.c_str(), name.c_str(), value.c_str());
    }

    template <> void Settings::set<int64_t>(const std::string& section, const std::string& name, int64_t value)
    {
        set<std::string>(section, name, std::to_string(value));
    }

    template <> void Settings::set<int32_t>(const std::string& section, const std::string& name, int32_t value) { set<int64_t>(section, name, value); }

    template <> void Settings::set<int16_t>(const std::string& section, const std::string& name, int16_t value) { set<int64_t>(section, name, value); }

    template <> void Settings::set<int8_t>(const std::string& section, const std::string& name, int8_t value) { set<int64_t>(section, name, value); }

    template <> void Settings::set<uint64_t>(const std::string& section, const std::string& name, uint64_t value)
    {
        set<std::string>(section, name, std::to_string(value));
    }

    template <> void Settings::set<uint32_t>(const std::string& section, const std::string& name, uint32_t value) { set<uint64_t>(section, name, value); }

    template <> void Settings::set<uint16_t>(const std::string& section, const std::string& name, uint16_t value) { set<uint64_t>(section, name, value); }

    template <> void Settings::set<uint8_t>(const std::string& section, const std::string& name, uint8_t value) { set<uint64_t>(section, name, value); }

    template <> void Settings::set<long double>(const std::string& section, const std::string& name, long double value)
    {
        set<std::string>(section, name, std::to_string(value));
    }

    template <> void Settings::set<double>(const std::string& section, const std::string& name, double value) { set<long double>(section, name, value); }

    template <> void Settings::set<float>(const std::string& section, const std::string& name, float value) { set<long double>(section, name, value); }

    template <> void Settings::set<bool>(const std::string& section, const std::string& name, bool value)
    {
        mImpl->mUserIni.SetBoolValue(section.c_str(), name.c_str(), value);
    }

    void Settings::Impl::createFile(const std::string& path)
    {
        std::ofstream file(path);
        file << "";
    }

    void Settings::Impl::clearSettings()
    {
        mUserIni.Reset();
        mDefaultsIni.Reset();
    }

    bool Settings::Impl::isKeyExists(const std::string& section, const std::string& key)
    {
        CSimpleIniCaseA::TNamesDepend keys;
        const bool hasKey = mUserIni.GetAllValues(section.c_str(), key.c_str(), keys);
        return hasKey;
    }
}
