#ifdef __GNUC__
    #pragma GCC diagnostic push

    #ifdef __clang__
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
