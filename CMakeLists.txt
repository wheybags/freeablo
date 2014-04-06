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
else()
    message(STATUS "Using SDL1")

    Find_Package ( SDL REQUIRED )
endif()


Find_Package ( ZLIB REQUIRED )
Find_Package ( BZip2 REQUIRED )


if(WIN32)
	set(Boost_USE_STATIC_LIBS   ON)
endif()
find_package(Boost COMPONENTS chrono date_time filesystem system random thread program_options REQUIRED)



if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ()

if ( NOT ZLIB_FOUND )
    message ( FATAL_ERROR "ZLIB not found!" )
endif ()

if ( NOT BZIP2_FOUND )
    message ( FATAL_ERROR "BZip2 not found!" )
endif ()

if(UNIX)
    set(FA_COMPILER_FLAGS "-Wall -pedantic -Wextra")
else()
    set(FA_COMPILER_FLAGS "")
endif()


include_directories (
    ${SDL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${BZIP2_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/components/"
    "${PROJECT_SOURCE_DIR}/extern/StormLib/src/"
)

add_subdirectory (components) 
add_subdirectory (extern) 

link_libraries (
    ${SDL_LIBRARY}
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
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
