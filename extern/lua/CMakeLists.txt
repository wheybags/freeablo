file(GLOB LUA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB LUA_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(lua
  ${LUA_SRC}
  ${LUA_HEADERS})

target_include_directories(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
