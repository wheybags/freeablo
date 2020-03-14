#pragma once

namespace Render
{
    class BindableGL
    {
    public:
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };

    class ScopedBindGL
    {
    public:
        ScopedBindGL(BindableGL& resource) : mResource(resource) { mResource.bind(); }

        ScopedBindGL(BindableGL* resource) : ScopedBindGL(*resource) {}

        ~ScopedBindGL() { mResource.unbind(); }

    private:
        BindableGL& mResource;
    };
}