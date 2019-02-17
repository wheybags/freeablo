#include "textstream.h"
#include <misc/assert.h>
#include <misc/stringops.h>

namespace Serial
{
    std::string TextReadStream::readTypedLine(const std::string& expectedType)
    {
        mLine++;
        std::string line;
        std::getline(mData, line);

        line.erase(0, line.find_first_not_of(" "));

        std::vector<std::string> data = Misc::StringUtils::split(line, ' ');

        release_assert(data.size() == 2);

        if (data[0] == "CATEGORY" || data[0] == "CATEGORY_END")
            return readTypedLine(expectedType); // ignore

        release_assert(data[0] == expectedType);

        return std::string(std::move(data[1]));
    }

    template <typename T> T fromString(const std::string& str)
    {
        std::istringstream iss;
        iss.str(str);
        T retval;
        iss >> retval;
        return retval;
    }

    bool TextReadStream::read_bool()
    {
        std::string data = readTypedLine("BOOL");
        release_assert(data == "true" || data == "false");
        return data == "true";
    }

    int64_t TextReadStream::read_int64_t()
    {
        std::string data = readTypedLine("I64");
        return fromString<int64_t>(data);
    }

    uint64_t TextReadStream::read_uint64_t()
    {
        std::string data = readTypedLine("U64");
        return fromString<uint64_t>(data);
    }

    int32_t TextReadStream::read_int32_t()
    {
        std::string data = readTypedLine("I32");
        return fromString<int32_t>(data);
    }

    uint32_t TextReadStream::read_uint32_t()
    {
        std::string data = readTypedLine("U32");
        return fromString<uint32_t>(data);
    }

    int16_t TextReadStream::read_int16_t()
    {
        std::string data = readTypedLine("I16");
        return fromString<int16_t>(data);
    }

    uint16_t TextReadStream::read_uint16_t()
    {
        std::string data = readTypedLine("U16");
        return fromString<uint16_t>(data);
    }

    int8_t TextReadStream::read_int8_t()
    {
        std::string data = readTypedLine("I8");
        return fromString<int32_t>(data); // use 32 here to stop it being interpreted as char
    }

    uint8_t TextReadStream::read_uint8_t()
    {
        std::string data = readTypedLine("U8");
        return fromString<uint32_t>(data); // use 32 here to stop it being interpreted as char
    }

    std::string TextReadStream::read_string()
    {
        std::string sizeData = readTypedLine("STRING");
        uint32_t size = fromString<uint32_t>(sizeData);

        mData >> std::noskipws;

        std::string tmp;
        char c;
        for (uint32_t i = 0; i < size; i++)
        {
            mData >> c;
            tmp += c;

            if (c == '\n')
                mLine++;
        }

        mData >> c;
        release_assert(c == '\n'); // trailing newline
        mLine++;

        mData >> std::skipws;

        return tmp;
    }

    void TextReadStream::startCategory(const std::string& name)
    {
        std::string data = readTypedLine("CATEGORY");
        release_assert(name == data);
    }

    void TextReadStream::endCategory(const std::string& name)
    {
        std::string data = readTypedLine("CATEGORY_END");
        release_assert(name == data);
    }

    size_t TextWriteStream::getCurrentSize() const
    {
        // why is tell non-const??
        return mData.size();
    }

    void TextWriteStream::resize(size_t size) { mData.resize(size); }

    std::pair<uint8_t*, size_t> TextWriteStream::getData() { return std::make_pair((uint8_t*)mData.data(), mData.size()); }

    void TextWriteStream::write(bool val) { writeTypedLine("BOOL", val ? "true" : "false"); }

    void TextWriteStream::write(int64_t val) { writeTypedLine("I64", std::to_string(val)); }

    void TextWriteStream::write(uint64_t val) { writeTypedLine("U64", std::to_string(val)); }

    void TextWriteStream::write(int32_t val) { writeTypedLine("I32", std::to_string(val)); }

    void TextWriteStream::write(uint32_t val) { writeTypedLine("U32", std::to_string(val)); }

    void TextWriteStream::write(int16_t val) { writeTypedLine("I16", std::to_string(val)); }

    void TextWriteStream::write(uint16_t val) { writeTypedLine("U16", std::to_string(val)); }

    void TextWriteStream::write(int8_t val) { writeTypedLine("I8", std::to_string(val)); }

    void TextWriteStream::write(uint8_t val) { writeTypedLine("U8", std::to_string(val)); }

    void TextWriteStream::write(const std::string& val)
    {
        writeTypedLine("STRING", std::to_string(uint32_t(val.size())));
        mData += val + "\n";
    }

    void TextWriteStream::startCategory(const std::string& name)
    {
        writeTypedLine("CATEGORY", name);
        mTabIndex++;
    }

    void TextWriteStream::endCategory(const std::string& name)
    {
        mTabIndex--;
        writeTypedLine("CATEGORY_END", name);
    }

    void TextWriteStream::writeTypedLine(const std::string& type, const std::string& data)
    {
        for (int32_t i = 0; i < mTabIndex; i++)
            mData += "  ";

        mData += type + " " + data + "\n";
    }
}
