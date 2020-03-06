cmake_minimum_required (VERSION 3.11)

## Windows Config ##
if (WIN32)
    set (CMAKE_SYSTEM_VERSION 8.1 CACHE STRING INTERNAL FORCE)

    set(desired_generators "Visual Studio 16 2019")

    if (NOT "${CMAKE_GENERATOR}" IN_LIST desired_generators)
        message(FATAL_ERROR "Incorrect generator, please run cmake with: -G GENERATOR, where GENERATOR is one of: ${desired_generators}")
    endif()
endif()

project (Freeablo)

set(OpenGL_GL_PREFERENCE LEGACY)
Find_Package(OpenGL REQUIRED)

set(FA_COMPILER_FLAGS "")

set (CMAKE_CXX_STANDARD 17)

if(WIN32)
	add_definitions(-DNOMINMAX) # http://stackoverflow.com/questions/5004858/stdmin-gives-error
endif()

option (FA_TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} -Wall -pedantic -Wextra -Wno-unknown-pragmas -Wno-missing-braces")
    SET(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
    if (FA_TREAT_WARNINGS_AS_ERRORS)
        set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} -Werror")
    endif()
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    SET(GCC_LINKER_FLAGS "-static-libstdc++ -static-libgcc")

    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINKER_FLAGS}")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_LINKER_FLAGS}")
endif()

if(MSVC)
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} /MP /W4 /D_CRT_SECURE_NO_WARNINGS /wd4244 /wd4100 /wd4611 /wd4458 /wd4459 /wd4127 /wd4800 /wd4267 /wd4172")
    # 4244 - e.g. 'argument': conversion from 'const long double' to 'double' -- boost headers
    # 4100 - unreferenced formal parameter -- boost headers
    # 4172 - boost headers
    # 4611 - interaction between '_setjmp' and C++ object destruction is non-portable -- savepng.cpp
    # 4458 - shadow for class member -- librocket headers
    # 4459 - declaration of 'self' hides global declaration
    # 4127 - conditional expression is constant, could be useful to check that both branches compilable.
    # 4267 -  conversion from 'size_t' to 'int', possible loss of data -- too many such warnings in 64-bit build currently
    # 4800 - forcing value to bool, stupid warning
    set(FA_COMPILER_FLAGS "${FA_COMPILER_FLAGS} /w44062")
    # 4061 - not all enumerator values are handled by the switch statement
    add_definitions (-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS )
    # SCL - because current boost is too old and emits this warning
    # CRT - something in nuklear.h
    if (FA_TREAT_WARNINGS_AS_ERRORS)
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
add_subdirectory(test)

if(MSVC)
    set_property(TARGET freeablo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    set_property(TARGET celview PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    set_property(TARGET exedump PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    set_property(TARGET launcher PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    set_property(TARGET unit_tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()

install(DIRECTORY resources/ DESTINATION resources
        PATTERN "settings-user.ini" EXCLUDE)

install(FILES changelog.md DESTINATION .)