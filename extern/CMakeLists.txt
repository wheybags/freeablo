
add_definitions(-D__SYS_ZLIB -D__SYS_BZLIB -D_7ZIP_ST)

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
SET(StormFiles ${StormFiles} StormLib/src/lzma/C/LzFind StormLib/src/lzma/C/LzmaEnc StormLib/src/lzma/C/LzmaDec)
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

