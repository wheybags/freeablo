#pragma once
#include "../../faworld/world.h"
#include <boost/optional.hpp>
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

        virtual boost::optional<std::vector<FAWorld::PlayerInput>> getAndClearInputs(FAWorld::Tick tick) = 0;
        virtual void update() = 0;

        static constexpr int32_t RELIABLE_CHANNEL_ID = 0;
        static constexpr int32_t UNRELIABLE_CHANNEL_ID = 0;

        enum class MessageType : uint8_t
        {
            // server-to-client
            MapToClient,
            InputsToClient,

            // client-to-server
            AcknowledgeMapToServer,
            InputsToServer
        };
    };
}
