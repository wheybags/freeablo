#ifndef FA_NET_OBJECT_H
#define FA_NET_OBJECT_H

#include <enet/enet.h>

namespace FAWorld
{
    class NetObject
    {
        public:
            virtual ~NetObject() {}

            /*!
             * \return size to be written
             */
            virtual size_t getWriteSize() = 0;

            /*!
             * \brief Write object to packet.
             * \param packet packet to write to
             * \param start index in package to start at, must be incremented by amount written by implementors
             * \return success
             */
            virtual bool writeTo(ENetPacket* packet, size_t& position) = 0;

            /*!
             * \param packet packet to read from
             * \param start index in package to start at, must be incremented by amount read by implementors
             * \return success
             */
            virtual bool readFrom(ENetPacket* packet, size_t& position) = 0;
    };
}

#endif
