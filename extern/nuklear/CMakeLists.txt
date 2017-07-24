add_library(nuklear
    nuklear.h
    fa_nuklear.h
    nuklear.c
)

target_include_directories(nuklear INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
