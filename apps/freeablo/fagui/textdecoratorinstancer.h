#ifndef TEXT_DECORATOR_INSTANCER_H
#define TEXT_DECORATOR_INSTANCER_H

#include <Rocket/Core.h>

namespace FAGui
{
    class TextDecoratorInstancer : Rocket::Core::DecoratorInstancer
    {
        public:
            TextDecoratorInstancer(Rocket::Core::RenderInterface* renderInterface);

        private:
            virtual Rocket::Core::Decorator* InstanceDecorator(const Rocket::Core::String& name, const Rocket::Core::PropertyDictionary& properties);
            virtual void ReleaseDecorator(Rocket::Core::Decorator* decorator);
            virtual void Release();

            Rocket::Core::RenderInterface* mRenderInterface;
    };
}

#endif
