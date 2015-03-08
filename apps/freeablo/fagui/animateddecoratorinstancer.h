#ifndef ANIMATED_DECORATOR_INSTANCER_H
#define ANIMATED_DECORATOR_INSTANCER_H

#include <Rocket/Core.h>

namespace FAGui
{
    class AnimatedDecoratorInstancer : Rocket::Core::DecoratorInstancer
    {
        public:
            AnimatedDecoratorInstancer(Rocket::Core::RenderInterface* renderInterface);

        private:
            virtual Rocket::Core::Decorator* InstanceDecorator(const Rocket::Core::String& name, const Rocket::Core::PropertyDictionary& properties);
            virtual void ReleaseDecorator(Rocket::Core::Decorator* decorator);
            virtual void Release();

            Rocket::Core::RenderInterface* mRenderInterface;
    };
}

#endif
