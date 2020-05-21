#pragma once
#include <misc/array2d.h>
#include <string>

struct ByteColour
{
    ByteColour(uint8_t r, uint8_t g, uint8_t b, bool visible) : r(r), g(g), b(b), a(visible ? 255 : 0) {}
    ByteColour() = default;

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

static_assert(sizeof(ByteColour) == 4);

class Image
{
public:
    typedef typename Misc::Array2D<ByteColour>::iterator iterator;
    typedef typename Misc::Array2D<ByteColour>::const_iterator const_iterator;

    Image() = default;
    Image(int32_t x, int32_t y);
    Image(int32_t width, int32_t height, std::vector<ByteColour>&& data) : mData(width, height, std::move(data)) {}
    Image(int32_t width, int32_t height, ByteColour* data, PointerDataType dataOwnership) : mData(width, height, data, dataOwnership) {}
    Image(const Image&) = delete;
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    const ByteColour& get(int32_t x, int32_t y) const { return mData.get(x, y); }
    ByteColour& get(int32_t x, int32_t y) { return mData.get(x, y); }

    iterator begin() { return mData.begin(); }
    const_iterator begin() const { return mData.begin(); }
    iterator end() { return mData.end(); }
    const_iterator end() const { return mData.end(); }

    int32_t width() const { return mData.width(); }
    int32_t height() const { return mData.height(); }

    void blitTo(Image& other,
                int32_t srcOffsetX,
                int32_t srcOffsetY,
                int32_t srcW,
                int32_t srcH,
                int32_t destOffsetX,
                int32_t destOffsetY,
                bool overwriteWithTransparent) const;
    void blitTo(Image& other, int32_t destOffsetX, int32_t destOffsetY) const
    {
        blitTo(other, 0, 0, this->width(), this->height(), destOffsetX, destOffsetY, true);
    }

    struct TrimmedData
    {
        int32_t trimmedOffsetX = 0;
        int32_t trimmedOffsetY = 0;
        int32_t trimmedWidth = 0;
        int32_t trimmedHeight = 0;
    };

    // calculates a trimmed rectangle that excludes transparent pixels around the edge of the image
    TrimmedData calculateTrimTransparentEdges() const;

    static Image loadFromFile(const std::string& path);
    static void saveToGif(const std::vector<Image> images, const std::string& path);
    static void saveToPng(const Image& image, const std::string& path);

public:
    Misc::Array2D<ByteColour> mData;
};
