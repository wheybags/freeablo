#include "textdecorator.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include <render/render.h>
#include <misc/misc.h>
#include <faio/faio.h>

namespace FAGui
{
    bool TextDecorator::mIsReady = false;
    std::map<int, TextDecorator::Font> TextDecorator::mFonts;
    std::vector<int> TextDecorator::mSpacebarSize;

    void TextDecorator::init(Rocket::Core::RenderInterface* renderInterface)
    {
        // Load fonts
        std::vector<std::string> fonts;
        fonts.push_back("ui_art/font16g.pcx&trans=0,255,0&vanim=16");
        fonts.push_back("ui_art/font24g.pcx&trans=0,255,0&vanim=26");
        fonts.push_back("ui_art/font30g.pcx&trans=0,255,0&vanim=31");
        fonts.push_back("ui_art/font42g.pcx&trans=0,255,0&vanim=42");

        std::vector<std::string> fontsGray;
        fontsGray.push_back("ui_art/font16s.pcx&trans=0,255,0&vanim=16");
        fontsGray.push_back("ui_art/font24s.pcx&trans=0,255,0&vanim=26");
        fontsGray.push_back("ui_art/font30s.pcx&trans=0,255,0&vanim=31");
        fontsGray.push_back("ui_art/font42y.pcx&trans=0,255,0&vanim=42");

        std::vector<std::string> fontsBinaryFiles;
        fontsBinaryFiles.push_back("ui_art/font16.bin");
        fontsBinaryFiles.push_back("ui_art/font24.bin");
        fontsBinaryFiles.push_back("ui_art/font30.bin");
        fontsBinaryFiles.push_back("ui_art/font42.bin");

        std::vector<int> fontsSize;
        fontsSize.push_back(16);
        fontsSize.push_back(24);
        fontsSize.push_back(30);
        fontsSize.push_back(42);

        mSpacebarSize.push_back(10);
        mSpacebarSize.push_back(16);
        mSpacebarSize.push_back(20);
        mSpacebarSize.push_back(26);

        for(int i = 0 ; i < (int)fonts.size(); i++)
        {
            int frame0Index = LoadTexture(fonts[i].c_str(), "");
            Render::RocketFATex* texture = (Render::RocketFATex*)GetTexture(frame0Index)->GetHandle(renderInterface);

            Font font;
            for(size_t k = 0; k < texture->animLength; k++)
            {
                std::ostringstream ss;
                ss << fonts[i] << "&frame=" << k;
                font.mFrames.push_back(LoadTexture(ss.str().c_str(), ""));

                // Gray version
                ss.str("");
                ss << fontsGray[i] << "&frame=" << k;
                font.mFramesGray.push_back(LoadTexture(ss.str().c_str(), ""));
            }

            //Read binary file with widths of letters

            FAIO::FAFile * binaryFile = FAIO::FAfopen(fontsBinaryFiles[i]);
            size_t size = FAIO::FAsize(binaryFile);
            for(unsigned int k = 0 ; k < size ; ++k)
            {
                FAIO::FAfread(&font.mFontSize[k], 1, 1, binaryFile);
            }
            FAIO::FAfclose(binaryFile);

            // Dirty hack for size of spacebar
            font.mFontSize[' ' + 2] = mSpacebarSize[i];


            mFonts[fontsSize[i]] = font;
        }

        mIsReady = true;
    }

    TextDecorator::TextDecorator(Rocket::Core::RenderInterface* renderInterface, const std::string& text, int size, const std::string& style) : Rocket::Core::Decorator(),
        mText(text),
        mSize(size)
    {
        if(!mIsReady)
            init(renderInterface);

        if(style == "gray")
            mStyle = GRAY;
        else
            mStyle = NORMAL;

        // validate size
        if(size != SIZE_16 &&
           size != SIZE_24 &&
           size != SIZE_30 &&
           size != SIZE_42)
        {
            std::cout << "Wrong font size: " << size << " for text: " << text << std::endl;
            return;
        }

        int frame0Index = mFonts[size].mFrames[0];

        // set up vertices
        std::vector<Rocket::Core::Vertex>& vertices = mGeometry.GetVertices();
        std::vector<int>& indices = mGeometry.GetIndices();

        mTexDimensions = GetTexture(frame0Index)->GetDimensions(renderInterface);

        Rocket::Core::Vertex vertex;
        vertex.colour = Rocket::Core::Colourb(255, 255, 255, 255);

        vertex.position = Rocket::Core::Vector2f(0, 0);
        vertex.tex_coord = Rocket::Core::Vector2f(0, 0);
        vertices.push_back(vertex);
        vertex.position = Rocket::Core::Vector2f(0, mTexDimensions.y);
        vertex.tex_coord = Rocket::Core::Vector2f(0, 1);
        vertices.push_back(vertex);
        vertex.position = Rocket::Core::Vector2f(mTexDimensions.x, mTexDimensions.y);
        vertex.tex_coord = Rocket::Core::Vector2f(1, 1);
        vertices.push_back(vertex);
        vertex.position = Rocket::Core::Vector2f(mTexDimensions.x, 0);
        vertex.tex_coord = Rocket::Core::Vector2f(1, 0);
        vertices.push_back(vertex);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);
    }

    Rocket::Core::DecoratorDataHandle TextDecorator::GenerateElementData(Rocket::Core::Element* element)
    {
        UNUSED_PARAM(element);
        return (Rocket::Core::DecoratorDataHandle)NULL;
    }

    void TextDecorator::ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data)
    {
        UNUSED_PARAM(element_data);
    }

    void TextDecorator::RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle element_data)
    {
        UNUSED_PARAM(element_data);

        int textLength = mText.size();
        float offset = 0.0f;

        for(int i = 0 ; i < textLength ; i++)
        {
            int currentFrame = mText[i];
            const Rocket::Core::Texture* tex;

            if(mStyle == GRAY)
                tex = GetTexture(mFonts[mSize].mFramesGray[currentFrame]);
            else
                tex = GetTexture(mFonts[mSize].mFrames[currentFrame]);

            Rocket::Core::Vector2f absoluteOffsetVector = element->GetAbsoluteOffset(Rocket::Core::Box::PADDING);
            Rocket::Core::Vector2f position = Rocket::Core::Vector2f(absoluteOffsetVector.x + offset,absoluteOffsetVector.y);
            mGeometry.SetTexture(tex);
            mGeometry.Render(position);

            offset += mFonts[mSize].mFontSize[currentFrame + 2];

        }
    }
}
