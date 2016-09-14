#ifndef FA_NET_OPS
#define FA_NET_OPS

#include <memory>

#include <serial/bitstream.h>
#include "netcommon.h"

namespace Engine
{
    enum class WritePacketResizableType
    {
        Resizable,
        NonResizable
    };

    struct WritePacket
    {
        Serial::WriteBitStream writer;

        //private:
            void* packet;
            uint8_t channelId;
            std::vector<uint8_t>* resizableBacking;
            bool reliable;

            friend void sendPacket(WritePacket packet, void* peer);
            friend void broadcastPacket(WritePacket packet, void* host);
            friend WritePacket getWritePacket(PacketType type, uint32_t size, bool reliable, WritePacketResizableType resizable);
    };

    WritePacket getWritePacket(PacketType type, uint32_t size, bool reliable, WritePacketResizableType resizable = WritePacketResizableType::NonResizable);

    void broadcastPacket(WritePacket packet, void* host);
    void sendPacket(WritePacket packet, void* peer);

    struct ReadPacket
    {
        Serial::ReadBitStream reader;
        PacketType type;

        ~ReadPacket();

        private:
            ReadPacket(Serial::ReadBitStream r, PacketType t, void* p) : reader(r), type(t), packet(p) {}
            
            void* packet;

            friend std::shared_ptr<ReadPacket> getReadPacket(void* packet);
    };

    std::shared_ptr<ReadPacket> getReadPacket(void* packet);
}

#endif