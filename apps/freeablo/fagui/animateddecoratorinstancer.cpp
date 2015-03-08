#include "animateddecoratorinstancer.h"

#include "animateddecorator.h"

#include <misc/misc.h>

namespace FAGui
{
    AnimatedDecoratorInstancer::AnimatedDecoratorInstancer(Rocket::Core::RenderInterface* renderInterface)
    {
        mRenderInterface = renderInterface;

        RegisterProperty("playtime", "1").AddParser("number");
        RegisterProperty("animfile", "default").AddParser("string");
    }

    Rocket::Core::Decorator* AnimatedDecoratorInstancer::InstanceDecorator(const Rocket::Core::String& name, const Rocket::Core::PropertyDictionary& properties)
    {
        UNUSED_PARAM(name);

        float playTime = properties.GetProperty("playtime")->Get<float>();
        std::string imgSrc = std::string(properties.GetProperty("animfile")->Get<Rocket::Core::String>().CString());

        return (Rocket::Core::Decorator*)new AnimatedDecorator(mRenderInterface, playTime, imgSrc);
    }

    void AnimatedDecoratorInstancer::ReleaseDecorator(Rocket::Core::Decorator* decorator)
    {
        delete decorator;
    }

    void AnimatedDecoratorInstancer::Release()
    {
        delete this;
    }
}
