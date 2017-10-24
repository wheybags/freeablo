add_executable(fontgenerator main.cpp)
target_link_libraries(fontgenerator Cel Settings)
set_target_properties(fontgenerator PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
