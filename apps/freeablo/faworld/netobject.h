#ifndef FA_NET_OBJECT_H
#define FA_NET_OBJECT_H

#include <enet/enet.h>

namespace FAWorld
{
    class NetObject
    {
        public:
            virtual ~NetObject() {}

            virtual size_t getSize() = 0;

            /*
             * @param packet packet to write to
             * @param start index in package to start at
             * @return new position after writing
             */
            virtual size_t writeTo(ENetPacket* packet, size_t start) = 0;

            /*
             * @param packet packet to read from
             * @param start index in package to start at
             * @return new position after reading
             */
            virtual size_t readFrom(ENetPacket* packet, size_t start) = 0;
    };
}

#endif
