cmake_minimum_required (VERSION 2.6)

project (Freeablo)



Find_Package ( SDL REQUIRED )
Find_Package ( SDL_ttf REQUIRED )
Find_Package ( ZLIB REQUIRED )
Find_Package ( BZip2 REQUIRED )


if(WIN32)
	set(Boost_USE_STATIC_LIBS   ON)
endif()
find_package(Boost COMPONENTS chrono date_time filesystem system random thread REQUIRED)



if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

if ( NOT SDLTTF_FOUND )
    message ( FATAL_ERROR "SDL_ttf not found!" )
endif ( NOT SDLTTF_FOUND )

if ( NOT ZLIB_FOUND )
    message ( FATAL_ERROR "ZLIB not found!" )
endif ( NOT ZLIB_FOUND )

if ( NOT BZIP2_FOUND )
    message ( FATAL_ERROR "BZip2 not found!" )
endif ( NOT BZIP2_FOUND )


include_directories (
    ${SDL_INCLUDE_DIR}
    ${SDL_TTF_INCLUDE_DIRS}
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
    ${SDLTTF_LIBRARY}
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    Cel
    FAIO
    Levels
    Misc
    Render
    Input
    StormLib
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(apps/freeablo)
add_subdirectory(apps/celview)
add_subdirectory(apps/mpqtool)
