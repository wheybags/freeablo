# nuklear
###################
add_subdirectory(nuklear)

# nativefiledialog
###################
set(LIB nativefiledialog)
set(SRCS nativefiledialog/src/nfd_common.c nativefiledialog/src/include/nfd.h)

set(EXTRA_LIBRARIES)

if (APPLE)
    set(SRCS ${SRCS} nativefiledialog/src/nfd_cocoa.m)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} "-framework AppKit")
endif()

if (WIN32)
    set(SRCS ${SRCS} nativefiledialog/src/nfd_win.cpp)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux" OR "${CMAKE_SYSTEM_NAME}" MATCHES "OpenBSD")
    set(SRCS ${SRCS} nativefiledialog/src/nfd_zenity.c)
endif()

add_library(${LIB} ${SRCS})

if (EXTRA_LIBRARIES)
    target_link_libraries(${LIB} ${EXTRA_LIBRARIES})
endif()

target_include_directories(${LIB} PUBLIC nativefiledialog/src/include)

#if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set_target_properties(${LIB} PROPERTIES COMPILE_FLAGS "-std=gnu99")
    endif()
#else()
#    set_property(TARGET ${LIB} PROPERTY C_STANDARD 99)
#endif()

# simpleini
###################
set(LIB simpleini)
add_library(${LIB} INTERFACE)
target_include_directories(${LIB} INTERFACE simpleini/)

add_subdirectory(filesystem)
add_subdirectory(cxxopts)
add_subdirectory("optional-lite")
add_subdirectory(fmt)
add_subdirectory(SPSCQueue)
