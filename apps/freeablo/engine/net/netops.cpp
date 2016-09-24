#include "netops.h"

#include <assert.h>

#include <enet/enet.h>

namespace Engine
{
    WritePacket getWritePacket(PacketType type, uint32_t size, bool reliable, WritePacketResizableType resizable)
    {
        uint32_t realSize = size + 1; // 1 byte for packetType

        std::vector<uint8_t>* resizableBacking = nullptr;
        ENetPacket* packet = NULL;
        
        // god, I'm ashamed of this
        // doing some bullshit to work around the good aul c++ no default constructor problem
        WritePacket* wpTmp = (WritePacket*)malloc(sizeof(WritePacket));

        if (resizable == WritePacketResizableType::NonResizable)
        {
            if (reliable)
                packet = enet_packet_create(NULL, realSize, ENET_PACKET_FLAG_RELIABLE);
            else
                packet = enet_packet_create(NULL, realSize, ENET_PACKET_FLAG_UNSEQUENCED);

            Serial::WriteBitStream stream(packet->data, packet->dataLength);

            wpTmp->writer = Serial::WriteBitStream(packet->data, packet->dataLength);
        }
        else
        {
            resizableBacking = new std::vector<uint8_t>(realSize, 0);
            wpTmp->writer = Serial::WriteBitStream(*resizableBacking);
        }

        static_assert((int)PacketType::MAX_PACKET_TYPE < 256, "can't send PacketType as uint8 if we have > 256 of em");

        uint8_t type8 = (uint8_t)type;
        wpTmp->writer.handleInt<0, (int32_t)PacketType::MAX_PACKET_TYPE>(type8);
        assert(wpTmp->writer.tell() <= 8); // make sure that the assumption we made earlier (1 byte for packetType) is ok

        uint8_t channelId = UNRELIABLE_CHANNEL_ID;
        if (reliable)
            channelId = RELIABLE_CHANNEL_ID;

        wpTmp->packet = packet;
        wpTmp->channelId = channelId;
        wpTmp->resizableBacking = resizableBacking;
        wpTmp->reliable = reliable;

        // see above comment on nastiness
        WritePacket wpTmp2 = *wpTmp;
        free(wpTmp);
        return wpTmp2;
    }

    ENetPacket* getPacket(WritePacket wp)
    {
        if (wp.resizableBacking != nullptr)
        {
            ENetPacket* packet = nullptr;

            if (wp.reliable)
                packet = enet_packet_create(NULL, wp.resizableBacking->size(), ENET_PACKET_FLAG_RELIABLE);
            else
                packet = enet_packet_create(NULL, wp.resizableBacking->size(), ENET_PACKET_FLAG_UNSEQUENCED);

            memcpy(packet->data, &(*wp.resizableBacking)[0], wp.resizableBacking->size());
            wp.writer = Serial::WriteBitStream(nullptr, 0); // make sure noone writes to it after this...
            delete wp.resizableBacking;

            return packet;
        }
        else
        {
            return (ENetPacket*)wp.packet;
        }
    }

    void broadcastPacket(WritePacket packet, void* host)
    {
        enet_host_broadcast((ENetHost*)host, packet.channelId, getPacket(packet));
    }

    void sendPacket(WritePacket packet, void* peer)
    {
        enet_peer_send((ENetPeer*)peer, packet.channelId, getPacket(packet));
    }

    std::shared_ptr<ReadPacket> getReadPacket(void* packet)
    {
        ENetPacket* p = (ENetPacket*)packet;

        auto read = Serial::ReadBitStream(p->data, p->dataLength);

        uint8_t type8 = 0;
        read.handleInt<0, (int32_t)PacketType::MAX_PACKET_TYPE>(type8);

        return std::shared_ptr<ReadPacket>(new ReadPacket(read, (PacketType)type8, packet));
    }

    ReadPacket::~ReadPacket()
    {
        ENetPacket* p = (ENetPacket*)packet;
        enet_packet_destroy(p);
    }
}