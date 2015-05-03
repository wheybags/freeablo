#ifndef TEXT_DECORATOR_H
#define TEXT_DECORATOR_H

#include <Rocket/Core.h>

#include <vector>
#include <map>

namespace FAGui
{
    class TextDecorator : Rocket::Core::Decorator
    {
        public:
            TextDecorator(Rocket::Core::RenderInterface* renderInterface, const std::string& text, int size, const std::string& style);

        private:
            struct Font
            {
                uint8_t mFontSize[300];
                std::vector<int> mFrames;
                std::vector<int> mFramesGray;
            };

            enum FontSize
            {
                SIZE_16 = 16,
                SIZE_24 = 24,
                SIZE_30 = 30,
                SIZE_42 = 42

            };

            enum FontStyle
            {
                NORMAL,
                GRAY
            };

        private:
            virtual Rocket::Core::DecoratorDataHandle GenerateElementData(Rocket::Core::Element* element);
            virtual void ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data);
            virtual void RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle element_data);

            std::string mText;
            int mSize;
            FontStyle mStyle;

            void init(Rocket::Core::RenderInterface* renderInterface);
            static bool mIsReady;
            static std::map<int, Font > mFonts;
            static std::vector<int> mSpacebarSize;

            Rocket::Core::Geometry mGeometry;
            Rocket::Core::Vector2i mTexDimensions;

    };
}

#endif
