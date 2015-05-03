#include "textdecoratorinstancer.h"

#include "textdecorator.h"

#include <misc/misc.h>

namespace FAGui
{
    TextDecoratorInstancer::TextDecoratorInstancer(Rocket::Core::RenderInterface* renderInterface)
    {
        mRenderInterface = renderInterface;

        RegisterProperty("style", "normal").AddParser("string");
        RegisterProperty("size", "42").AddParser("number");
        RegisterProperty("text", "TEST").AddParser("string");
    }

    Rocket::Core::Decorator* TextDecoratorInstancer::InstanceDecorator(const Rocket::Core::String& name, const Rocket::Core::PropertyDictionary& properties)
    {
        UNUSED_PARAM(name);

        std::string text = std::string(properties.GetProperty("text")->Get<Rocket::Core::String>().CString());
        std::string style = std::string(properties.GetProperty("style")->Get<Rocket::Core::String>().CString());
        int size = properties.GetProperty("size")->Get<int>();

        return (Rocket::Core::Decorator*)new TextDecorator(mRenderInterface, text, size, style);
    }

    void TextDecoratorInstancer::ReleaseDecorator(Rocket::Core::Decorator* decorator)
    {
        delete decorator;
    }

    void TextDecoratorInstancer::Release()
    {
        delete this;
    }
}
