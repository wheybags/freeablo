/*
    path.h -- A simple class for manipulating paths on Linux/Windows/Mac OS

    Copyright (c) 2015 Wenzel Jakob <wenzel@inf.ethz.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "fwd.h"
#include <algorithm>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>

NAMESPACE_BEGIN(filesystem)

/**
 * \brief Simple class for manipulating paths on Linux/Windows/Mac OS
 *
 * This class is just a temporary workaround to avoid the heavy boost
 * dependency until boost::filesystem is integrated into the standard template
 * library at some point in the future.
 */
class path {
public:
    enum path_type {
        windows_path = 0,
        posix_path = 1,
#if defined(_WIN32)
        native_path = windows_path
#else
        native_path = posix_path
#endif
    };

    path() : m_type(native_path), m_absolute(false) { }

    path(const path &path)
        : m_type(path.m_type), m_path(path.m_path), m_absolute(path.m_absolute) {}

    path(path &&path)
        : m_type(path.m_type), m_path(std::move(path.m_path)),
          m_absolute(path.m_absolute) {}

    path(const char *string) { set(string); }

    path(const std::string &string) { set(string); }

#if defined(_WIN32)
    path(const std::wstring &wstring) { set(wstring); }
    path(const wchar_t *wstring) { set(wstring); }
#endif

    size_t length() const { return m_path.size(); }

    bool empty() const { return m_path.empty(); }

    bool is_absolute() const { return m_absolute; }

    path make_absolute() const;

    bool exists() const;

    size_t file_size() const;

    bool is_directory() const;

    bool is_file() const;

    std::string extension() const {
        const std::string &name = filename();
        size_t pos = name.find_last_of(".");
        if (pos == std::string::npos)
            return "";
        return name.substr(pos+1);
    }

    std::string filename() const {
        if (empty())
            return "";
        const std::string &last = m_path[m_path.size()-1];
        return last;
    }

    path parent_path() const {
        path result;
        result.m_absolute = m_absolute;

        if (m_path.empty()) {
            if (!m_absolute)
                result.m_path.push_back("..");
        } else {
            size_t until = m_path.size() - 1;
            for (size_t i = 0; i < until; ++i)
                result.m_path.push_back(m_path[i]);
        }
        return result;
    }

    path operator/(const path &other) const {
        if (other.m_absolute)
            throw std::runtime_error("path::operator/(): expected a relative path!");
        if (m_type != other.m_type)
            throw std::runtime_error("path::operator/(): expected a path of the same type!");

        path result(*this);

        for (size_t i=0; i<other.m_path.size(); ++i)
            result.m_path.push_back(other.m_path[i]);

        return result;
    }

    std::string str(path_type type = native_path) const {
        std::ostringstream oss;

        if (m_absolute) {
            if (m_type == posix_path)
                oss << "/";
            else {
                size_t length = 0;
                for (size_t i = 0; i < m_path.size(); ++i)
                    // No special case for the last segment to count the NULL character
                    length += m_path[i].length() + 1;
                // Windows requires a \\?\ prefix to handle paths longer than MAX_PATH
                // (including their null character). NOTE: relative paths >MAX_PATH are
                // not supported at all in Windows.
                if (length > MAX_PATH_WINDOWS_LEGACY)
                    oss << "\\\\?\\";
            }
        }

        for (size_t i=0; i<m_path.size(); ++i) {
            oss << m_path[i];
            if (i+1 < m_path.size()) {
                if (type == posix_path)
                    oss << '/';
                else
                    oss << '\\';
            }
        }

        return oss.str();
    }

    void set(const std::string &str, path_type type = native_path) {
        m_type = type;
        if (type == windows_path) {
            std::string tmp = str;

            // Long windows paths (sometimes) begin with the prefix \\?\. It should only
            // be used when the path is >MAX_PATH characters long, so we remove it
            // for convenience and add it back (if necessary) in str()/wstr().
            static const std::string PREFIX = "\\\\?\\";
            if (tmp.length() >= PREFIX.length()
             && std::mismatch(std::begin(PREFIX), std::end(PREFIX), std::begin(tmp)).first == std::end(PREFIX)) {
                tmp.erase(0, 4);
            }
            m_path = tokenize(tmp, "/\\");
            m_absolute = tmp.size() >= 2 && std::isalpha(tmp[0]) && tmp[1] == ':';
        } else {
            m_path = tokenize(str, "/");
            m_absolute = !str.empty() && str[0] == '/';
        }
    }

    path &operator=(const path &path) {
        m_type = path.m_type;
        m_path = path.m_path;
        m_absolute = path.m_absolute;
        return *this;
    }

    path &operator=(path &&path) {
        if (this != &path) {
            m_type = path.m_type;
            m_path = std::move(path.m_path);
            m_absolute = path.m_absolute;
        }
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const path &path) {
        os << path.str();
        return os;
    }

    bool remove_file() const;

    bool resize_file(size_t target_length);

    static path getcwd();

#if defined(_WIN32)
    std::wstring wstr(path_type type = native_path) const;

    void set(const std::wstring& wstring, path_type type = native_path);

    path &operator=(const std::wstring &str) { set(str); return *this; }
#endif

    bool operator==(const path &p) const { return p.m_path == m_path; }
    bool operator!=(const path &p) const { return p.m_path != m_path; }

protected:
    static std::vector<std::string> tokenize(const std::string &string, const std::string &delim) {
        std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
        std::vector<std::string> tokens;

        while (lastPos != std::string::npos) {
            if (pos != lastPos)
                tokens.push_back(string.substr(lastPos, pos - lastPos));
            lastPos = pos;
            if (lastPos == std::string::npos || lastPos + 1 == string.length())
                break;
            pos = string.find_first_of(delim, ++lastPos);
        }

        return tokens;
    }

protected:
#if defined(_WIN32)
    static const size_t MAX_PATH_WINDOWS = 32767;
#endif
    static const size_t MAX_PATH_WINDOWS_LEGACY = 260;
    path_type m_type;
    std::vector<std::string> m_path;
    bool m_absolute;
};

bool create_directory(const path& p);

bool create_directories(const path& p);

inline bool remove(const path& path)
{
    return path.remove_file();
}

inline bool exists(const path& path)
{
    return path.exists();
}

bool copy_file(const path& source, const path& destination);

NAMESPACE_END(filesystem)
