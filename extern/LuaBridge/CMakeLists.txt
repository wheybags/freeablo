cmake_minimum_required (VERSION 3.5)

project (LuaBridge)

include (CMakeDependentOption)

set (CMAKE_CXX_STANDARD 11)

option (LUABRIDGE_NO_CXX11 "Use C++11 standard if supported by compiler" OFF)

cmake_dependent_option (LUABRIDGE_TESTING "Build tests" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

if (LUABRIDGE_NO_CXX11)
  add_definitions (-DLUABRIDGE_NO_CXX11)
endif ()

add_subdirectory (Source)

if (LUABRIDGE_TESTING)
    set (gtest_force_shared_crt ON CACHE BOOL "Use /MD and /MDd" FORCE)
    add_subdirectory (third_party/gtest)

    add_subdirectory (Tests)
endif ()

add_custom_target (Documentation SOURCES
    CHANGES.md
    README.md
    Doxyfile
    index.html
    Manual.html
)
