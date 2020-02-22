#pragma once
namespace FAWorld
{
    class CharacterInventory;
}

namespace Level
{
    template <class property_type> class BaseProperty
    {
    public:
        GenericProperty();
        virtual void operator=(const GenericProperty& rhs) { set(rhs.get()); }

        virtual property_type get() { return mProperty; }

        virtual void set(property_type property) { mProperty = property; }

    private:
        property_type mProperty;
    };
}
