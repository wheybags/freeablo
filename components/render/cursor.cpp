#include <Image/image.h>
#include <SDL.h>
#include <render/cursor.h>

namespace Render
{
    Cursor::Cursor(const Image& image, int32_t hotX, int32_t hotY)
    {
        int32_t pitch = sizeof(ByteColour) * image.width();
        void* pixels = const_cast<void*>(reinterpret_cast<const void*>(image.mData.data()));
        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, image.width(), image.height(), 32, pitch, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

        mSdlCursor = SDL_CreateColorCursor(surface, hotX, hotY);

        SDL_FreeSurface(surface);
    }

    Cursor::~Cursor() { SDL_FreeCursor(mSdlCursor); }

    void Cursor::activateCursor() const { SDL_SetCursor(mSdlCursor); }
    void Cursor::setDefaultCursor() { SDL_SetCursor(SDL_GetDefaultCursor()); }
}