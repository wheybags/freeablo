cmake_minimum_required (VERSION 2.6)

project (Freeablo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "/usr/local/lib/libtomcrypt.a")
    set(FREETYPE_INCLUDE_DIRS "/usr/local/Cellar/freetype/2.5.3_1/include/freetype2")
endif()

Find_Package(SDL2 REQUIRED)
Find_Package(SDL2_image REQUIRED)
Find_Package(SDL2_mixer REQUIRED)
Find_Package(OpenGL REQUIRED)

Find_Package(PythonLibs 2 REQUIRED)
find_package(LibRocket REQUIRED)

if(WIN32)
    set(Boost_USE_STATIC_LIBS   ON)
else()
    Find_Package ( ZLIB REQUIRED )
    Find_Package ( BZip2 REQUIRED )
    
    if ( NOT ZLIB_FOUND )
        message ( FATAL_ERROR "ZLIB not found!" )
    endif ()
    if ( NOT BZIP2_FOUND )
        message ( FATAL_ERROR "BZip2 not found!" )
    endif ()
endif()

find_package(Boost COMPONENTS chrono date_time filesystem system random thread program_options atomic python REQUIRED)

if ( NOT SDL2_FOUND )
    message ( FATAL_ERROR "SDL2 not found!" )
endif ()

if(UNIX)
    set(FA_COMPILER_FLAGS "-Wall -pedantic -Wextra")
else()
    set(FA_COMPILER_FLAGS "")
endif()

include_directories (
    ${SDL2_INCLUDE_DIR}
    ${SDL2IMAGE_INCLUDE_DIR}
    ${SDL2MIXER_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${ROCKET_INCLUDE_DIR}
    "${PROJECT_SOURCE_DIR}/components/"
    "${PROJECT_SOURCE_DIR}/extern/StormLib/src/"
)

# allow the user to specify extra include dirs on the command line
if(CLI_INCLUDE_DIRS)
    include_directories(${CLI_INCLUDE_DIRS})
endif()

add_subdirectory (extern) 

link_libraries(StormLib)

add_subdirectory (components) 

link_libraries (
    ${SDL2_LIBRARY}
    ${SDL2IMAGE_LIBRARY}
    ${SDL2MIXER_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${ROCKET_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${Boost_LIBRARIES}
    Cel
    FAIO
    Levels
    Misc
    Render
    Input
    DiabloExe
    StormLib
    Audio
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(apps/freeablo)
add_subdirectory(apps/celview)
add_subdirectory(apps/mpqtool)
add_subdirectory(apps/exedump)
