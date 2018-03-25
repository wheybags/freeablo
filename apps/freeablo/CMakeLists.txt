add_library(freeablo_lib # split into a library so I can link to it from tests
    fa_main.cpp

    falevelgen/levelgen.h
    falevelgen/levelgen.cpp
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
    faworld/actor/statemachine.cpp
    faworld/actor/statemachine.h

    faworld/actor.cpp
    faworld/actor.h
    faworld/behaviour.cpp
    faworld/behaviour.h
    faworld/equiptarget.cpp
    faworld/equiptarget.h
    faworld/hoverstate.cpp
    faworld/hoverstate.h
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
    faworld/itembonus.h
    faworld/itembonus.cpp
    faworld/itemfactory.h
    faworld/itemfactory.cpp
    faworld/itemenums.h
    faworld/itemenums.cpp
    faworld/itemmap.h
    faworld/itemmap.cpp
    faworld/actorstats.h
    faworld/actorstats.cpp
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
    faworld/playerbehaviour.cpp
    faworld/playerbehaviour.h
    faworld/playerstats.cpp
    faworld/playerstats.h
    faworld/target.cpp
    faworld/target.h
    faworld/playerinput.h
    faworld/playerinput.cpp
    faworld/storedata.h
    faworld/storedata.cpp

    fagui/textcolor.h
    fagui/guimanager.h
    fagui/guimanager.cpp
    fagui/dialogmanager.h
    fagui/dialogmanager.cpp
    fagui/commandhistory.h
    fagui/commandhistory.cpp
    fagui/nkhelpers.h
    fagui/nkhelpers.cpp
    fagui/menuhandler.h
    fagui/menuhandler.cpp
    fagui/autocomplete.h
    fagui/autocomplete.cpp
    fagui/menu/menuscreen.cpp
    fagui/menu/menuscreen.h
    fagui/menu/pausemenuscreen.cpp
    fagui/menu/pausemenuscreen.h
    fagui/menu/selectheromenuscreen.cpp
    fagui/menu/selectheromenuscreen.h
    fagui/menu/startingmenuscreen.cpp
    fagui/menu/startingmenuscreen.h

    engine/threadmanager.h
    engine/threadmanager.cpp
    engine/engineinputmanager.h
    engine/engineinputmanager.cpp
    engine/inputobserverinterface.h
    engine/enginemain.h
    engine/enginemain.cpp
    engine/localinputhandler.cpp
    engine/localinputhandler.h

    engine/net/server.h
    engine/net/server.cpp
    engine/net/client.h
    engine/net/client.cpp
    engine/net/multiplayerinterface.h
    engine/net/multiplayerinterface.cpp
    engine/net/netcommon.h
    engine/net/netcommon.cpp

    faaudio/audiomanager.h
    faaudio/audiomanager.cpp

    fasavegame/objectidmapper.h
    fasavegame/objectidmapper.cpp
    fasavegame/gameloader.h
    fasavegame/gameloader.cpp
)

target_link_libraries(freeablo_lib PUBLIC NuklearMisc Render Audio Serial Input enet::enet)

add_executable(freeablo main.cpp)
target_link_libraries(freeablo freeablo_lib)

set_target_properties(freeablo_lib PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
