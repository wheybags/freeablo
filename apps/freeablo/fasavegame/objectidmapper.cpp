#include "objectidmapper.h"
#include <cassert>

namespace FASaveGame
{
    void ObjectIdMapper::addClass(const std::string& name, std::function<void*(GameLoader&)> constructor)
    {
        assert(!mMappings.count(name));
        mMappings[name] = constructor;
    }

    void* ObjectIdMapper::construct(const std::string& name, GameLoader& gameLoader)
    {
        assert(mMappings.count(name));
        return mMappings[name](gameLoader);
    }
}
