add_executable(launcher
    main.cpp
)

target_link_libraries(launcher SDL2::SDL2 Cel NuklearMisc ${HUNTER_BOOST_LIBS})
if (MINGW)
target_link_libraries(launcher -lmingw32 SDL2::SDL2main)
else ()
target_link_libraries(launcher SDL2::SDL2main)
endif ()
set_target_properties(launcher PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
