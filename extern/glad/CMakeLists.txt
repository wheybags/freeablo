if (DEBUG_GRAPHICS)
  set(glad_prefix debug)
else()
  set(glad_prefix normal)
endif()

add_library(glad "${glad_prefix}/src/glad.c")
target_include_directories(glad PUBLIC "${glad_prefix}/include")
