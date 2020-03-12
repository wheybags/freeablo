#include "enums.h"
#include <string>

namespace FAWorld
{
    const char* playerClassToString(PlayerClass value)
    {
        switch (value)
        {
            case PlayerClass::warrior:
                return "Warrior";
            case PlayerClass::rogue:
                return "Rogue";
            case PlayerClass::sorceror:
                return "Sorceror";
            case PlayerClass::none:
                break;
        }
        invalid_enum(PlayerClass, value);
    }

    PlayerClass playerClassFromString(std::string_view str)
    {
        if (str == "Warrior")
            return PlayerClass::warrior;
        else if (str == "Rogue")
            return PlayerClass::rogue;
        else if (str == "Sorceror")
            return PlayerClass::sorceror;

        message_and_abort_fmt("Invalid character class string: \"%s\"", std::string(str).c_str());
    }
}
