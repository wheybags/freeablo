#ifndef FA_BITSTREAM_H
#define FA_BITSTREAM_H

#include <cstdint>
#include <string>
#include <iostream>
#include <cstring>
#include <vector>

#include <assert.h>

namespace Serial
{
    namespace BSPos
    {
        enum BSPos
        {
            Start,
            Current,
            End
        };
    }

    namespace Error
    {
        enum Error
        {
            Success,
            OutOfRange,
            EndOfStream,
            InvalidData
        };

        std::string getName(Error err);

        #ifdef ERR_GET_NAME_IMPL
            const char* ErrorStrings[] = 
            { 
                "Success",
                "OutOfRange",
                "EndOfStream",
                "InvalidData" 
            };

            std::string getName(Error err)
            {
                return std::string(ErrorStrings[err]);
            }
        #endif //ERR_GET_NAME_IMPL
    }

    class BitStreamBase
    {
        public:
            int64_t tell();
            bool seek(int64_t offset, BSPos::BSPos origin);

        protected:
            void init(uint8_t* buf, int64_t sizeInBytes);

            int64_t mCurrentPos;
            int64_t mSize;
            uint8_t* mData;
    };

    class WriteBitStream : public BitStreamBase
    {
        public:
            bool isWriting() { return true; }

            WriteBitStream(uint8_t* buf, int64_t sizeInBytes);
            WriteBitStream(std::vector<uint8_t>& resizeableBacking); /// creates a resizeable stream backed by the vector

            Error::Error handleBool(bool& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int64_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint64_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int32_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint32_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int8_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint8_t& val);

            Error::Error handleInt32(int32_t& val);
            Error::Error handleInt32(uint32_t& val);

            template <class SerializableClass> Error::Error handleObject(SerializableClass& o);

            Error::Error handleString(uint8_t* data, uint32_t len);

            void fillWithZeros();

        private:
            template <int64_t minVal, int64_t maxVal> Error::Error handleIntBase(uint64_t& val, bool handleSign);

            void resize(int64_t sizeInBits);

            std::vector<uint8_t>* mResizableBacking = nullptr;
    };

    class ReadBitStream : public BitStreamBase
    {
        public:
            bool isWriting() { return false; }

            ReadBitStream(uint8_t* buf, int64_t sizeInBytes);

            Error::Error handleBool(bool& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int64_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint64_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int32_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint32_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int8_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint8_t& val);

            Error::Error handleInt32(int32_t& val);
            Error::Error handleInt32(uint32_t& val);

            template <class SerializableClass> Error::Error handleObject(SerializableClass& o);

            Error::Error handleString(uint8_t* data, uint32_t len);

            bool verifyZeros();
        
        private:
            template <int64_t minVal, int64_t maxVal> Error::Error handleIntBase(uint64_t& val, bool handleSign);
    };
}

#include "bitstream.inl"

#ifdef NDEBUG

    #define SERIALISE_MACRO_BASE(macro) do  \
    {                                       \
        auto ret = macro;                   \
        if (ret != Serial::Error::Success)  \
            return ret;                     \
    } while(0)

#else
    #define SERIALISE_MACRO_BASE(macro) do              \
    {                                                   \
        auto ret = macro;                               \
        if (ret != Serial::Error::Success)              \
        {                                               \
            assert(ret == Serial::Error::EndOfStream);  \
            return ret;                                 \
        }                                               \
    } while(0)
#endif



// workaround for msvc's bad handling of "dependent-name"s https://stackoverflow.com/questions/2974780/visual-c-compiler-allows-dependent-name-as-a-type-without-typename
#ifdef _MSC_VER
    #define _serialise_int(stream, min, max, val) stream.handleInt<min, max>(val)
    #define _serialise_int32(stream, val) stream.handleInt32(val)
    #define _serialise_bool(stream, val) stream.handleBool(val)
    #define _serialise_object(stream, val) stream.handleObject(val) 
    #define _serialise_str(stream, val, len) stream.handleString(val, len)
#else
    #define _serialise_int(stream, min, max, val) stream.template handleInt<min, max>(val)
    #define _serialise_int32(stream, val) stream.handleInt32(val)
    #define _serialise_bool(stream, val) stream.handleBool(val)
    #define _serialise_object(stream, val) stream.template handleObject(val) 
    #define _serialise_str(stream, val, len) stream.handleString(val, len)
#endif

#define serialise_int(stream, min, max, val) SERIALISE_MACRO_BASE(_serialise_int(stream, min, max, val))
#define serialise_int32(stream, val) SERIALISE_MACRO_BASE(_serialise_int32(stream, val))
#define serialise_bool(stream, val) SERIALISE_MACRO_BASE(_serialise_bool(stream, val))
#define serialise_object(stream, val) SERIALISE_MACRO_BASE(_serialise_object(stream, val))
#define serialise_str(stream, val, len) SERIALISE_MACRO_BASE(_serialise_str(stream, val, len))

#define serialise_enum(stream, type, val) do                        \
{                                                                   \
    int32_t enumValInt = (int32_t)val;                              \
    serialise_int(stream, 0, (int32_t)type::ENUM_END, enumValInt);  \
    val = (type)enumValInt;                                         \
} while(0)

#define FA_SERIAL_TEMPLATE_INSTANTIATE(class) \
    template Serial::Error::Error class::faSerial<Serial::ReadBitStream>(Serial::ReadBitStream& stream); \
    template Serial::Error::Error class::faSerial<Serial::WriteBitStream>(Serial::WriteBitStream& stream)

#endif // !FA_BITSTREAM_H
