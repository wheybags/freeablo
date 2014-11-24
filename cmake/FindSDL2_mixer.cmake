# Locate the SDL2_mixer library. This CMake module is a modified version
# of the original FindSDL_mixer.cmake file
# ###########################################################################
# Locate SDL_mixer library
# This module defines
# SDL2MIXER_LIBRARY, the name of the library to link against
# SDLMIXER_FOUND, if false, do not try to link to SDL
# SDL2MIXER_INCLUDE_DIR, where to find SDL/SDL.h
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake 
# module, but with modifications to recognize OS X frameworks and 
# additional Unix paths (FreeBSD, etc).

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(SDL2MIXER_INCLUDE_DIR SDL_mixer.h
  HINTS
  $ENV{SDL2MIXERDIR}
  $ENV{SDL2DIR}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL2
  /usr/include/SDL2
  /sw/include/SDL2 # Fink
  /opt/local/include/SDL2 # DarwinPorts
  /opt/csw/include/SDL2 # Blastwave
  /opt/include/SDL2
)

FIND_LIBRARY(SDL2MIXER_LIBRARY 
  NAMES SDL2_mixer
  HINTS
  $ENV{SDL2MIXERDIR}
  $ENV{SDL2DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

SET(SDL2MIXER_FOUND "NO")
IF(SDL2MIXER_LIBRARY AND SDL2MIXER_INCLUDE_DIR)
  SET(SDL2MIXER_FOUND "YES")
ENDIF(SDL2MIXER_LIBRARY AND SDL2MIXER_INCLUDE_DIR)

