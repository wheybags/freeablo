add_executable(celview
    main.cpp
)

target_link_libraries(celview NuklearMisc)
set_target_properties(celview PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
