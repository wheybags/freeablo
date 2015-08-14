#.rst:
# FindFFMPEG
# ----------
#
# Find the native FFMPEG includes and library
#
# This module defines::
#
#  FFMPEG_INCLUDE_DIR, where to find avcodec.h, avformat.h ...
#  FFMPEG_LIBRARIES, the libraries to link against to use FFMPEG.
#  FFMPEG_FOUND, If false, do not try to use FFMPEG.
#
# also defined, but not for general use are::
#
#   FFMPEG_avformat_LIBRARY, where to find the FFMPEG avformat library.
#   FFMPEG_avcodec_LIBRARY, where to find the FFMPEG avcodec library.
#
# This is useful to do it this way so that we can always add more libraries
# if needed to ``FFMPEG_LIBRARIES`` if ffmpeg ever changes...

#=============================================================================
# Copyright: 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)

# Originally from VTK project


find_path(FFMPEG_INCLUDE_DIR1 avformat.h
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/ffmpeg
  $ENV{FFMPEG_DIR}/libavformat
  $ENV{FFMPEG_DIR}/include/libavformat
  $ENV{FFMPEG_DIR}/include/ffmpeg
  /usr/local/include/ffmpeg
  /usr/include/ffmpeg
  /usr/include/libavformat
  /usr/include/ffmpeg/libavformat
  /usr/local/include/libavformat
)

find_path(FFMPEG_INCLUDE_DIR2 avutil.h
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/ffmpeg
  $ENV{FFMPEG_DIR}/libavutil
  $ENV{FFMPEG_DIR}/include/libavutil
  $ENV{FFMPEG_DIR}/include/ffmpeg
  /usr/local/include/ffmpeg
  /usr/include/ffmpeg
  /usr/include/libavutil
  /usr/include/ffmpeg/libavutil
  /usr/local/include/libavutil
)

find_path(FFMPEG_INCLUDE_DIR3 avcodec.h
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/ffmpeg
  $ENV{FFMPEG_DIR}/libavcodec
  $ENV{FFMPEG_DIR}/include/libavcodec
  $ENV{FFMPEG_DIR}/include/ffmpeg
  /usr/local/include/ffmpeg
  /usr/include/ffmpeg
  /usr/include/libavcodec
  /usr/include/ffmpeg/libavcodec
  /usr/local/include/libavcodec
)

find_path(FFMPEG_INCLUDE_DIR4 swscale.h
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/ffmpeg
  $ENV{FFMPEG_DIR}/libswscale
  $ENV{FFMPEG_DIR}/include/libswscale
  $ENV{FFMPEG_DIR}/include/ffmpeg
  /usr/local/include/ffmpeg
  /usr/include/ffmpeg
  /usr/include/libswscale
  /usr/include/ffmpeg/libswscale
  /usr/local/include/libswscale
)

find_path(FFMPEG_INCLUDE_DIR5 avdevice.h
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/ffmpeg
  $ENV{FFMPEG_DIR}/libavdevice
  $ENV{FFMPEG_DIR}/include/libavdevice
  $ENV{FFMPEG_DIR}/include/ffmpeg
  /usr/local/include/ffmpeg
  /usr/include/ffmpeg
  /usr/include/libavdevice
  /usr/include/ffmpeg/libavdevice
  /usr/local/include/libavdevice
)

if(FFMPEG_INCLUDE_DIR1)
  if(FFMPEG_INCLUDE_DIR2)
    if(FFMPEG_INCLUDE_DIR3)
      set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR1}
                             ${FFMPEG_INCLUDE_DIR2}
                             ${FFMPEG_INCLUDE_DIR3})
    endif()
  endif()
endif()

if(FFMPEG_INCLUDE_DIR4)
  set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         ${FFMPEG_INCLUDE_DIR4})
endif()

if(FFMPEG_INCLUDE_DIR5)
  set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         ${FFMPEG_INCLUDE_DIR5}
                         ${FFMPEG_INCLUDE_DIR5}/..)
endif()

find_library(FFMPEG_avformat_LIBRARY avformat
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
  $ENV{FFMPEG_DIR}/libavformat
  /usr/local/lib
  /usr/lib
)

find_library(FFMPEG_avcodec_LIBRARY avcodec
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
  $ENV{FFMPEG_DIR}/libavcodec
  /usr/local/lib
  /usr/lib
)

find_library(FFMPEG_avutil_LIBRARY avutil
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
  $ENV{FFMPEG_DIR}/libavutil
  /usr/local/lib
  /usr/lib
)

if(NOT DISABLE_SWSCALE)
  find_library(FFMPEG_swscale_LIBRARY swscale
    $ENV{FFMPEG_DIR}
    $ENV{FFMPEG_DIR}/lib
    $ENV{FFMPEG_DIR}/libswscale
    /usr/local/lib
    /usr/lib
  )
endif(NOT DISABLE_SWSCALE)

find_library(FFMPEG_avdevice_LIBRARY avdevice
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
  $ENV{FFMPEG_DIR}/libavdevice
  /usr/local/lib
  /usr/lib
)

find_library(_FFMPEG_z_LIBRARY_ z
  $ENV{FFMPEG_DIR}
  $ENV{FFMPEG_DIR}/lib
  /usr/local/lib
  /usr/lib
)



if(FFMPEG_INCLUDE_DIR)
  if(FFMPEG_avformat_LIBRARY)
    if(FFMPEG_avcodec_LIBRARY)
      if(FFMPEG_avutil_LIBRARY)
        set(FFMPEG_FOUND "YES")
        set(FFMPEG_LIBRARIES ${FFMPEG_avformat_LIBRARY}
                             ${FFMPEG_avcodec_LIBRARY}
                             ${FFMPEG_avutil_LIBRARY}
          )
        if(FFMPEG_swscale_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_swscale_LIBRARY}
          )
        endif()
        if(FFMPEG_avdevice_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_avdevice_LIBRARY}
          )
        endif()
        if(_FFMPEG_z_LIBRARY_)
          set( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                                ${_FFMPEG_z_LIBRARY_}
          )
        endif()
      endif()
    endif()
  endif()
endif()

mark_as_advanced(
  FFMPEG_INCLUDE_DIR
  FFMPEG_INCLUDE_DIR1
  FFMPEG_INCLUDE_DIR2
  FFMPEG_INCLUDE_DIR3
  FFMPEG_INCLUDE_DIR4
  FFMPEG_INCLUDE_DIR5
  FFMPEG_avformat_LIBRARY
  FFMPEG_avcodec_LIBRARY
  FFMPEG_avutil_LIBRARY
  FFMPEG_swscale_LIBRARY
  FFMPEG_avdevice_LIBRARY
  _FFMPEG_z_LIBRARY_
  )

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
  set_package_properties(FFMPEG PROPERTIES DESCRIPTION "A complete, cross-platform solution to record, convert and stream audio and video")
  set_package_properties(FFMPEG PROPERTIES URL "http://ffmpeg.org/")
endif()
