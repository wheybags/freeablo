#include "objectidmapper.h"
#include <misc/assert.h>

namespace FASaveGame
{
    void ObjectIdMapper::addClass(const std::string& name, std::function<void*(GameLoader&)> constructor)
    {
        release_assert(!mMappings.count(name));
        mMappings[name] = constructor;
    }

    void* ObjectIdMapper::construct(const std::string& name, GameLoader& gameLoader)
    {
        release_assert(mMappings.count(name));
        return mMappings[name](gameLoader);
    }
}
