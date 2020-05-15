#include <Image/image.h>
#include <SDL_image.h>
#include <cstring>
#include <faio/fafileobject.h>
#include <misc/savePNG.h>
#include <misc/stringops.h>

// clang-format off
#include <misc/disablewarn.h>
#include "../../extern/jo_gif/jo_gif.cpp"
#include <misc/enablewarn.h>
// clang-format on

Image::Image(int32_t x, int32_t y) : mData(x, y, Misc::Array2D<ByteColour>::InitType::Uninitialised)
{
    void* dataPtr = mData.data();
    memset(dataPtr, 0, x * y * sizeof(ByteColour));
}

void Image::blitTo(Image& other,
                   int32_t srcOffsetX,
                   int32_t srcOffsetY,
                   int32_t srcW,
                   int32_t srcH,
                   int32_t destOffsetX,
                   int32_t destOffsetY,
                   bool overwriteWithTransparent) const
{
    release_assert(destOffsetX >= 0 && destOffsetY >= 0);
    release_assert(destOffsetX + srcW <= other.width() && destOffsetY + srcH <= other.height());
    release_assert(srcOffsetX + srcW <= this->width() && srcOffsetY + srcH <= this->height());

    if (overwriteWithTransparent)
    {
        for (int32_t y = 0; y < srcH; y++)
        {
            const ByteColour* src = &this->get(srcOffsetX, y + srcOffsetY);
            ByteColour* dest = &other.get(destOffsetX, y + destOffsetY);

            memcpy(dest, src, sizeof(ByteColour) * srcW);
        }
    }
    else
    {
        for (int32_t y = 0; y < srcH; y++)
        {
            for (int32_t x = 0; x < srcW; x++)
            {
                const ByteColour& sourcePixel = this->get(srcOffsetX + x, srcOffsetY + y);
                if (sourcePixel.a)
                    other.get(destOffsetX + x, destOffsetY + y) = sourcePixel;
            }
        }
    }
}

static ByteColour getPixelFromSurface(const SDL_Surface* s, int x, int y)
{
    Uint32 pix;

    int bpp = s->format->BytesPerPixel;
    // Here p is the address to the pixel we want to retrieve
    Uint8* p = (Uint8*)s->pixels + y * s->pitch + x * bpp;

    switch (bpp)
    {
        case 1:
            pix = *p;
            break;

        case 2:
            pix = *(Uint16*)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                pix = p[0] << 16 | p[1] << 8 | p[2];
            else
                pix = p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            pix = *(Uint32*)p;
            break;

        default:
            pix = 0;
    }

    Uint8 r, g, b, a;
    SDL_GetRGBA(pix, s->format, &r, &g, &b, &a);

    return ByteColour(r, g, b, a == 255);
}

Image Image::loadFromFile(const std::string& path)
{
    // TODO: use our own image loading code here, instead of relying on SDL
    // stb_image (https://github.com/nothings/stb/blob/master/stb_image.h) could handle most of it,
    // and we would need to get a pcx decoder from somewhere else.

    std::vector<uint8_t> fileData = FAIO::FAFileObject(path).readAll();

    std::string extension = Misc::StringUtils::getFileExtension(path);
    SDL_Surface* surface = IMG_LoadTyped_RW(SDL_RWFromMem(fileData.data(), fileData.size()), 1, extension.c_str());
    release_assert(surface);

    Image image(surface->w, surface->h);
    for (int32_t y = 0; y < image.height(); y++)
    {
        for (int32_t x = 0; x < image.width(); x++)
        {
            image.get(x, y) = getPixelFromSurface(surface, x, y);
        }
    }

    SDL_FreeSurface(surface);

    return image;
}

Image::TrimmedData Image::calculateTrimTransparentEdges() const
{
    bool isEmpty = true;

    int32_t left = width() - 1;
    int32_t right = 0;
    int32_t top = height() - 1;
    int32_t bottom = 0;

    for (int32_t y = 0; y < height(); y++)
    {
        for (int32_t x = 0; x < width(); x++)
        {
            if (get(x, y).a != 0)
            {
                isEmpty = false;

                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }

    if (isEmpty)
        return TrimmedData();

    TrimmedData trimmedData;
    trimmedData.trimmedWidth = right - left + 1;
    trimmedData.trimmedHeight = bottom - top + 1;
    trimmedData.trimmedOffsetX = left;
    trimmedData.trimmedOffsetY = top;

    return trimmedData;
}

void Image::saveToGif(std::vector<Image> images, const std::string& path)
{
    release_assert(images.size());

    int32_t width = images[0].width();
    int32_t height = images[0].height();

    jo_gif_t gif = jo_gif_start(path.c_str(), width, height, 0, 256);

    for (const auto& image : images)
    {
        const Image* useImage = &image;
        Image tmp;

        if (image.width() != width || image.height() != height)
        {
            tmp = Image(width, height);
            image.blitTo(tmp, 0, 0, std::min(width, image.width()), std::min(height, image.height()), 0, 0, true);
            useImage = &tmp;
        }

        auto dataPointer = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(useImage->mData.data()));
        jo_gif_frame(&gif, dataPointer, 10, true, 0x00, 0xFF, 0x00);
    }

    jo_gif_end(&gif);
}

void Image::saveToPng(const Image& image, const std::string& path)
{
    int32_t pitch = sizeof(ByteColour) * image.width();
    void* pixels = const_cast<void*>(reinterpret_cast<const void*>(image.mData.data()));
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, image.width(), image.height(), 32, pitch, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    SDL_SavePNG(surface, path.c_str());

    SDL_FreeSurface(surface);
}
