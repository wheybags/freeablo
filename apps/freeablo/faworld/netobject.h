#ifndef FA_NET_OBJECT_H
#define FA_NET_OBJECT_H

#include <enet/enet.h>
#include <stdint.h>

// put in implementation file for NetObject subclasses. Requires the below macro.
// see registerNetObjectClass below
#define STATIC_HANDLE_NET_OBJECT_IN_IMPL(className) \
    int32_t className::mClassIndirectId = -1; \
    NetObject* StaticInitialiseFunc##className() { return new className(); } \
    struct StaticInitialise##className \
    { \
        StaticInitialise##className() \
        { \
            className::mClassIndirectId = NetObject::registerNetObjectClass(#className, StaticInitialiseFunc##className); \
        } \
    } _StaticInitialise##className;

// put in class defenition of NetObject subclasses
#define STATIC_HANDLE_NET_OBJECT_IN_CLASS() \
    public: \
        static int32_t mClassIndirectId; \
        virtual int32_t getClassId() { return ::FAWorld::NetObject::getClassIdFromIndirectId(mClassIndirectId); }


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

            /*!
             * \return The implementor's base priority for synchronisation, eg, Player will have higher priority than Monster
             */
             virtual size_t getBasePriority()
             {
                return 1;
             }

            /*!
             * \return The priority for sending this NetObject. Increases with every tick that it is not sent, and
             * is reset to 1 every time it is sent. Value is weighted by getBasePriority()
             */
             size_t getPriority();

             /*!
              * \brief Called after every network tick. Used to accumulate priorities, for later recall with getPriority()
              * \param wasSent whether the object was sent this tick.
              */
             void tickDone(bool wasSent);

             /*!
              * \brief Registers a factory function for a NetObject subclass, so they can be constructed using just the id
              * \return an indirect id that can be converted to a real id with getClassIdFromIndirectId()
              */
             static int32_t registerNetObjectClass(const char* typeName, NetObject* (*factoryFunc)());

             /*!
              * \brief construct a class instance of a NetObject subclass, using a classId previously
              * registered with registerNetObjectClass(). The reason for this is that the classId can
              * be sent over the network allowing the reciever to construct the appropriate class
              * using just the id.
              */
             static NetObject* construct(int32_t classId);

             /*!
              * \brief get the class id for sending over the network, this is the id used on construct() and
              * registerNetObjectClass() above.
              */
             virtual int32_t getClassId();

             /*!
              * \param indirectId an indirect id from registerNetObjectClass()
              * \return an id for use with construct()
              */
             static int32_t getClassIdFromIndirectId(int32_t indirectId);


        private:
             size_t mPriority = 1;
    };
}

#endif
