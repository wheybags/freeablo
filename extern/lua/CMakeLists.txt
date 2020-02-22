file(GLOB LUA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB LUA_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(lua
  ${LUA_SRC}
  ${LUA_HEADERS})

add_executable(lua_bin
  ${LUA_SRC}
  ${LUA_HEADERS})

target_link_libraries(lua Misc)
target_link_libraries(lua_bin Misc)

target_include_directories(lua INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
