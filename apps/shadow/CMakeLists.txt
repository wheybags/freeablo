add_executable(shadow
    main.cpp)
set_target_properties(shadow PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(shadow freeablo_lib)
