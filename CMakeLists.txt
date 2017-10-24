cmake_minimum_required (VERSION 2.8.11)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.131.tar.gz"
    SHA1 "bc0eec34712af8854085c2dc6a90b78df17a9743"
    LOCAL
)

project (Freeablo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(HUNTER_LIBS "")

hunter_add_package(SDL2)
find_Package(SDL2 CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} SDL2::SDL2)

hunter_add_package(SDL_image)
find_package(SDL_image CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} SDL_image::SDL_image)

hunter_add_package(SDL_mixer)
find_package(SDL_mixer CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} SDL_mixer::SDL_mixer)

hunter_add_package(PNG)
find_package(PNG CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} PNG::png)

hunter_add_package(enet)
find_package(enet CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} enet::enet)

hunter_add_package(ZLIB)
find_package(ZLIB CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} ZLIB::zlib)

hunter_add_package(stormlib)
find_package(stormlib CONFIG REQUIRED)
set(HUNTER_LIBS ${HUNTER_LIBS} stormlib::stormlib)

hunter_add_package(Boost COMPONENTS filesystem serialization system program_options date_time regex)
find_package(Boost CONFIG REQUIRED  filesystem serialization system program_options date_time regex)
set(HUNTER_BOOST_LIBS Boost::filesystem Boost::serialization Boost::system Boost::program_options Boost::date_time Boost::regex)
set(HUNTER_LIBS ${HUNTER_LIBS} ${HUNTER_BOOST_LIBS})

Find_Package(OpenGL REQUIRED)

set(FA_COMPILER_FLAGS "")

# Use C++11
if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()
if(WIN32)
	add_definitions(-DNOMINMAX) # http://stackoverflow.com/questions/5004858/stdmin-gives-error
endif()

option (TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors")

if(UNIX)
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} -Wall -pedantic -Wextra -Wno-unknown-pragmas")
    SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
    if (TREAT_WARNINGS_AS_ERRORS)
        set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} -Werror")
    endif()
endif()

if(MSVC)
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} /W4 /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4100 /wd4611 /wd4458 /wd4459 /wd4127 /wd4800")
    # 4244 - e.g. 'argument': conversion from 'const long double' to 'double' -- boost headers
    # 4100 - unreferenced formal parameter -- boost headers
    # 4611 - interaction between '_setjmp' and C++ object destruction is non-portable -- savepng.cpp
    # 4458 - shadow for class member -- librocket headers
    # 4459 - declaration of 'self' hides global declaration
    # 4127 - conditional expression is constant, could be useful to check that both branches compilable.
    # 4800 - forcing value to bool, stupid warning
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} /w44062")
    # 4061 - not all enumerator values are handled by the switch statement
    if (TREAT_WARNINGS_AS_ERRORS)
        set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} /WX")
    endif()
endif()

#TODO: get rid of this?
include_directories("${PROJECT_SOURCE_DIR}/components/")

# allow the user to specify extra include dirs and defines on the command line
if(CLI_INCLUDE_DIRS)
    include_directories(${CLI_INCLUDE_DIRS})
endif()
if(CLI_DEFINES)
	add_definitions(${CLI_DEFINES})
endif()


add_subdirectory (extern)
add_subdirectory (components)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(apps/freeablo)
add_subdirectory(apps/celview)
add_subdirectory(apps/mpqtool)
add_subdirectory(apps/exedump)
add_subdirectory(apps/launcher)
add_subdirectory(apps/fontgenerator)
add_subdirectory(apps/findpath)
add_subdirectory(test)
