set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(UI_HEADERS mainwindow.ui)

add_executable(launcher

    mainwindow.ui
    main.cpp
    mainwindow.h
    mainwindow.cpp
    )

set_target_properties(launcher PROPERTIES COMPILE_FLAGS "${FA_COMPILER_FLAGS}")
