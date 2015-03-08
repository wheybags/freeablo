#ifndef ANIMATED_DECORATOR_H
#define ANIMATED_DECORATOR_H

#include <Rocket/Core.h>

#include <vector>

namespace FAGui
{
    class AnimatedDecorator : Rocket::Core::Decorator
    {
        public:
            AnimatedDecorator(Rocket::Core::RenderInterface* renderInterface, float playTime, const std::string& imgSrc);

        private:
            virtual Rocket::Core::DecoratorDataHandle GenerateElementData(Rocket::Core::Element* element);
            virtual void ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data);
            virtual void RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle element_data);

            std::vector<int> mFrames;

            Rocket::Core::Geometry mGeometry;

            float mPlayTime;
    };
}

#endif
