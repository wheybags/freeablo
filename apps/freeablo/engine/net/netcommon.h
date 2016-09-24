#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <stdint.h>
#include <serial/bitstream.h>

namespace Engine
{
    #define ENABLE_NET_STALL_ON_TIMEOUT

    uint32_t getStallThreshold();


    static constexpr uint8_t UNRELIABLE_CHANNEL_ID = 0;
    static constexpr uint8_t RELIABLE_CHANNEL_ID = 1;

    enum class PacketType
    {
        Level,
        Sprite,
        NewClient,
        MAX_PACKET_TYPE
    };

    struct ServerPacketHeader
    {
        uint32_t numPlayers;
        uint32_t tick;

        template <class Stream>
        Serial::Error::Error faSerial(Stream& stream)
        {
            serialise_int(stream, 0, 1024, numPlayers);
            serialise_int32(stream, tick);
            return Serial::Error::Success;
        }
    };

    struct ClientPacket
    {
        size_t destX;
        size_t destY;
        int32_t levelIndex; // TODO: don't just trust this data
    };
}

#endif