project(unit_tests)

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

set(SOURCES
    main.cpp

    findpath/drawpath.cpp
    findpath/drawpath.h
    findpath/findpath_tests.cpp
    findpath/levelimplstub.h
    findpath/neighbors_tests.cpp

    fixedpoint.cpp
    settings.cpp
    serialisation.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
target_link_libraries(${PROJECT_NAME} GTest::gtest freeablo_lib Misc Settings)
