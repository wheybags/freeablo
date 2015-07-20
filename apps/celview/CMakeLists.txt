set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(UI_HEADERS ../../resources/ui/celview.ui)

include_directories (
    ${SDL2_INCLUDE_DIR})

link_libraries (
    StormLib
    ${SDL2_LIBRARY})

add_executable(celview

    ../../resources/ui/celview.ui
    main.cpp
    mainwindow.h
    mainwindow.cpp
)

set_target_properties(celview PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")

if(UNIX) # qt headers generate a bunch of warnings, just ignore em
    set_target_properties(StormLib PROPERTIES COMPILE_FLAGS "-w")
endif()

