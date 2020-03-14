#pragma once
#include "buffer.h"
#include <render/vertexlayout.h>
#include <vector>

namespace Render
{
    class VertexArrayObject
    {
    public:
        explicit VertexArrayObject(std::vector<NonNullConstPtr<VertexLayout>> bindings) : mBindings(std::move(bindings)) {}

        virtual ~VertexArrayObject() = default;

        size_t getVertexBufferCount() { return mBindings.size(); }

        virtual Buffer* getVertexBuffer(size_t index) = 0;

        virtual Buffer* getIndexBuffer() = 0;

    public:
        std::vector<NonNullConstPtr<VertexLayout>> mBindings;
    };
}