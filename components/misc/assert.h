
#pragma once

#include <stdio.h>
#include <stdlib.h>

#define release_assert(cond)                                                                                                                                   \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if (!(cond))                                                                                                                                           \
        {                                                                                                                                                      \
            fprintf(stderr, "ASSERTION FAILED: (%s) in %s:%d\n", #cond, __FILE__, __LINE__);                                                                   \
            exit(1);                                                                                                                                           \
        }                                                                                                                                                      \
    } while (0)

#ifdef NDEBUG
#define debug_assert(cond)                                                                                                                                     \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        (void)sizeof(cond);                                                                                                                                    \
    } while (0)
#else
#define debug_assert(cond) release_assert(cond)
#endif

