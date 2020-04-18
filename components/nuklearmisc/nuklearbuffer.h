#pragma once
#include <fa_nuklear.h>

class NuklearBuffer
{
public:
    NuklearBuffer() { nk_buffer_init_default(&mBuffer); }
    NuklearBuffer(NuklearBuffer&) = delete;
    NuklearBuffer(NuklearBuffer&& other)
    {
        mBuffer = other.mBuffer;
        other.mBuffer = {};
        nk_buffer_init_default(&other.mBuffer);
    }

    ~NuklearBuffer() { nk_buffer_free(&mBuffer); }

    operator nk_buffer*() { return &mBuffer; }
    operator const nk_buffer*() const { return &mBuffer; }

public:
    nk_buffer mBuffer;
};