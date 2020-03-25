#pragma once
#include "buffer.h"
#include <misc/misc.h>
#include <render/vertexlayout.h>
#include <vector>

namespace Render
{
    class VertexArrayObject
    {
    public:
        VertexArrayObject(VertexArrayObject&) = delete;
        explicit VertexArrayObject(std::vector<NonNullConstPtr<VertexLayout>> bindings) : mBindings(std::move(bindings)) {}
        virtual ~VertexArrayObject() = default;

        size_t getVertexBufferCount() { return mBindings.size(); }
        const std::vector<NonNullConstPtr<VertexLayout>>& getBindings() const { return mBindings; }

        virtual Buffer* getVertexBuffer(size_t index) = 0;
        virtual Buffer* getIndexBuffer() = 0;

    protected:
        std::vector<NonNullConstPtr<VertexLayout>> mBindings;
    };
}