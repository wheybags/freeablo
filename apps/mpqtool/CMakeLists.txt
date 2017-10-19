add_executable(mpqtool main.cpp)
target_link_libraries(mpqtool FAIO)
set_target_properties(mpqtool PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
