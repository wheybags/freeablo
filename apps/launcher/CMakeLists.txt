add_executable(launcher
    main.cpp
)

target_link_libraries(launcher SDL2 SDL2main Cel NuklearMisc)
if (MINGW)
    target_link_libraries(launcher -lmingw32)
endif ()
set_target_properties(launcher PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

install(TARGETS launcher DESTINATION bin)
