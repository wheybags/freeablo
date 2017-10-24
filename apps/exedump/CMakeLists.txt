add_executable(exedump main.cpp)
target_link_libraries(exedump DiabloExe)
set_target_properties(exedump PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
