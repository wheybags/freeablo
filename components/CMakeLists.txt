add_library(Cel
    cel/celfile.cpp cel/celfile.h
    cel/celframe.h
    cel/pal.cpp cel/pal.h
    cel/celdecoder.cpp cel/celdecoder.h)
target_link_libraries(Cel FAIO Misc)
set_target_properties(Cel PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(FAIO faio/faio.cpp faio/faio.h faio/fafileobject.h faio/fafileobject.cpp)
target_link_libraries(FAIO storm Filesystem)
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
target_link_libraries(Levels FAIO DiabloExe Serial)
set_target_properties(Levels PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Misc
    misc/array2d.h
    misc/assert.h
    misc/direction.cpp
    misc/direction.h
    misc/disablewarn.h
    misc/enablewarn.h
    misc/enum_range.h
    misc/fixedpoint.cpp
    misc/fixedpoint.h
    misc/int128.cpp
    misc/int128.h
    misc/int128_have_intrinsic.inc
    misc/int128_no_intrinsic.inc
    misc/maxcurrentitem.cpp
    misc/maxcurrentitem.h
    misc/md5.cpp
    misc/md5.h
    misc/misc.h
    misc/misc.cpp
    misc/savePNG.cpp
    misc/savePNG.h
    misc/stdhashes.h
    misc/stringops.h
    misc/simplevec2.h
    misc/simplevec2.cpp
    misc/averager.cpp
    misc/averager.h
)
target_link_libraries(Misc PUBLIC Settings png SDL2 Serial Filesystem)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Misc PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Random
    random/random.cpp
    random/random.h
    random/mersennetwister.h
)
target_link_libraries(Random Serial)
set_target_properties(Random PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")


set(RenderFiles
    render/render.h
    render/levelobjects.h
    render/sdl2backend.cpp
    render/sdl_gl_funcs.h
    render/sdl_gl_funcs.cpp
    render/nuklear_sdl_gl3.cpp
    render/nuklear_sdl_gl3.h
    render/misc.h
    render/atlastexture.cpp
    render/atlastexture.h
    ../extern/RectangleBinPack/Rect.cpp
    ../extern/RectangleBinPack/Rect.h
    ../extern/RectangleBinPack/SkylineBinPack.cpp
    ../extern/RectangleBinPack/SkylineBinPack.h
)
add_library(Render ${RenderFiles})
target_link_libraries(Render PUBLIC Cel Levels nuklear SDL2 SDL_image ${OPENGL_LIBRARIES})
set_target_properties(Render PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Input
    input/hotkey.cpp
    input/hotkey.h
    input/inputmanager.h
    input/keys.h
    input/sdl2backend.cpp)
set_target_properties(Input PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(Input nuklear Settings Render SDL2 SPSCQueue)

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
    diabloexe/affix.h
	diabloexe/talkdata.h)
target_link_libraries(DiabloExe Misc FAIO)
set_target_properties(DiabloExe PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Audio
    audio/fa_audio.h
    audio/sdl2backend.cpp
)
target_link_libraries(Audio FAIO SDL_mixer)
set_target_properties(Audio PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Settings
    settings/settings.h
    settings/settings.cpp
)
target_link_libraries(Settings simpleini Filesystem)
set_target_properties(Settings PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Serial
    serial/loader.h
    serial/loader.cpp
    serial/streaminterface.h
    serial/textstream.h
    serial/textstream.cpp
)
target_link_libraries(Serial Misc)
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
