cmake_minimum_required (VERSION 2.6)

project (Freeablo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

option(USE_SDL2 "use SDL2 instead of SDL1" ON)

if(USE_SDL2)
    message(STATUS "Using SDL2")
    
    Find_Package ( SDL2 REQUIRED )
    set(SDL_FOUND ${SDL2_FOUND})
    set(SDL_LIBRARY ${SDL2_LIBRARY})
    set(SDL_INCLUDE_DIR ${SDL2_INCLUDE_DIR})

    Find_Package(SDL2_image REQUIRED)
    set(SDLIMAGE_FOUND ${SDL2IMAGE_FOUND})
    set(SDLIMAGE_LIBRARY ${SDL2IMAGE_LIBRARY})
    set(SDLIMAGE_INCLUDE_DIR ${SDL2IMAGE_INCLUDE_DIR})


else()
    message(STATUS "Using SDL1")

    Find_Package ( SDL REQUIRED )
    Find_Package (SDL_image REQUIRED)
endif()

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

find_package(Boost COMPONENTS chrono date_time filesystem system random thread program_options atomic REQUIRED)

if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ()

if(UNIX)
    set(FA_COMPILER_FLAGS "-Wall -pedantic -Wextra")
else()
    set(FA_COMPILER_FLAGS "")
endif()

include_directories (
    ${SDL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
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
    ${SDL_LIBRARY}
    ${SDLIMAGE_LIBRARY}
    ${Boost_LIBRARIES}
    Cel
    FAIO
    Levels
    Misc
    Render
    Input
    DiabloExe
    StormLib
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(apps/freeablo)
add_subdirectory(apps/celview)
add_subdirectory(apps/mpqtool)
add_subdirectory(apps/exedump)
