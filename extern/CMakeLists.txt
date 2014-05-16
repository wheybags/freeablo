# stormlib
############################
if(NOT WIN32)
    add_definitions(-D__SYS_ZLIB -D__SYS_BZLIB -D_7ZIP_ST)
    Find_Package ( ZLIB REQUIRED )
    Find_Package ( BZip2 REQUIRED )
    if ( NOT ZLIB_FOUND )
        message ( FATAL_ERROR "ZLIB not found!" )
    endif ( NOT ZLIB_FOUND )

    if ( NOT BZIP2_FOUND )
        message ( FATAL_ERROR "BZip2 not found!" )
    endif ( NOT BZIP2_FOUND )

    include_directories(
        ${ZLIB_INCLUDE_DIRS}
        ${BZIP2_INCLUDE_DIRS}
    )

    link_libraries(
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
    )
    SET(StormFiles ${StormFiles} StormLib/src/lzma/C/LzFind StormLib/src/lzma/C/LzmaEnc StormLib/src/lzma/C/LzmaDec)
else()
    AUX_SOURCE_DIRECTORY(StormLib/src/zlib StormFiles)
    AUX_SOURCE_DIRECTORY(StormLib/src/bzip2 StormFiles)
    AUX_SOURCE_DIRECTORY(StormLib/src/lzma/C StormFiles)
endif()

AUX_SOURCE_DIRECTORY(StormLib/src StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/hashes/ StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/headers/ StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/math/ StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/misc/ StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/asn1 StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/ecc StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/pkcs1 StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtomcrypt/src/pk/rsa StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/libtommath StormFiles)
SET(StormFiles ${StormFiles} StormLib/src/huffman/huff)
AUX_SOURCE_DIRECTORY(StormLib/src/pklib StormFiles)
AUX_SOURCE_DIRECTORY(StormLib/src/sparse StormFiles)
SET(StormFiles ${StormFiles} StormLib/src/adpcm/adpcm)
AUX_SOURCE_DIRECTORY(StormLib/src/jenkins StormFiles)

add_library(StormLib
${StormFiles}
)

target_link_libraries(StormLib ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES})

# ignore warnings in stormlib
if(UNIX)
    set_target_properties(StormLib PROPERTIES COMPILE_FLAGS "-w")
endif()



# libRocket
#############################
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "")
add_subdirectory(libRocket/Build)

set(ROCKET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libRocket/Include" PARENT_SCOPE)

set(LIBRARIES Core Controls Debugger)
foreach(library ${LIBRARIES})
    set(NAME Rocket${library})
    
    set(ROCKET_LIBRARIES ${ROCKET_LIBRARIES} ${NAME} PARENT_SCOPE)

    # ignore warnings in libRocket
    if(UNIX)
        set_target_properties(${NAME} PROPERTIES COMPILE_FLAGS "-w")
    endif()
endforeach()
