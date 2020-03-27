#include <Image/image.h>
#include <SDL_image.h>
#include <cstring>
#include <faio/fafileobject.h>
#include <misc/stringops.h>

void Image::blitTo(Image& other, int32_t destOffsetX, int32_t destOffsetY)
{
    release_assert(destOffsetX >= 0 && destOffsetY >= 0);
    release_assert(destOffsetX + width() <= other.width() && destOffsetY + height() <= other.height());

    for (int32_t line = 0; line < height(); line++)
    {
        ByteColour* src = &get(0, line);
        ByteColour* dest = &other.get(destOffsetX, line + destOffsetY);

        memcpy(dest, src, sizeof(ByteColour) * width());
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
