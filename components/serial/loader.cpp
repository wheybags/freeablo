#include "loader.h"
#include "streaminterface.h"
#include <misc/assert.h>

namespace Serial
{
    Loader::Loader(ReadStreamInterface& stream) : mStream(stream)
    {
        mVersion = load<uint32_t>();
        release_assert(mVersion <= CurrentSaveVersion); // TODO: recoverable errors here
        release_assert(mVersion >= MinimumSupportedSaveVersion);
    }

    template <> bool Loader::load<bool>() { return mStream.read_bool(); }

    template <> int64_t Loader::load<int64_t>() { return mStream.read_int64_t(); }

    template <> uint64_t Loader::load<uint64_t>() { return mStream.read_uint64_t(); }

    template <> int32_t Loader::load<int32_t>() { return mStream.read_int32_t(); }

    template <> uint32_t Loader::load<uint32_t>() { return mStream.read_uint32_t(); }

    template <> int16_t Loader::load<int16_t>() { return mStream.read_int16_t(); }

    template <> uint16_t Loader::load<uint16_t>() { return mStream.read_uint16_t(); }

    template <> int8_t Loader::load<int8_t>() { return mStream.read_int8_t(); }

    template <> uint8_t Loader::load<uint8_t>() { return mStream.read_uint8_t(); }

    template <> std::string Loader::load<std::string>() { return mStream.read_string(); }

    template <> Misc::Point Loader::load<Misc::Point>()
    {
        Misc::Point point;
        point.x = mStream.read_int32_t();
        point.y = mStream.read_int32_t();
        return point;
    }

    void Loader::startCategory(const std::string& name) { mStream.startCategory(name); }

    void Loader::endCategory(const std::string& name) { mStream.endCategory(name); }

    Saver::Saver(WriteStreamInterface& stream) : mStream(stream) { save(CurrentSaveVersion); }

    void Saver::save(bool val) { mStream.write(val); }

    void Saver::save(int64_t val) { mStream.write(val); }

    void Saver::save(uint64_t val) { mStream.write(val); }

    void Saver::save(int32_t val) { mStream.write(val); }

    void Saver::save(uint32_t val) { mStream.write(val); }

    void Saver::save(int16_t val) { mStream.write(val); }

    void Saver::save(uint16_t val) { mStream.write(val); }

    void Saver::save(int8_t val) { mStream.write(val); }

    void Saver::save(uint8_t val) { mStream.write(val); }

    void Saver::save(const std::string& val) { mStream.write(val); }

    void Saver::save(const Misc::Point& point)
    {
        mStream.write(point.x);
        mStream.write(point.y);
    }

    void Saver::startCategory(const std::string& name) { mStream.startCategory(name); }

    void Saver::endCategory(const std::string& name) { mStream.endCategory(name); }

    template <typename T> ScopedCategory<T>::ScopedCategory(std::string&& name, T& saverOrLoader) : mName(std::move(name)), mSaverOrLoader(saverOrLoader)
    {
        mSaverOrLoader.startCategory(mName);
    }

    template <typename T> ScopedCategory<T>::~ScopedCategory() { mSaverOrLoader.endCategory(mName); }

    // explicit template instantiations, instead of putting all the implementations in the header
    template class ScopedCategory<Loader>;
    template class ScopedCategory<Saver>;
}
