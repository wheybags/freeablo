# stormlib
############################
#if(NOT WIN32)
#    add_definitions(-D__SYS_ZLIB -D__SYS_BZLIB -D_7ZIP_ST)
#    Find_Package ( ZLIB REQUIRED )
#    Find_Package ( BZip2 REQUIRED )
#    if ( NOT ZLIB_FOUND )
#        message ( FATAL_ERROR "ZLIB not found!" )
#    endif ( NOT ZLIB_FOUND )
#
#    if ( NOT BZIP2_FOUND )
#        message ( FATAL_ERROR "BZip2 not found!" )
#    endif ( NOT BZIP2_FOUND )
#
#    include_directories(
#        ${ZLIB_INCLUDE_DIRS}
#        ${BZIP2_INCLUDE_DIRS}
#    )
#
#    link_libraries(
#        ${ZLIB_LIBRARIES}
#        ${BZIP2_LIBRARIES}
#    )
#    SET(StormFiles ${StormFiles} StormLib/src/lzma/C/LzFind StormLib/src/lzma/C/LzmaEnc StormLib/src/lzma/C/LzmaDec)
#else()
#    AUX_SOURCE_DIRECTORY(StormLib/src/zlib StormFiles)
#    AUX_SOURCE_DIRECTORY(StormLib/src/bzip2 StormFiles)
#    AUX_SOURCE_DIRECTORY(StormLib/src/lzma/C StormFiles)
#endif()
#
#AUX_SOURCE_DIRECTORY(StormLib/src StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/hashes/ StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/headers/ StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/math/ StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/misc/ StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/asn1 StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/ecc StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/pkcs1 StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/rsa StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/libtommath StormFiles)
#SET(StormFiles ${StormFiles} StormLib/src/huffman/huff)
#AUX_SOURCE_DIRECTORY(StormLib/src/pklib StormFiles)
#AUX_SOURCE_DIRECTORY(StormLib/src/sparse StormFiles)
#SET(StormFiles ${StormFiles} StormLib/src/adpcm/adpcm)
#AUX_SOURCE_DIRECTORY(StormLib/src/jenkins StormFiles)
#
#add_library(StormLib
#${StormFiles}
#)
#
#target_link_libraries(StormLib ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES})
#
## ignore warnings in stormlib
#if(UNIX)
#    set_target_properties(StormLib PROPERTIES COMPILE_FLAGS "-w")
#endif()

# nuklear
###################
add_subdirectory(nuklear)

# nativefiledialog
###################
set(LIB nativefiledialog)
set(SRCS nativefiledialog/src/nfd_common.c nativefiledialog/src/include/nfd.h)

set(EXTRA_LIBRARIES)

if (APPLE)
    set(SRCS ${SRCS} nativefiledialog/src/nfd_cocoa.mm)
    set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} AppKit)
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
