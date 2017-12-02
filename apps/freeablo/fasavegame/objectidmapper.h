#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <cstdint>

namespace FASaveGame
{
    class GameLoader;

    class ObjectIdMapper
    {
    public:
        void addClass(const std::string& name, std::function<void*(GameLoader&)> constructor);
        void* construct(const std::string& name, GameLoader& gameLoader);

    private:
        std::unordered_map<std::string, std::function<void*(GameLoader&)>> mMappings;
    };
}
