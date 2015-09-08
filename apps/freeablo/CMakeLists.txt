add_executable(freeablo 
    main.cpp 

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
    
    faworld/actor.cpp
    faworld/actor.h 
    faworld/monster.h
    faworld/monster.cpp 
    faworld/player.h  
	faworld/player.cpp
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
	faworld/actorstats.h
	faworld/actorstats.cpp
	faworld/characterstats.h
	faworld/characterstats.cpp
	faworld/monsterstats.h
	faworld/monsterstats.cpp
    faworld/netobject.h
    
	fagui/guimanager.h
    fagui/guimanager.cpp
    fagui/animateddecorator.h
    fagui/animateddecorator.cpp
    fagui/animateddecoratorinstancer.h
    fagui/animateddecoratorinstancer.cpp
    fagui/console.h
    fagui/console.cpp
    fagui/commandhistory.h
    fagui/commandhistory.cpp
    fagui/autocomplete.h
    fagui/autocomplete.cpp
    fagui/fapython.cpp
    fagui/fapython.h

    engine/threadmanager.h
    engine/threadmanager.cpp
    engine/input.h
    engine/input.cpp
    engine/enginemain.h
    engine/enginemain.cpp
    engine/netmanager.cpp
    engine/netmanager.h

    faaudio/audiomanager.h
    faaudio/audiomanager.cpp

    fasavegame/savegame.h
    fasavegame/savegamemanager.h
    fasavegame/savegamemanager.cpp


)
set_target_properties(freeablo PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
