#pragma once
#include <cstdint>
#include <vector>

namespace FAWorld
{
    class PlayerInput;
}

namespace Engine
{
    class MultiplayerInterface
    {
    public:
        virtual ~MultiplayerInterface() = default;

        virtual std::vector<FAWorld::PlayerInput> getAndClearInputs() = 0;
        virtual void update() = 0;

        static constexpr int32_t RELIABLE_CHANNEL_ID = 0;
        static constexpr int32_t UNRELIABLE_CHANNEL_ID = 0;

        enum class MessageType : uint8_t
        {
            // server-to-client
            MapToClient,

            // client-to-server
            AcknowledgeMapToServer
        };
    };
}
