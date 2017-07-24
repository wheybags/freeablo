// This file is a wrapper for nuklear.h, with the preprocessor defines
// for the options we want to use project-wide

#ifndef FA_NUKLEAR_H
#define FA_NUKLEAR_H

    #define NK_INCLUDE_FIXED_TYPES
    #define NK_INCLUDE_STANDARD_IO
    #define NK_INCLUDE_STANDARD_VARARGS
    #define NK_INCLUDE_DEFAULT_ALLOCATOR
    #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    #define NK_INCLUDE_FONT_BAKING
    #define NK_INCLUDE_DEFAULT_FONT

    #include "nuklear.h"

#endif
