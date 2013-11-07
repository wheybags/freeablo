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

add_subdirectory (components) 

include_directories (
    "${PROJECT_SOURCE_DIR}/components/cel"

    ${SDL_INCLUDE_DIR}
    ${SDL_TTF_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
)

link_libraries (
   ${SDL_LIBRARY}
   ${SDLTTF_LIBRARY}
   ${Boost_LIBRARIES}
)

add_executable(freeablo dun_file.cpp  main.cpp  min.cpp  til.cpp)
target_link_libraries(freeablo Cel)

