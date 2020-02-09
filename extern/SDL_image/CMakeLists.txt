CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(SDL_image VERSION 2.0.1)

set(ENABLE_DEFS "")
set(LINK_LIBS ${SDL2_LIBRARY})
set(INCLUDE_DIRS ${SDL2_INCLUDE_DIR})

option(ENABLE_BMP "ENABLE_BMP" ON)
option(ENABLE_GIF "ENABLE_GIF" ON)
option(ENABLE_JPG "ENABLE_JPG" OFF)
option(ENABLE_LBM "ENABLE_LBM" ON)
option(ENABLE_PCX "ENABLE_PCX" ON)
option(ENABLE_PNG "ENABLE_PNG" ON)
option(ENABLE_PNM "ENABLE_PNM" ON)
option(ENABLE_TGA "ENABLE_TGA" ON)
option(ENABLE_TIF "ENABLE_TIF" OFF)
option(ENABLE_WEBP "ENABLE_WEBP" OFF) # BROKEN
option(ENABLE_XCF "ENABLE_XCF" ON)
option(ENABLE_XPM "ENABLE_XPM" ON)
option(ENABLE_XV "ENABLE_XV" ON)

if(ENABLE_BMP)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_BMP)
endif()

if(ENABLE_GIF)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_GIF)
endif()

if(ENABLE_JPG)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_JPG)
    find_package(JPEG REQUIRED)
    set(LINK_LIBS "${LINK_LIBS}" "${JPEG_LIBRARIES}")
    set(INCLUDE_DIRS "${INCLUDE_DIRS}" "${JPEG_INCLUDE_DIRS}")
endif()

if(ENABLE_LBM)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_LBM)
endif()

if(ENABLE_PCX)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_PCX)
endif()

if(ENABLE_PNG)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_PNG)
    #find_package(PNG REQUIRED)
    #set(LINK_LIBS "${LINK_LIBS}" "${PNG_LIBRARIES}")
    #set(INCLUDE_DIRS "${INCLUDE_DIRS}" "${PNG_INCLUDE_DIRS}")
    set(LINK_LIBS "${LINK_LIBS}" png)
endif()

if(ENABLE_PNM)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_PNM)
endif()

if(ENABLE_TGA)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_TGA)
endif()

if(ENABLE_TIF)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_TIF)
    find_package(TIFF REQUIRED)
    set(LINK_LIBS "${LINK_LIBS}" "${TIFF_LIBRARIES}")
    set(INCLUDE_DIRS "${INCLUDE_DIRS}" "${TIFF_INCLUDE_DIRS}")
endif()

if(ENABLE_WEBP)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_WEBP)
    find_package(WebP REQUIRED) # BROKEN, debian doesn't install the cmake config files, so find_package will fail
    set(LINK_LIBS "${LINK_LIBS}" "${WebP_LIBRARIES}")
    set(INCLUDE_DIRS "${INCLUDE_DIRS}" "${WebP_INCLUDE_DIRS}")
endif()

if(ENABLE_XCF)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_XCF)
endif()

if(ENABLE_XV)
    set(ENABLE_DEFS "${ENABLE_DEFS}" LOAD_XV)
endif()

set(SRC_C_FILES
    IMG.c
    IMG_bmp.c
    IMG_gif.c
    IMG_jpg.c
    IMG_lbm.c
    IMG_pcx.c
    IMG_png.c
    IMG_pnm.c
    IMG_tga.c
    IMG_tif.c
    IMG_webp.c
    IMG_xcf.c
    IMG_xpm.c
    IMG_xv.c
    IMG_xxx.c
)

set(SRC_H_FILES  
    SDL_image.h
    miniz.h
)

set(SRC_FILES ${SRC_C_FILES} ${SRC_H_FILES})

add_library(SDL_image ${SRC_FILES})


target_link_libraries(SDL_image PUBLIC ${LINK_LIBS} SDL2)
include_directories(${INCLUDE_DIRS})
target_include_directories(SDL_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(SDL_image PUBLIC "${ENABLE_DEFS}" SDL_IMAGE_USE_COMMON_BACKEND)
