#ifdef __GNUC__
    #pragma GCC diagnostic push

    #define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

    #if defined(__clang__) || GCC_VERSION > 40900
        #pragma GCC diagnostic ignored "-Wpedantic"
    #else
        #pragma GCC diagnostic ignored "-pedantic"
    #endif
    
    #pragma GCC diagnostic ignored "-Wall"
    #pragma GCC diagnostic ignored "-Wextra"
    #pragma GCC diagnostic ignored "-Wlong-long"
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
