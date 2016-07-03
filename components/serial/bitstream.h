#ifndef FA_BITSTREAM_H
#define FA_BITSTREAM_H

#include <cstdint>
#include <string>
#include <iostream>

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
            WriteBitStream(uint8_t* buf, int64_t sizeInBytes);

            Error::Error handleBool(bool& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int64_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint64_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int32_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint32_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int8_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint8_t& val);

            Error::Error handleInt32(int32_t& val);

            template <class SerializableClass> Error::Error handleObject(SerializableClass& o);

        private:
            template <int64_t minVal, int64_t maxVal> Error::Error handleIntBase(uint64_t& val, bool handleSign);
    };

    class ReadBitStream : public BitStreamBase
    {
        public:
            ReadBitStream(uint8_t* buf, int64_t sizeInBytes);

            Error::Error handleBool(bool& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int64_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint64_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int32_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint32_t& val);

            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(int8_t& val);
            template <int64_t minVal, int64_t maxVal> Error::Error handleInt(uint8_t& val);

            Error::Error handleInt32(int32_t& val);

            template <class SerializableClass> Error::Error handleObject(SerializableClass& o);
        
        private:
            template <int64_t minVal, int64_t maxVal> Error::Error handleIntBase(uint64_t& val, bool handleSign);
    };
}

#include "bitstream.inl"


// workaround for msvc's bad handling of "dependent-name"s https://stackoverflow.com/questions/2974780/visual-c-compiler-allows-dependent-name-as-a-type-without-typename
#ifdef _MSC_VER
    #define serialise_int(stream, min, max, val) stream.handleInt<min, max>(val)
    #define serialise_int32(stream, val) stream.handleInt32(val)
    #define serialise_bool(stream, val) stream.handleBool(val)
    #define serialise_object(stream, val) stream.handleObject(val) 
#else
    #define serialise_int(stream, min, max, val) stream.template handleInt<min, max>(val)
    #define serialise_int32(stream, val) stream.template handleInt32(val)
    #define serialise_bool(stream, val) stream.template handleBool(val)
    #define serialise_object(stream, val) stream.template handleObject(val) 
#endif

#endif // !FA_BITSTREAM_H
