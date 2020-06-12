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
    farender/animationplayer.cpp
    farender/animationplayer.h
    farender/fontinfo.cpp
    farender/fontinfo.h
    farender/spriteloader.cpp
    farender/spriteloader.h
    farender/debugrenderdata.h

    faworld/actor/basestate.cpp
    faworld/actor/basestate.h
    faworld/actor/attackstate.cpp
    faworld/actor/attackstate.h
    faworld/actor/statemachine.cpp
    faworld/actor/statemachine.h

    faworld/actor.cpp
    faworld/actor.h
    faworld/actoranimationmanager.cpp
    faworld/actoranimationmanager.h
    faworld/actorstats.cpp
    faworld/actorstats.h
    faworld/behaviour.cpp
    faworld/behaviour.h
    faworld/equiptarget.cpp
    faworld/equiptarget.h
    faworld/faction.cpp
    faworld/faction.h
    faworld/findpath.cpp
    faworld/findpath.h
    faworld/gamelevel.cpp
    faworld/gamelevel.h
    faworld/hoverstate.cpp
    faworld/hoverstate.h
    faworld/inventory.cpp
    faworld/inventory.h
    faworld/itemfactory.cpp
    faworld/itemfactory.h
    faworld/itemmap.cpp
    faworld/itemmap.h
    faworld/missile/missile.cpp
    faworld/missile/missile.h
    faworld/missile/missileactorengagement.cpp
    faworld/missile/missileattributes.cpp
    faworld/missile/missilecreation.cpp
    faworld/missile/missileenums.h
    faworld/missile/missilegraphic.cpp
    faworld/missile/missilegraphic.h
    faworld/missile/missilemovement.cpp
    faworld/monster.cpp
    faworld/monster.h
    faworld/movementhandler.cpp
    faworld/movementhandler.h
    faworld/player.cpp
    faworld/player.h
    faworld/playerbehaviour.cpp
    faworld/playerbehaviour.h
    faworld/playerfactory.cpp
    faworld/playerfactory.h
    faworld/playerinput.cpp
    faworld/playerinput.h
    faworld/position.cpp
    faworld/position.h
    faworld/potion.cpp
    faworld/potion.h
    faworld/storedata.cpp
    faworld/storedata.h
    faworld/target.cpp
    faworld/target.h
    faworld/world.cpp
    faworld/world.h
    faworld/enums.h
    faworld/enums.cpp
    faworld/item/itembase.cpp
    faworld/item/itembase.h
    faworld/item/itembaseholder.cpp
    faworld/item/itembaseholder.h
    faworld/item/equipmentitembase.cpp
    faworld/item/equipmentitembase.h
    faworld/item/usableitembase.cpp
    faworld/item/usableitembase.h
    faworld/item/item.cpp
    faworld/item/item.h
    faworld/item/equipmentitem.cpp
    faworld/item/equipmentitem.h
    faworld/item/usableitem.cpp
    faworld/item/usableitem.h

    fagui/textcolor.h
    fagui/textcolor.cpp
    fagui/guimanager.h
    fagui/guimanager.cpp
    fagui/dialogmanager.h
    fagui/dialogmanager.cpp
    fagui/commandhistory.h
    fagui/commandhistory.cpp
    fagui/nkhelpers.h
    fagui/nkhelpers.cpp
    fagui/menuentry.h
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
    fagui/menu/multiplayermenu.cpp
    fagui/menu/multiplayermenu.h
    fagui/menu/multiplayerconnecting.cpp
    fagui/menu/multiplayerconnecting.h
    fagui/mouseandclickmenu.cpp
    fagui/mouseandclickmenu.h
    fagui/characterdialoguepopup.cpp
    fagui/characterdialoguepopup.h
    fagui/shopdialogs.cpp
    fagui/shopdialogs.h
    fagui/talkdialoguepopup.h
    fagui/talkdialoguepopup.cpp
    fagui/npcs/adriadialog.cpp
    fagui/npcs/adriadialog.h
    fagui/npcs/caindialog.cpp
    fagui/npcs/caindialog.h
    fagui/npcs/farnhamdialog.cpp
    fagui/npcs/farnhamdialog.h
    fagui/npcs/gilliandialog.cpp
    fagui/npcs/gilliandialog.h
    fagui/npcs/griswolddialog.cpp
    fagui/npcs/griswolddialog.h
    fagui/npcs/ogdendialog.cpp
    fagui/npcs/ogdendialog.h
    fagui/npcs/pepindialog.cpp
    fagui/npcs/pepindialog.h
    fagui/npcs/wirtdialog.cpp
    fagui/npcs/wirtdialog.h

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
    fasavegame/gameloader.cpp farender/levelrenderer.cpp farender/levelrenderer.h engine/debugsettings.h engine/debugsettings.cpp faworld/item/golditembase.cpp faworld/item/golditembase.h faworld/item/golditem.cpp faworld/item/golditem.h faworld/magiceffects/magiceffectbase.cpp faworld/magiceffects/magiceffectbase.h faworld/item/itemprefixorsuffixbase.cpp faworld/item/itemprefixorsuffixbase.h faworld/magiceffects/magiceffect.cpp faworld/magiceffects/magiceffect.h faworld/magiceffects/simplebuffdebuffeffect.cpp faworld/magiceffects/simplebuffdebuffeffect.h faworld/magiceffects/simplebuffdebuffeffectbase.cpp faworld/magiceffects/simplebuffdebuffeffectbase.h faworld/item/itemprefixorsuffix.cpp faworld/item/itemprefixorsuffix.h)

target_link_libraries(freeablo_lib PUBLIC NuklearMisc Render Audio Serial Input Random Image enet cxxopts fmt::fmt)
target_include_directories(freeablo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(freeablo main.cpp)
target_link_libraries(freeablo freeablo_lib)

set_target_properties(freeablo_lib PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

install(TARGETS freeablo DESTINATION bin)
