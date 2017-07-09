cmake_minimum_required (VERSION 2.8.11)

project (Freeablo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "/usr/local/lib/libtomcrypt.a")
    set(FREETYPE_INCLUDE_DIRS "/usr/local/Cellar/freetype/2.5.3_1/include/freetype2")
endif()

Find_Package(SDL2 REQUIRED)
Find_Package(SDL2_image REQUIRED)
Find_Package(SDL2_mixer REQUIRED)
Find_Package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
Find_Package(OpenGL REQUIRED)

Find_Package(PythonLibs 2 REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Enet REQUIRED)

if (Qt5_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(FA_COMPILER_FLAGS "")

if(NOT WIN32)
    Find_Package ( ZLIB REQUIRED )
    Find_Package ( BZip2 REQUIRED )

    if ( NOT ZLIB_FOUND )
        message ( FATAL_ERROR "ZLIB not found!" )
    endif ()
    if ( NOT BZIP2_FOUND )
        message ( FATAL_ERROR "BZip2 not found!" )
    endif ()
else()
	add_definitions(-DNOMINMAX) # http://stackoverflow.com/questions/5004858/stdmin-gives-error
endif()

find_package(Boost COMPONENTS filesystem serialization system program_options python date_time regex REQUIRED)

if ( NOT SDL2_FOUND )
    message ( FATAL_ERROR "SDL2 not found!" )
endif ()

set (CMAKE_CXX_STANDARD 11)

# Use C++11
if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()

if(UNIX)
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} -Wall -pedantic -Wextra -Wno-unknown-pragmas")
    SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif()

if(MSVC)
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} /W4 /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4100 /wd4611 /wd4458 /wd4459 /wd4127")
    # 4244 - e.g. 'argument': conversion from 'const long double' to 'double' -- boost headers
    # 4100 - unreferenced formal parameter -- boost headers
    # 4611 - interaction between '_setjmp' and C++ object destruction is non-portable -- savepng.cpp
    # 4458 - shadow for class member -- librocket headers
    # 4459 - declaration of 'self' hides global declaration
    # 4127 - conditional expression is constant, could be useful to check that both branches compilable.
endif()

include_directories ( SYSTEM
    ${Boost_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${SDL2IMAGE_INCLUDE_DIR}
    ${SDL2MIXER_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${ENET_INCLUDE_DIR}
    "${PROJECT_SOURCE_DIR}/extern/StormLib/src/"
)

include_directories("${PROJECT_SOURCE_DIR}/components/")


# allow the user to specify extra include dirs and defines on the command line
if(CLI_INCLUDE_DIRS)
    include_directories(${CLI_INCLUDE_DIRS})
endif()
if(CLI_DEFINES)
	add_definitions(${CLI_DEFINES})
endif()


add_subdirectory (extern)

link_libraries(StormLib)

add_subdirectory (components)

link_libraries (
    ${SDL2_LIBRARY}
    ${SDL2IMAGE_LIBRARY}
    ${SDL2MIXER_LIBRARY}
    ${PNG_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${Boost_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${ENET_LIBRARY}
    Cel
    FAIO
    Levels
    Misc
    Render
    Input
    DiabloExe
    StormLib
    Audio
    Settings
    Script
    Serial
    nuklear
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(apps/freeablo)
add_subdirectory(apps/celview)
add_subdirectory(apps/mpqtool)
add_subdirectory(apps/exedump)
add_subdirectory(apps/launcher)
add_subdirectory(apps/fontgenerator)
add_subdirectory(apps/findpath)
add_subdirectory(test)
