FIND_PATH(ROCKET_INCLUDE_DIR Rocket/Core.h
  HINTS
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)


FIND_LIBRARY(ROCKET_CONTROLS
  RocketControls
  HINTS
  PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
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

FIND_LIBRARY(ROCKET_CORE
  RocketCore
  HINTS
  PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
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

FIND_LIBRARY(ROCKET_DEBUGGER
  RocketDebugger
  HINTS
  PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
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





SET(ROCKET_FOUND "NO")
IF(ROCKET_CONTROLS AND ROCKET_CORE AND ROCKET_DEBUGGER AND ROCKET_INCLUDE_DIR)
    SET(ROCKET_FOUND "YES")
    SET(ROCKET_LIBRARIES ${ROCKET_CONTROLS} ${ROCKET_CORE} ${ROCKET_DEBUGGER})
ENDIF()


