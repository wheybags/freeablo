#include "simplevec2.h"

#define _USE_MATH_DEFINES
#include "math.h"

namespace Vec2Helper
{
    template <typename T> T sqrt(T val) { return T(FixedPoint(val).sqrt().floor()); }
    template <> FixedPoint sqrt(FixedPoint val) { return val.sqrt(); }
    template <> float sqrt(float val) { return ::sqrtf(val); }
    template <> double sqrt(double val) { return ::sqrt(val); }

    template <typename T> T atan2_degrees(T y, T x) { return T(FixedPoint::atan2_degrees(y, x).floor()); }
    template <> FixedPoint atan2_degrees(FixedPoint y, FixedPoint x) { return FixedPoint::atan2_degrees(y, x); }
    template <> float atan2_degrees(float y, float x) { return ::atan2f(y, x) * (180 / M_PI); }
    template <> double atan2_degrees(double y, double x) { return ::atan2(y, x) * (180 / M_PI); }

    template <typename T> FixedPoint forceFixedPoint(T val) { return val; }
    template <> FixedPoint forceFixedPoint(float val) { return FixedPoint::fromRawValue(int64_t(val * FixedPoint::scalingFactor)); }
    template <> FixedPoint forceFixedPoint(double val) { return FixedPoint::fromRawValue(int64_t(val * FixedPoint::scalingFactor)); }

    template <typename T> T invalid() { return std::numeric_limits<T>::max(); }
    template <> FixedPoint invalid() { return FixedPoint::invalid(); }
}

template <> Vec2<FixedPoint>::Vec2(Serial::Loader& loader)
{
    x.load(loader);
    y.load(loader);
}

template <typename T> Vec2<T>::Vec2(Serial::Loader& loader)
{
    x = loader.load<T>();
    y = loader.load<T>();
}

template <> void Vec2<FixedPoint>::save(Serial::Saver& saver) const
{
    x.save(saver);
    y.save(saver);
}

template <typename T> void Vec2<T>::save(Serial::Saver& saver) const
{
    saver.save(x);
    saver.save(y);
}

template <typename T> Vec2<T>& Vec2<T>::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator*=(const Vec2& other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator/=(const Vec2& other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator+=(T scalar)
{
    x += scalar;
    y += scalar;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator-=(T scalar)
{
    x -= scalar;
    y -= scalar;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator*=(T scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

template <typename T> Vec2<T>& Vec2<T>::operator/=(T scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}
template <typename T> Vec2<T> Vec2<T>::invalid() { return Vec2<T>(Vec2Helper::invalid<T>(), Vec2Helper::invalid<T>()); }

template <typename T> T Vec2<T>::magnitude() const { return Vec2Helper::sqrt(x * x + y * y); }

template <typename T> T Vec2<T>::magnitudeSquared() const { return x * x + y * y; }

template <typename T> void Vec2<T>::normalise()
{
    if (x == 0 && y == 0)
        return;

    T mag = magnitude();

    if (mag == 0)
        return;

    x = x / mag;
    y = y / mag;
}

template <typename T> Misc::Direction Vec2<T>::getDirection() const
{
    // isometricDir 0° is east with respect to isometric tiles (i.e. south east).
    FixedPoint isometricDir = Vec2Helper::forceFixedPoint(Vec2Helper::atan2_degrees(y, x));
    return Misc::Direction::fromIsometricDegrees(isometricDir);
}

template class Vec2<int32_t>;
template class Vec2<float>;
template class Vec2<double>;
template class Vec2<FixedPoint>;
