#include "animateddecorator.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include <render/render.h>
#include <misc/misc.h>

namespace FAGui
{
    AnimatedDecorator::AnimatedDecorator(Rocket::Core::RenderInterface* renderInterface, float playTime, const std::string& imgSrc) : Rocket::Core::Decorator()
    {
        mPlayTime = playTime;

        int frame0Index = LoadTexture(imgSrc.c_str(), "");

        Render::RocketFATex* texture = (Render::RocketFATex*)GetTexture(frame0Index)->GetHandle(renderInterface);

        mFrames.push_back(frame0Index);


        for(size_t i = 0; i < texture->animLength; i++)
        {
            std::ostringstream ss;
            ss << imgSrc << "&frame=" << i;
            mFrames.push_back(LoadTexture(ss.str().c_str(), ""));
        }

        // set up vertices
        std::vector<Rocket::Core::Vertex>& vertices = mGeometry.GetVertices();
        std::vector<int>& indices = mGeometry.GetIndices();

        Rocket::Core::Vector2i texDimensions = GetTexture(frame0Index)->GetDimensions(renderInterface);

        Rocket::Core::Vertex vertex;
        vertex.colour = Rocket::Core::Colourb(255, 255, 255, 255);

        vertex.position = Rocket::Core::Vector2f(0, 0);
        vertex.tex_coord = Rocket::Core::Vector2f(0, 0);
        vertices.push_back(vertex);
        vertex.position = Rocket::Core::Vector2f(0, (float)texDimensions.y);
        vertex.tex_coord = Rocket::Core::Vector2f(0, 1);
        vertices.push_back(vertex);
        vertex.position = Rocket::Core::Vector2f((float)texDimensions.x, (float)texDimensions.y);
        vertex.tex_coord = Rocket::Core::Vector2f(1, 1);
        vertices.push_back(vertex);
        vertex.position = Rocket::Core::Vector2f((float)texDimensions.x, 0);
        vertex.tex_coord = Rocket::Core::Vector2f(1, 0);
        vertices.push_back(vertex);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);
    }

    Rocket::Core::DecoratorDataHandle AnimatedDecorator::GenerateElementData(Rocket::Core::Element* element)
    {
        UNUSED_PARAM(element);
        return (Rocket::Core::DecoratorDataHandle)NULL;
    }

    void AnimatedDecorator::ReleaseElementData(Rocket::Core::DecoratorDataHandle element_data)
    {
        UNUSED_PARAM(element_data);
    }

    void AnimatedDecorator::RenderElement(Rocket::Core::Element* element, Rocket::Core::DecoratorDataHandle element_data)
    {
        UNUSED_PARAM(element_data);

        float time = Rocket::Core::GetSystemInterface()->GetElapsedTime();
        float animSeekTime = std::fmod(time, mPlayTime);

        int currentFrame = (int)(animSeekTime/(mPlayTime/(float)mFrames.size()));
        const Rocket::Core::Texture* tex = GetTexture(mFrames[currentFrame]);

        mGeometry.SetTexture(tex);
        mGeometry.Render(element->GetAbsoluteOffset(Rocket::Core::Box::PADDING));
    }
}
