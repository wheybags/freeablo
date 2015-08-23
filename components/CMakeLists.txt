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
    level/sol.h
	level/itemmanager.cpp
	level/itemmanager.h
	level/item.cpp
	level/item.h)
target_link_libraries(Levels FAIO DiabloExe)
set_target_properties(Levels PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Misc 
    misc/stringops.h
    misc/helper2d.h
    misc/md5.h
    misc/md5.cpp
    misc/misc.h
    misc/misc.cpp
    misc/disablewarn.h
    misc/enablewarn.h
    misc/savePNG.h
    misc/savePNG.cpp
)
target_link_libraries(Misc ${PNG_LIBRARY})
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Misc PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")


set(RenderFiles 
    render/render.h 
    render/levelobjects.cpp 
    render/levelobjects.h
    render/rocketglue/RenderInterfaceSDL2.cpp  
    render/rocketglue/RenderInterfaceSDL2.h 
    render/rocketglue/SystemInterfaceSDL2.cpp
    render/rocketglue/SystemInterfaceSDL2.h
    render/rocketglue/FAIOFileInterface.h
    render/rocketglue/FAIOFileInterface.cpp
    render/rocketglue/drawcommand.h
    render/sdl2backend.cpp
)
add_library(Render ${RenderFiles})
target_link_libraries(Render Cel Levels ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${OPENGL_LIBRARIES} ${ROCKET_LIBRARIES} ${PYTHON_LIBRARIES})
set_target_properties(Render PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Input
    input/common.cpp
    input/common.h
    input/hotkey.cpp
    input/hotkey.h
    input/inputmanager.h
    input/keys.h
    input/sdl2backend.cpp)
set_target_properties(Input PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(DiabloExe
    diabloexe/diabloexe.h
    diabloexe/diabloexe.cpp
    diabloexe/monster.h
    diabloexe/monster.cpp
    diabloexe/npc.h
    diabloexe/npc.cpp
    diabloexe/baseitem.cpp
    diabloexe/baseitem.h
	diabloexe/uniqueitem.cpp
	diabloexe/uniqueitem.h
    diabloexe/prefix.cpp
    diabloexe/prefix.h)
target_link_libraries(DiabloExe ${Boost_Libraries} Misc)
set_target_properties(DiabloExe PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Audio
    audio/audio.h
    audio/sdl2backend.cpp
)
target_link_libraries(Audio ${SDL2MIXER_LIBRARY})
set_target_properties(Audio PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Settings
    settings/settings.h
    settings/settings.cpp
)
target_link_libraries(Settings ${Boost_Libraries})
set_target_properties(Settings PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Script
    script/scriptcontext.h
    script/scriptcontext.cpp
    script/commandtranslator.h
    script/commandtranslator.cpp
)
target_link_libraries(Script ${Boost_Libraries}  ${PYTHON_LIBRARIES})
set_target_properties(Script PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
