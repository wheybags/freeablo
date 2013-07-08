cmake_minimum_required (VERSION 2.6)

project (Freeablo)



Find_Package ( SDL REQUIRED )
Find_Package ( SDL_ttf REQUIRED )

# Workaround for the non-working REQUIRED flag
if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

if ( NOT SDLTTF_FOUND )
    message ( FATAL_ERROR "SDL_ttf not found!" )
endif ( NOT SDLTTF_FOUND )


include_directories ("${PROJECT_SOURCE_DIR}/components/cel")
add_subdirectory (components) 

link_libraries (
   ${SDL_LIBRARY}
   ${SDLTTF_LIBRARY}
   SDLmain
)

add_executable(freeablo dun_file.cpp  main.cpp  min.cpp  til.cpp)
target_link_libraries(freeablo Cel)

