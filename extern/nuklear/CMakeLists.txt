add_library(nuklear
    nuklear_impl.h
    nuklear_header.h
    fa_nuklear.h
    nuklear.c
)

target_include_directories(nuklear INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
