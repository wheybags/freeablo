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
    level/level.cpp)
target_link_libraries(Levels FAIO)

add_library(Misc 
    misc/stringops.h
    misc/helper2d.h
    misc/md5.h
    misc/md5.c
    misc/fareadini.h
    misc/fareadini.cpp)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)

add_library(Render
    render/render.h
    render/sdlbackend.cpp)
target_link_libraries(Render Cel Levels ${SDL_LIBRARY})

add_library(Input
    input/inputmanager.h
    input/sdlbackend.cpp)
