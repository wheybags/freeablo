#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

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
