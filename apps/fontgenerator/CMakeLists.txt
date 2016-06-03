link_libraries (
    StormLib)

add_executable(fontgenerator
    main.cpp)
set_target_properties(fontgenerator PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

if(UNIX) # qt headers generate a bunch of warnings, just ignore em
    set_target_properties(StormLib PROPERTIES COMPILE_FLAGS "-w")
endif()
