add_library(Cel 
    cel/celfile.cpp cel/celfile.h  
    cel/celframe.h cel/celframe.cpp
    cel/pal.cpp cel/pal.h 
    cel/tiledecoding.cpp cel/tiledecoding.h 
    cel/celdecoding.cpp cel/celdecoding.h
    cel/cl2decoding.cpp cel/cl2decoding.h)

add_library(FAIO faio/faio.cpp faio/faio.h)
target_link_libraries(FAIO StormLib ${Boost_LIBRARIES})

add_library(Levels 
    level/dun.cpp  
    level/dun.h
    level/min.cpp  
    level/min.h  
    level/tileset.cpp  
    level/tileset.h
    level/level.h
    level/level.cpp
    level/sol.cpp
    level/sol.h)
target_link_libraries(Levels FAIO)

add_library(Misc 
    misc/stringops.h
    misc/helper2d.h
    misc/md5.h
    misc/md5.cpp
    misc/fareadini.h
    misc/fareadini.cpp
    misc/misc.h
    misc/misc.cpp)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)


set(RenderFiles render/render.h render/sdlcommon.cpp)
if(USE_SDL2)
    set(RenderFiles ${RenderFiles} render/sdl2backend.cpp)
else()
    set(RenderFiles ${RenderFiles} render/sdlbackend.cpp)
endif()

add_library(Render ${RenderFiles})
target_link_libraries(Render Cel Levels ${SDL_LIBRARY})


add_library(Input
    input/inputmanager.h
    input/sdlcommon.cpp)

add_library(DiabloExe
    diabloexe/diabloexe.h
    diabloexe/diabloexe.cpp
    diabloexe/monster.h
    diabloexe/monster.cpp
    diabloexe/npc.h
    diabloexe/npc.cpp)
target_link_libraries(DiabloExe ${Boost_Libraries} Misc)
