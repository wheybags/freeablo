set(LIB simpleini)
add_library(${LIB} INTERFACE)
target_include_directories(${LIB} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
