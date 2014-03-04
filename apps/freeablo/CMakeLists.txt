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
    
    faworld/actor.cpp
    faworld/actor.h  
    faworld/player.h  
    faworld/position.h  
    faworld/position.cpp
    faworld/world.cpp  
    faworld/world.h)
