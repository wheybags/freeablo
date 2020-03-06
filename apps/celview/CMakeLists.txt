add_executable(celview
    main.cpp
)

target_link_libraries(celview NuklearMisc SDL2main fmt::fmt)
if (MINGW)
    target_link_libraries(celview -lmingw32)
endif ()
set_target_properties(celview PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

install(TARGETS celview DESTINATION bin)
