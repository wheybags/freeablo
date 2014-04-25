add_library(Cel 
    cel/celfile.cpp cel/celfile.h  
    cel/celframe.h cel/celframe.cpp
    cel/pal.cpp cel/pal.h 
    cel/tiledecoding.cpp cel/tiledecoding.h 
    cel/celdecoding.cpp cel/celdecoding.h
    cel/cl2decoding.cpp cel/cl2decoding.h)

add_library(FAIO faio/faio.cpp faio/faio.h)
target_link_libraries(FAIO StormLib ${Boost_LIBRARIES})
set_target_properties(FAIO PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

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
set_target_properties(Levels PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

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
set_target_properties(Misc PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")


set(RenderFiles render/render.h render/sdlcommon.cpp render/levelobjects.cpp render/levelobjects.h)
if(USE_SDL2)
    set(RenderFiles ${RenderFiles} render/sdl2backend.cpp)
else()
    set(RenderFiles ${RenderFiles} render/sdlbackend.cpp)
endif()

add_library(Render ${RenderFiles})
target_link_libraries(Render Cel Levels ${SDL_LIBRARY})
set_target_properties(Render PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Input
    input/inputmanager.h
    input/sdlcommon.cpp)
set_target_properties(Input PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(DiabloExe
    diabloexe/diabloexe.h
    diabloexe/diabloexe.cpp
    diabloexe/monster.h
    diabloexe/monster.cpp
    diabloexe/npc.h
    diabloexe/npc.cpp)
target_link_libraries(DiabloExe ${Boost_Libraries} Misc)
set_target_properties(DiabloExe PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
