FIND_PATH(ROCKET_INCLUDE_DIR Rocket/Core.h
  HINTS
  PATHS
  $ENV{ROCKET_ROOT}/include
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

set(rocket_search_paths
  $ENV{ROCKET_ROOT}
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

set(rocket_search_suffixes lib64 lib libs64 libs libs/Win32 libs/Win64)


FIND_LIBRARY(ROCKET_CONTROLS
  RocketControls
  HINTS
  PATH_SUFFIXES ${rocket_search_suffixes}
  PATHS
  ${rocket_search_paths}
)
FIND_LIBRARY(ROCKET_CORE
  RocketCore
  HINTS
  PATH_SUFFIXES ${rocket_search_suffixes}
  PATHS
  ${rocket_search_paths}
)
FIND_LIBRARY(ROCKET_DEBUGGER
  RocketDebugger
  HINTS
  PATH_SUFFIXES ${rocket_search_suffixes}
  PATHS
  ${rocket_search_paths}
)

FIND_LIBRARY(ROCKET_CONTROLS_D
  RocketControls_d
  HINTS
  PATH_SUFFIXES ${rocket_search_suffixes}
  PATHS
  ${rocket_search_paths}
)
FIND_LIBRARY(ROCKET_CORE_D
  RocketCore_d
  HINTS
  PATH_SUFFIXES ${rocket_search_suffixes}
  PATHS
  ${rocket_search_paths}
)
FIND_LIBRARY(ROCKET_DEBUGGER_D
  RocketDebugger_d
  HINTS
  PATH_SUFFIXES ${rocket_search_suffixes}
  PATHS
  ${rocket_search_paths}
)





SET(ROCKET_FOUND "NO")
IF(ROCKET_CONTROLS AND ROCKET_CORE AND ROCKET_DEBUGGER AND ROCKET_INCLUDE_DIR)
    SET(ROCKET_FOUND "YES")
    IF(ROCKET_CONTROLS_D AND ROCKET_CORE_D AND ROCKET_DEBUGGER_D)
        SET(ROCKET_LIBRARIES optimized ${ROCKET_CONTROLS} optimized ${ROCKET_CORE} optimized ${ROCKET_DEBUGGER})
        SET(ROCKET_LIBRARIES ${ROCKET_LIBRARIES} debug ${ROCKET_CONTROLS_D} debug ${ROCKET_CORE_D} debug ${ROCKET_DEBUGGER_D})
    ELSE()
        SET(ROCKET_LIBRARIES ${ROCKET_CONTROLS} ${ROCKET_CORE} ${ROCKET_DEBUGGER})
    ENDIF()
ENDIF()


