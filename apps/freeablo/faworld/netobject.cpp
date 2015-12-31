#include "netobject.h"

#include <assert.h>

namespace FAWorld
{
    void NetObject::tickDone(bool wasSent)
    {
        if(wasSent)
            mPriority = 1;
        else
            mPriority += 1;
    }

    size_t NetObject::getPriority()
    {
        return getBasePriority() * mPriority;
    }

    // use a static size table. I would use an std::map, but because of static initialisation orders
    // (the macros that actually do the registering exploit static class instance constructors), we
    // can't guarantee that it would be initialised before we call it. With a static array like this,
    // it should be zeroed before static constructors are run. We could use a pointer and
    // initialise it on first use but then that'd show up as a leak in valgrind and generally
    // just feels a bit nasty. We're unlikely to need more than this anyway, and if we ever do it's
    // trivial to increase it.
    static constexpr int32_t factoryTableSize = 20;
    NetObject* (*factoryFuncTable[factoryTableSize])() = {NULL};

    void NetObject::registerNetObjectClass(int32_t classId, NetObject* (*factoryFunc)())
    {
        assert(classId < factoryTableSize && "NetObject is out of bounds");
        assert(factoryFuncTable[classId] == NULL && "duplicate NetObject id");

        factoryFuncTable[classId] = factoryFunc;
    }

    NetObject* NetObject::construct(int32_t classId)
    {
        return factoryFuncTable[classId]();
    }

    int32_t NetObject::getClassId()
    {
        assert(false && "class has no id");
        return -1;
    }
}
