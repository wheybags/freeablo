set(LIB nativefiledialog)
set(SRCS src/nfd_common.c src/include/nfd.h)

set(EXTRA_LIBRARIES)

if (APPLE)
    set(SRCS ${SRCS} src/nfd_cocoa.m)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} "-framework AppKit")
endif()

if (WIN32)
    set(SRCS ${SRCS} src/nfd_win.cpp)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux" OR "${CMAKE_SYSTEM_NAME}" MATCHES "OpenBSD")
    set(SRCS ${SRCS} src/nfd_zenity.c)
endif()

add_library(${LIB} ${SRCS})

if (EXTRA_LIBRARIES)
    target_link_libraries(${LIB} ${EXTRA_LIBRARIES})
endif()

target_include_directories(${LIB} PUBLIC src/include)

#if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set_target_properties(${LIB} PROPERTIES COMPILE_FLAGS "-std=gnu99")
    endif()
#else()
#    set_property(TARGET ${LIB} PROPERTY C_STANDARD 99)
#endif()


