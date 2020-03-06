#pragma once
#include "assert.h"
#include "direction.h"
#include <filesystem/path.h>
#include <iomanip>
#include <sstream>
#include <stddef.h>
#include <stdint.h>
#include <utility>

namespace Misc
{
    void saveArgv0(const char* argv0);
    filesystem::path getResourcesPath();

    template <typename T> T clamp(T value, T min, T max) { return value < min ? min : (value > max ? max : value); }

    template <typename T> class ScopedSetter
    {
    public:
        ScopedSetter(T& toSet, T val) : mOriginal(toSet), mToSet(toSet) { mToSet = val; }
        ScopedSetter(T& toSet) : mOriginal(toSet), mToSet(toSet) {}

        ~ScopedSetter() { mToSet = mOriginal; }

    private:
        T mOriginal;
        T& mToSet;
    };

    std::string numberToHumanFileSize(double sizeInBytes);

    std::string escapePathForShell(const std::string& str);
}

template <typename T> class NonNullPtr
{
public:
    NonNullPtr(T* ptr) { reset(ptr); }
    NonNullPtr(T& ref) { reset(&ref); }
    NonNullPtr(NonNullPtr<T>& other) : NonNullPtr(other.get()) {}

    void reset(T* newptr)
    {
        release_assert(newptr);
        ptr = newptr;
    }

    NonNullPtr& operator=(const NonNullPtr& other)
    {
        reset(other.ptr);
        return *this;
    }

    T* get() { return ptr; }
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }

    const T* get() const { return ptr; }
    const T* operator*() const { return ptr; }
    const T* operator->() const { return ptr; }

private:
    T* ptr = nullptr;
};

template <typename T> class NonNullConstPtr
{
public:
    NonNullConstPtr(const T* ptr) { reset(ptr); }
    NonNullConstPtr(const T& ref) { reset(&ref); }
    NonNullConstPtr(const NonNullConstPtr<T>& other) : NonNullConstPtr(other.get()) {}
    NonNullConstPtr(const NonNullPtr<T>& other) : NonNullConstPtr(other.get()) {}

    void reset(const T* newptr)
    {
        release_assert(newptr);
        ptr = newptr;
    }

    NonNullConstPtr& operator=(const NonNullConstPtr& other)
    {
        reset(other.ptr);
        return *this;
    }

    const T* get() const { return ptr; }
    const T& operator*() const { return *ptr; }
    const T* operator->() const { return ptr; }

private:
    const T* ptr = nullptr;
};

#define UNUSED_PARAM(x) (void)(x)
