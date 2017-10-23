add_executable(celview
    main.cpp
)

target_link_libraries(celview NuklearMisc SDL2::SDL2main)
set_target_properties(celview PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
