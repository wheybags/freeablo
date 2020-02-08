#pragma once
#include "assert.h"
#include "direction.h"
#include <iomanip>
#include <sstream>
#include <stddef.h>
#include <stdint.h>
#include <utility>

namespace Misc
{
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

    std::string escapeSpacesOnPath(const std::string& str);
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

#include <memory>
#include <utility>

namespace nonstd
{
    namespace detail
    {
        template <class T> struct up_if_object
        {
            typedef std::unique_ptr<T> type;
        };

        template <class T> struct up_if_object<T[]>
        {
        };

        template <class T, std::size_t N> struct up_if_object<T[N]>
        {
        };

        template <class T> struct up_if_array
        {
        };

        template <class T> struct up_if_array<T[]>
        {
            typedef std::unique_ptr<T[]> type;
        };

        template <class T> struct up_remove_reference
        {
            typedef T type;
        };

        template <class T> struct up_remove_reference<T&>
        {
            typedef T type;
        };

        template <class T> struct up_remove_reference<T&&>
        {
            typedef T type;
        };

        template <class T> struct up_element
        {
        };

        template <class T> struct up_element<T[]>
        {
            typedef T type;
        };
    }

    template <class T> inline typename detail::up_if_object<T>::type make_unique() { return std::unique_ptr<T>(new T()); }

    template <class T, class... Args> inline typename detail::up_if_object<T>::type make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template <class T> inline typename detail::up_if_object<T>::type make_unique(typename detail::up_remove_reference<T>::type&& value)
    {
        return std::unique_ptr<T>(new T(std::move(value)));
    }

    template <class T> inline typename detail::up_if_object<T>::type make_unique_noinit() { return std::unique_ptr<T>(new T); }

    template <class T> inline typename detail::up_if_array<T>::type make_unique(std::size_t size)
    {
        return std::unique_ptr<T>(new typename detail::up_element<T>::type[size]());
    }

    template <class T> inline typename detail::up_if_array<T>::type make_unique_noinit(std::size_t size)
    {
        return std::unique_ptr<T>(new typename detail::up_element<T>::type[size]);
    }
}

#define UNUSED_PARAM(x) (void)(x)
