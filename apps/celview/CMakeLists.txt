add_executable(celview
    main.cpp
)

target_link_libraries(celview NuklearMisc)
if (MINGW)
target_link_libraries(celview -lmingw32 SDL2::SDL2main)
else ()
target_link_libraries(celview SDL2::SDL2main)
endif ()
set_target_properties(celview PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
