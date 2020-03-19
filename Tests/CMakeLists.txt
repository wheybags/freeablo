set (LUABRIDGE_TEST_SOURCE_FILES
  Source/ClassTests.cpp
  Source/IssueTests.cpp
  Source/IteratorTests.cpp
  Source/LegacyTests.cpp
  Source/ListTests.cpp
  Source/LuaRefTests.cpp
  Source/MapTests.cpp
  Source/NamespaceTests.cpp
  Source/PerformanceTests.cpp
  Source/RefCountedPtrTests.cpp
  Source/StackTests.cpp
  Source/Tests.cpp
  Source/TestBase.h
  Source/TestTypes.h
  Source/TestsMain.cpp
  Source/UnorderedMapTests.cpp
  Source/VectorTests.cpp
)

source_group ("Source" FILES ${LUABRIDGE_TEST_SOURCE_FILES})

set (LUABRIDGE_TEST_JUICE_FILES
  JuceLibraryCode/AppConfig.h
  JuceLibraryCode/BinaryData.h
  JuceLibraryCode/BinaryData.cpp
  JuceLibraryCode/JuceHeader.h
)

add_library (JuceLibraryCode ${LUABRIDGE_TEST_JUICE_FILES})


# Lua 5.1

file (GLOB_RECURSE LUABRIDGE_TEST_LUA51_FILES
  LuaLibrary.h
  Lua/LuaLibrary5.1.5.cpp
)

add_library (Lua51 ${LUABRIDGE_TEST_LUA51_FILES})

target_compile_options (Lua51
  PUBLIC -DLUABRIDGEDEMO_LUA_VERSION=501
)

target_include_directories (Lua51 PRIVATE .)


# Lua 5.2, C++11

file (GLOB_RECURSE LUABRIDGE_TEST_LUA52_FILES
  Lua/LuaLibrary.h
  Lua/LuaLibrary5.2.0.cpp
)

add_library (Lua52 ${LUABRIDGE_TEST_LUA52_FILES})

target_compile_options (Lua52
  PUBLIC -DLUABRIDGEDEMO_LUA_VERSION=502
)

target_include_directories (Lua52 PRIVATE .)


macro (add_test_app LUABRIDGE_TEST_NAME LUABRIDGE_TEST_LUA_LIBRARY)

  add_executable (${LUABRIDGE_TEST_NAME}
    ${LUABRIDGE_TEST_SOURCE_FILES}
  )

  target_include_directories (${LUABRIDGE_TEST_NAME} PRIVATE . Source)

  target_link_libraries (${LUABRIDGE_TEST_NAME}
    LuaBridge
    ${LUABRIDGE_TEST_LUA_LIBRARY}
    gtest
    JuceLibraryCode
  )

endmacro (add_test_app)


# C++11

add_test_app (Tests51 Lua51)

add_test_app (Tests52 Lua52)


# Pre C++11

add_test_app (Tests51L Lua51)
target_compile_definitions (Tests51L PRIVATE LUABRIDGE_NO_CXX11)

add_test_app (Tests52L Lua52)
target_compile_definitions (Tests52L PRIVATE LUABRIDGE_NO_CXX11)
