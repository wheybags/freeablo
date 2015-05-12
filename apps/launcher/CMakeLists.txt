set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(UI_HEADERS ../../resources/ui/mainwindow.ui)

include_directories (
    ${SDL2_INCLUDE_DIR})

link_libraries (
    ${SDL2_LIBRARY})

add_executable(launcher

    ../../resources/ui/mainwindow.ui
    main.cpp
    mainwindow.h
    mainwindow.cpp
    page.h
    playpage.h
    playpage.cpp
    graphicspage.h
    graphicspage.cpp
    processinvoker.h
    processinvoker.cpp
)

set_target_properties(launcher PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
