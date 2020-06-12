#pragma once

#include <cstdint>
struct SDL_Cursor;
class Image;

namespace Render
{
    class Cursor
    {
    public:
        Cursor(Cursor&) = delete;
        Cursor(const Image& image, int32_t hotX, int32_t hotY);
        ~Cursor();

        void activateCursor() const;
        static void setDefaultCursor();

    private:
        SDL_Cursor* mSdlCursor = nullptr;
    };
}