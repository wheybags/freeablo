add_executable(launcher
    main.cpp
)

target_link_libraries(launcher SDL2::SDL2 SDL2::SDL2main Cel NuklearMisc ${HUNTER_BOOST_LIBS})
set_target_properties(launcher PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
