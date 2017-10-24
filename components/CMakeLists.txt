add_library(Cel 
    cel/celfile.cpp cel/celfile.h  
    cel/celframe.h cel/celframe.cpp
    cel/pal.cpp cel/pal.h  
    cel/celdecoder.cpp cel/celdecoder.h)
target_link_libraries(Cel FAIO)
set_target_properties(Cel PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(FAIO faio/faio.cpp faio/faio.h faio/fafileobject.h faio/fafileobject.cpp)
target_link_libraries(FAIO stormlib::stormlib ${HUNTER_BOOST_LIBS})
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
    level/baseitemmanager.h
    level/baseproperty.h)
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
    misc/stdhashes.h
)
target_link_libraries(Misc Settings PNG::png SDL2::SDL2)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Misc PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")


set(RenderFiles 
    render/render.h 
    render/levelobjects.cpp 
    render/levelobjects.h
    render/sdl2backend.cpp
    render/sdl_gl_funcs.h
    render/sdl_gl_funcs.cpp
    render/nuklear_sdl_gl3.cpp
    render/nuklear_sdl_gl3.h
    render/misc.h
)
add_library(Render ${RenderFiles})
target_link_libraries(Render PUBLIC Cel Levels nuklear SDL2::SDL2 SDL_image::SDL_image ${OPENGL_LIBRARIES})
set_target_properties(Render PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Input
    input/hotkey.cpp
    input/hotkey.h
    input/inputmanager.h
    input/keys.h
    input/sdl2backend.cpp)
set_target_properties(Input PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(Input nuklear Settings Render SDL2::SDL2)

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
    diabloexe/characterstats.h
    diabloexe/characterstats.cpp
    diabloexe/affix.cpp
    diabloexe/affix.h)
target_link_libraries(DiabloExe Misc FAIO ${HUNTER_BOST_LIBS})
set_target_properties(DiabloExe PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Audio
    audio/audio.h
    audio/sdl2backend.cpp
)
target_link_libraries(Audio FAIO SDL_mixer::SDL_mixer)
set_target_properties(Audio PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Settings
    settings/settings.h
    settings/settings.cpp
)
target_link_libraries(Settings ${HUNTER_BOOST_LIBS})
set_target_properties(Settings PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(StateMachine
    statemachine/statemachine.h
    statemachine/statemachine.cpp
)
target_link_libraries(StateMachine ${HUNTER_BOOST_LIBS})
set_target_properties(StateMachine PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Serial
    serial/bitstream.h
    serial/bitstream.inl
    serial/bitstream.cpp
    serial/bithackmacros.h
)
set_target_properties(Serial PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(NuklearMisc
    nuklearmisc/inputfwd.cpp
    nuklearmisc/inputfwd.h
    nuklearmisc/widgets.cpp
    nuklearmisc/widgets.h
    nuklearmisc/standaloneguispritehandler.cpp
    nuklearmisc/standaloneguispritehandler.h
)
set_target_properties(NuklearMisc PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(NuklearMisc nuklear nativefiledialog Input Render)
