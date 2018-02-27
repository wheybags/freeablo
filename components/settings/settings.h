
#pragma once

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace Settings
{

    typedef std::vector<std::string> Container;

    // adding custom translator to allow reading hex values
    template <typename E> class stream_translator
    {
        typedef boost::property_tree::customize_stream<char, std::char_traits<char>, E> customized;

    public:
        typedef E external_type;

        explicit stream_translator(std::locale loc = std::locale()) : m_loc(loc) {}

        boost::optional<E> get_value(const std::string& v)
        {
            auto v_cpy = v;
            boost::trim(v_cpy);
            std::istringstream iss(v);
            iss.imbue(m_loc);
            if (boost::starts_with(v_cpy, "0x"))
            {
                iss.ignore(2);
                iss >> std::hex;
            }
            E e;
            customized::extract(iss, e);
            if (iss.fail() || iss.bad() || iss.get() != std::char_traits<char>::eof())
            {
                return boost::optional<E>();
            }
            return e;
        }
        boost::optional<std::string> put_value(const E& v)
        {
            std::ostringstream oss;
            oss.imbue(m_loc);
            customized::insert(oss, v);
            if (oss)
            {
                return oss.str();
            }
            return {};
        }

    private:
        std::locale m_loc;
    };

    template <> inline boost::optional<std::string> stream_translator<std::string>::get_value(const std::string& v) { return v; }

    class Settings
    {
    public:
        static const std::string DEFAULT_PATH;
        static const std::string USER_PATH;
        static const std::string USER_DIR;

        Settings();
        bool loadUserSettings();
        bool loadFromFile(const std::string& path);
        bool save();

        bool isSectionExists(const std::string& section);
        Container getSections();
        Container getPropertiesInSection(const std::string& section);

        template <class T> T get(const std::string& section, const std::string& name, T defaultValue = T())
        {
            std::string connector = ".";
            if (section.empty())
                connector = "";

            std::string fullName = section + connector + name;

            boost::optional<T> child = mUserPropertyTree.get_optional<T>(fullName.c_str(), stream_translator<T>{});
            if (!child)
            {
                T value;
                try
                {
                    value = mDefaultsPropertyTree.get<T>(fullName.c_str());
                }
                catch (...)
                {
                    value = defaultValue;
                }

                return value;
            }

            return child.get();
        }

        template <class T> void set(const std::string& section, const std::string& name, T value)
        {
            std::string connector = ".";
            if (section.empty())
                connector = "";

            std::string fullName = section + connector + name;
            mUserPropertyTree.put(fullName.c_str(), value);
        }

    private:
        bool loadFile(const std::string& path, boost::property_tree::ptree& pt);
        void createFile(const std::string& path);
        void clearSettings();

        enum class Mode
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
