#include "netobject.h"

#include <assert.h>

#include <vector>
#include <iostream>
#include <algorithm>

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
    const char* typeNames[factoryTableSize] = {NULL};
    int32_t indirectionArray[factoryTableSize] = {0};

    int32_t current = 0;

    int32_t NetObject::registerNetObjectClass(const char* typeName, NetObject* (*factoryFunc)())
    {
        int32_t classIndirectId = current;
        current++;

        factoryFuncTable[classIndirectId] = factoryFunc;
        typeNames[classIndirectId] = typeName;

        std::vector<std::string> strs;
        for(int32_t i = 0; i < current; i++)
            strs.push_back(typeNames[i]);

        std::sort(strs.begin(), strs.end());

        for(int32_t i = 0; i < current; i++)
        {
            for(int32_t j = 0; j < current; j++)
            {
                if(strs[j] == typeNames[i])
                    indirectionArray[i] = j;
            }
        }

        return classIndirectId;
    }

    int32_t NetObject::getClassIdFromIndirectId(int32_t indirectId)
    {
        return indirectionArray[indirectId];
    }

    NetObject* NetObject::construct(int32_t classId)
    {
        for(int32_t i = 0; i < current; i++)
        {
            if(indirectionArray[i] == classId)
                return factoryFuncTable[i]();
        }

        return NULL;
    }

    int32_t NetObject::getClassId()
    {
        assert(false && "class has no id");
        return -1;
    }
}
