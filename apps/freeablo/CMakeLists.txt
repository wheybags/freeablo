add_library(freeablo_lib # split into a library so I can link to it from tests
    fa_main.cpp

    falevelgen/levelgen.h
    falevelgen/levelgen.cpp
    falevelgen/random.cpp
    falevelgen/random.h
    falevelgen/mst.cpp
    falevelgen/mst.h
    falevelgen/tileset.cpp
    falevelgen/tileset.h

    farender/renderer.cpp
    farender/renderer.h
    farender/spritecache.cpp
    farender/spritecache.h
    farender/spritemanager.cpp
    farender/spritemanager.h
    farender/animationplayer.cpp
    farender/animationplayer.h
    farender/fontinfo.cpp
    farender/fontinfo.h

    faworld/actor/basestate.cpp
    faworld/actor/basestate.h
    faworld/actor/attackstate.cpp
    faworld/actor/attackstate.h

    faworld/actor.cpp
    faworld/actor.h
    faworld/behaviour.cpp
    faworld/behaviour.h
    faworld/hoverstate.cpp
    faworld/hoverstate.h
    faworld/monster.h
    faworld/monster.cpp
    faworld/player.h
    faworld/player.cpp
    faworld/playerfactory.h
    faworld/playerfactory.cpp
    faworld/position.h
    faworld/position.cpp
    faworld/world.cpp
    faworld/world.h
    faworld/inventory.h
    faworld/inventory.cpp
    faworld/item.h
    faworld/item.cpp
    faworld/itemmanager.h
    faworld/itemmanager.cpp
    faworld/itemmap.h
    faworld/itemmap.cpp
    faworld/actorstats.h
    faworld/actorstats.cpp
    faworld/characterstats.h
    faworld/characterstats.cpp
    faworld/netobject.h
    faworld/netobject.cpp
    faworld/gamelevel.cpp
    faworld/gamelevel.h
    faworld/findpath.h
    faworld/findpath.cpp
    faworld/faction.cpp
    faworld/faction.h
    faworld/movementhandler.cpp
    faworld/movementhandler.h
    faworld/actoranimationmanager.h
    faworld/actoranimationmanager.cpp

    fagui/guimanager.h
    fagui/guimanager.cpp
    fagui/commandhistory.h
    fagui/commandhistory.cpp
    fagui/autocomplete.h
    fagui/autocomplete.cpp

    engine/threadmanager.h
    engine/threadmanager.cpp
    engine/engineinputmanager.h
    engine/engineinputmanager.cpp
    engine/inputobserverinterface.h
    engine/enginemain.h
    engine/enginemain.cpp

    engine/net/netmanager.cpp
    engine/net/netmanager.h
    engine/net/server.h
    engine/net/server.cpp
    engine/net/client.h
    engine/net/client.cpp
    engine/net/netcommon.h
    engine/net/netcommon.cpp
    engine/net/netops.h
    engine/net/netops.cpp

    faaudio/audiomanager.h
    faaudio/audiomanager.cpp

    fasavegame/savegame.h
    fasavegame/savegamemanager.h
    fasavegame/savegamemanager.cpp
)

target_link_libraries(freeablo_lib PUBLIC NuklearMisc Render Audio Serial Input enet::enet)

add_executable(freeablo main.cpp)
target_link_libraries(freeablo freeablo_lib)

set_target_properties(freeablo_lib PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
