add_executable(exedump main.cpp dumptileset.cpp dumptileset.h)
target_link_libraries(exedump DiabloExe fmt::fmt cxxopts Cel Levels)
set_target_properties(exedump PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
