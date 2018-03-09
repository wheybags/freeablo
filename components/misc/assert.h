
#pragma once

#include <stdio.h>
#include <stdlib.h>

#define message_and_abort(message, ...)                                                                                                                        \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        fprintf(stderr, message, __VA_ARGS__);                                                                                                                 \
        abort();                                                                                                                                               \
    } while (0)

#define release_assert(cond)                                                                                                                                   \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if (!(cond))                                                                                                                                           \
            message_and_abort("ASSERTION FAILED: (%s) in %s:%d\n", #cond, __FILE__, __LINE__);                                                                 \
    } while (0)

#ifdef NDEBUG
#   define debug_assert(cond)                                                                                                                                  \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        (void)sizeof(cond);                                                                                                                                    \
    } while (0)
#else
#   define debug_assert(cond) release_assert(cond)
#endif


#define invalid_enum(enumType, val) message_and_abort("Invalid %s enum used at %s:%d, value %d", #enumType, __FILE__, __LINE__, int(val))
