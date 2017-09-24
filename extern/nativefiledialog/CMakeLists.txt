set(LIB nativefiledialog)
set(SRCS src/nfd_common.c src/include/nfd.h)

set(EXTRA_LIBRARIES)

if (APPLE)
    set(SRCS ${SRCS} src/nfd_cocoa.mm)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} AppKit)
endif()

if (WIN32)
    set(SRCS ${SRCS} src/nfd_win.cpp)
endif()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(SRCS ${SRCS} src/nfd_zenity.c)
endif()

add_library(${LIB} ${SRCS})

if (EXTRA_LIBRARIES)
    target_link_libraries(${LIB} ${EXTRA_LIBRARIES})
endif()

target_include_directories(${LIB} PUBLIC src/include)
