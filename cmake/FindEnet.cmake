FIND_PATH(ENET_INCLUDE_DIR enet/enet.h
  HINTS
  $ENV{ENETDIR}
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

FIND_LIBRARY(ENET_LIBRARY  
  NAMES enet
  HINTS
  $ENV{ENETDIR}
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

IF(WIN32)
	set(ENET_LIBRARY "${ENET_LIBRARY}" "WINMM.LIB" "Ws2_32.lib") # needs some libs on windows apparently
ENDIF()

SET(ENET_FOUND "NO")
IF(ENET_LIBRARY AND ENET_INCLUDE_DIR)
  SET(ENET_FOUND "YES")
ENDIF(ENET_LIBRARY AND ENET_INCLUDE_DIR)
