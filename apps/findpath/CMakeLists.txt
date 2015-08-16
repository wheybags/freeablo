add_executable(findpath
    main.cpp)
set_target_properties(findpath PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(findpath freeablo_lib)
