cmake_minimum_required (VERSION 2.6)

project (Freeablo)



Find_Package ( SDL REQUIRED )
Find_Package ( SDL_ttf REQUIRED )

if(WIN32)
	set(Boost_USE_STATIC_LIBS   ON)
endif()
find_package(Boost COMPONENTS filesystem system REQUIRED)



# Workaround for the non-working REQUIRED flag
if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

if ( NOT SDLTTF_FOUND )
    message ( FATAL_ERROR "SDL_ttf not found!" )
endif ( NOT SDLTTF_FOUND )


include_directories (
    ${SDL_INCLUDE_DIR}
    ${SDL_TTF_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
    "${PROJECT_SOURCE_DIR}/components/"
)

add_subdirectory (components) 

link_libraries (
    ${SDL_LIBRARY}
    ${SDLTTF_LIBRARY}
    ${Boost_LIBRARIES}
    Cel
    FAIO
)

SET( FILES
    dun_file.cpp
    min.cpp
    til.cpp
    dun_file.h  
    min.h  
    til.h
)

add_executable(freeablo main.cpp ${FILES})
add_executable(view view.cpp ${FILES})


