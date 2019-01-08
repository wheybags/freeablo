add_library(Cel
    cel/celfile.cpp cel/celfile.h
    cel/celframe.h
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
    misc/maxcurrentitem.cpp
    misc/maxcurrentitem.h
    misc/md5.cpp
    misc/md5.h
    misc/misc.h
    misc/point.cpp
    misc/point.h
    misc/savePNG.cpp
    misc/savePNG.h
    misc/stdhashes.h
    misc/stringops.h
    misc/vec2fix.cpp
    misc/vec2fix.h
)
target_link_libraries(Misc Settings PNG::png SDL2::SDL2)
SET_TARGET_PROPERTIES(Misc PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Misc PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Random
    random/random.cpp
    random/random.h
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
target_link_libraries(DiabloExe Misc FAIO ${HUNTER_BOOST_LIBS})
set_target_properties(DiabloExe PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Audio
    audio/audio.h
    audio/sdl2backend.cpp
)
target_link_libraries(Audio FAIO SDL_mixer::SDL_mixer)
set_target_properties(Audio PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

if (MSVC)
    # Download and prepare MSYS2 libmpv for MSVC conversion.
	# https://github.com/mpv-player/mpv/blob/master/DOCS/compile-windows.md#linking-libmpv-with-msvc-programs
	set(MPV_MSYS2_VERSION "mpv-dev-20181002")
	set(MPV_MSYS2_URL "https://mpv.srsfckn.biz/${MPV_MSYS2_VERSION}.7z")
	set(MPV_MSYS2_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${MPV_MSYS2_VERSION}")
	set(MPV_MSYS2_7Z_FILE "${MPV_MSYS2_BASE_DIR}.7z")
	
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		# 64 bit
		set(MPV_MSYS2_LIB_DIR "${MPV_MSYS2_BASE_DIR}/x86_64")
		set(MSVC_LIB_MACHINE x64)
	elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
		# 32 bit
		set(MPV_MSYS2_LIB_DIR "${MPV_MSYS2_BASE_DIR}/i686")
		set(MSVC_LIB_MACHINE x86)
	endif()

	if (NOT EXISTS ${MPV_MSYS2_BASE_DIR})
		message("Downloading MSYS2 mpv libraries ${MPV_MSYS2_URL}")
		file(DOWNLOAD ${MPV_MSYS2_URL} ${MPV_MSYS2_7Z_FILE} SHOW_PROGRESS)
		message("Extracting MSYS2 mpv libraries...")
		file(MAKE_DIRECTORY ${MPV_MSYS2_BASE_DIR})
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar xf ${MPV_MSYS2_7Z_FILE} --format=7zip
			WORKING_DIRECTORY ${MPV_MSYS2_BASE_DIR}
			RESULT_VARIABLE res)
		if (res)
			message(FATAL_ERROR "libmpv extraction failed: ${res}")
		endif()

		# Need to add "LIBRARY MPV-1\nEXPORTS\n" to the start of the def file
		# https://github.com/mpv-player/mpv/issues/5007#issuecomment-337431977
		file(READ ${MPV_MSYS2_LIB_DIR}/libmpv.def inital_libmpv_def_file)
		file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/libmpv.def "LIBRARY MPV-1\nEXPORTS\n${inital_libmpv_def_file}" )

		# Copy mpv-1.dll to build dir, this means exe's must be run from this location (otherwise could copy next to exe's or add to path).
		file(COPY ${MPV_MSYS2_LIB_DIR}/mpv-1.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/..)
	endif()

	set(MPV_FOUND TRUE)
	set(MPV_LIBRARIES libmpv)
	set(MPV_INCLUDE_DIRS "${MPV_MSYS2_BASE_DIR}/include")
	set(MPV_DEFINITIONS "")
else (MSVC)
include(FindMPV)
endif(MSVC)

IF (MPV_FOUND)
    add_library(Video
        video/video.h
        video/mpv_sdl_backend.cpp
    )
	if (MSVC)
		# Convert MSYS2 lib to MSVC format, possibly could use built in cmake module GNUtoMS_lib.cmake instead.
		add_custom_command(
			TARGET Video
			PRE_BUILD
			COMMAND lib ARGS /def:${CMAKE_CURRENT_BINARY_DIR}/libmpv.def /name:mpv-1.dll /out:${CMAKE_CURRENT_BINARY_DIR}/libmpv.lib /machine:${MSVC_LIB_MACHINE})
	endif(MSVC)
    target_link_libraries(Video FAIO "${MPV_LIBRARIES}" SDL2::SDL2)
    target_include_directories(Video PRIVATE "${MPV_INCLUDE_DIRS}")
    set_target_properties(Video PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS} ${MPV_DEFINITIONS}")    
ELSE (MPV_FOUND)
    message("MPV not found, using stub implemenation, video disabled")
    add_library(Video
        video/video.h
        video/stubbackend.cpp
    )
    set_target_properties(Video PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
ENDIF (MPV_FOUND)

add_executable(VideoTest
    video/videotest.cpp
)
target_link_libraries(VideoTest Video SDL2::SDL2)
set_target_properties(VideoTest PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Settings
    settings/settings.h
    settings/settings.cpp
)
target_link_libraries(Settings simpleini ${HUNTER_BOOST_LIBS})
set_target_properties(Settings PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

add_library(Serial
    serial/loader.h
    serial/loader.cpp
    serial/streaminterface.h
    serial/textstream.h
    serial/textstream.cpp
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
