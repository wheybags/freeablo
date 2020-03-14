#pragma once
#include <glad/glad.h>
#include <optional>

namespace Render
{
    class BindableGL
    {
    public:
        virtual void bind(std::optional<GLenum> binding) = 0;
        virtual void unbind(std::optional<GLenum> binding) = 0;
    };

    class ScopedBindGL
    {
    public:
        explicit ScopedBindGL(BindableGL& resource, std::optional<GLenum> binding = std::nullopt) : mResource(resource), mBinding(binding)
        {
            mResource.bind(mBinding);
        }
        explicit ScopedBindGL(BindableGL* resource, std::optional<GLenum> binding = std::nullopt) : ScopedBindGL(*resource, binding) {}

        ~ScopedBindGL() { mResource.unbind(mBinding); }

    private:
        BindableGL& mResource;
        std::optional<GLenum> mBinding;
    };
}