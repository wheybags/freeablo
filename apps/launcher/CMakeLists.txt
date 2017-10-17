add_executable(launcher
    main.cpp
)

set_target_properties(launcher PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(launcher NuklearMisc)
