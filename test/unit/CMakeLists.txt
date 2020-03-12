project(unit_tests)

set(SOURCES
    main.cpp

    findpath/drawpath.cpp
    findpath/drawpath.h
    findpath/findpath_tests.cpp
    findpath/levelimplstub.h
    findpath/neighbors_tests.cpp

    fixedpoint.cpp
    settings.cpp
    random.cpp
    testlevelgen.cpp
    testcombatformulas.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(${PROJECT_NAME} gtest_main freeablo_lib Misc Settings Filesystem)
