#include "path.h"

#if defined(_WIN32)
# include <windows.h>
# include <ShlObj.h>
#else
# include <unistd.h>
# include <fcntl.h>
#endif
#include <sys/stat.h>

#if defined(__linux)

# include <linux/limits.h>
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
# include <copyfile.h>
#endif
#if defined(__linux)
# include <sys/sendfile.h>
#endif

NAMESPACE_BEGIN(filesystem)

path path::make_absolute() const
{
#if !defined(_WIN32)
    char temp[PATH_MAX];
    if (realpath(str().c_str(), temp) == NULL)
        throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
    return path(temp);
#else
    std::wstring value = wstr(), out(MAX_PATH_WINDOWS, '\0');
    DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH_WINDOWS, &out[0], NULL);
    if (length == 0)
        throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
    return path(out.substr(0, length));
#endif
}

bool path::exists() const
{
#if defined(_WIN32)
    return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat sb;
    return stat(str().c_str(), &sb) == 0;
#endif
}

size_t path::file_size() const
{
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstr().c_str(), &sb) != 0)
        throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#else
    struct stat sb;
    if (stat(str().c_str(), &sb) != 0)
        throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#endif
    return (size_t)sb.st_size;
}

bool path::is_directory() const
{
#if defined(_WIN32)
    DWORD result = GetFileAttributesW(wstr().c_str());
    if (result == INVALID_FILE_ATTRIBUTES)
        return false;
    return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat sb;
    if (stat(str().c_str(), &sb))
        return false;
    return S_ISDIR(sb.st_mode);
#endif
}

bool path::is_file() const
{
#if defined(_WIN32)
    DWORD attr = GetFileAttributesW(wstr().c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
    struct stat sb;
    if (stat(str().c_str(), &sb))
        return false;
    return S_ISREG(sb.st_mode);
#endif
}

bool path::remove_file() const
{
#if !defined(_WIN32)
    return std::remove(str().c_str()) == 0;
#else
    return DeleteFileW(wstr().c_str()) != 0;
#endif
}

bool path::resize_file(size_t target_length)
{
#if !defined(_WIN32)
    return ::truncate(str().c_str(), (off_t)target_length) == 0;
#else
    HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    LARGE_INTEGER size;
    size.QuadPart = (LONGLONG)target_length;
    if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0)
    {
        CloseHandle(handle);
        return false;
    }
    if (SetEndOfFile(handle) == 0)
    {
        CloseHandle(handle);
        return false;
    }
    CloseHandle(handle);
    return true;
#endif
}

path path::getcwd()
{
#if !defined(_WIN32)
    char temp[PATH_MAX];
    if (::getcwd(temp, PATH_MAX) == NULL)
        throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
    return path(temp);
#else
    std::wstring temp(MAX_PATH_WINDOWS, '\0');
    if (!_wgetcwd(&temp[0], MAX_PATH_WINDOWS))
        throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
    return path(temp.c_str());
#endif
}

#if defined(_WIN32)
std::wstring path::wstr(path_type type) const
{
    std::string temp = str(type);
    int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0], size);
    return result;
}

void path::set(const std::wstring& wstring, path_type type)
{
    std::string string;
    if (!wstring.empty())
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), NULL, 0, NULL, NULL);
        string.resize(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), &string[0], size, NULL, NULL);
    }
    set(string, type);
}

#endif

bool create_directory(const path& p)
{
#if defined(_WIN32)
    return CreateDirectoryW(p.wstr().c_str(), NULL) != 0;
#else
    return mkdir(p.str().c_str(), S_IRWXU) == 0;
#endif
}

bool create_directories(const path& p)
{
#if defined(_WIN32)
    return SHCreateDirectory(nullptr, p.make_absolute().wstr().c_str()) == ERROR_SUCCESS;
#else
    if (create_directory(p.str().c_str()))
        return true;

    if (p.empty())
        return false;

    if (errno == ENOENT)
    {
        if (create_directory(p.parent_path()))
            return mkdir(p.str().c_str(), S_IRWXU) == 0;
        else
            return false;
    }
    return false;
#endif
}

bool copy_file(const path& source, const path& destination)
{
#if defined(_WIN32)
    return CopyFileW(source.wstr().c_str(), destination.wstr().c_str(), FALSE) != 0;
#else
    bool retval = false;
    std::vector<char> buffer(1024 * 1024 * 5);
    size_t lastWriteSize = 0;

    FILE* sourceFile = fopen(source.str().c_str(), "rb");
    FILE* destFile = fopen(destination.str().c_str(), "wb");
    if (!sourceFile || !destFile)
        goto done;

    do
    {
        size_t read = fread(buffer.data(), 1, buffer.size(), sourceFile);
        if (ferror(sourceFile))
            goto done;

        lastWriteSize = fwrite(buffer.data(), 1, read, destFile);
        if (ferror(destFile))
            goto done;

    } while (lastWriteSize);

    retval = true;

done:
    if (sourceFile)
        fclose(sourceFile);
    if (destFile)
        fclose(destFile);

    return retval;
#endif
}

NAMESPACE_END(fielsystem)
