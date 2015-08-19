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
	faworld/actorstats.hpp
	faworld/characterstats.h
	faworld/characterstats.cpp
    
    fagui/guimanager.h
    fagui/guimanager.cpp
    fagui/animateddecorator.h
    fagui/animateddecorator.cpp
    fagui/animateddecoratorinstancer.h
    fagui/animateddecoratorinstancer.cpp

    engine/threadmanager.h
    engine/threadmanager.cpp

    faaudio/audiomanager.h
    faaudio/audiomanager.cpp
)
set_target_properties(freeablo PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
