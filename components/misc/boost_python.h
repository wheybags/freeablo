
#ifndef PY_BOOST_HEADER_HACK
#define PY_BOOST_HEADER_HACK

    // fix a compile issue on osx: https://bugs.python.org/issue10910
    #if __APPLE__
        #include <unistd.h>

        #define APPLE_TEMP __APPLE__
        #undef __APPLE__ // undefing __APPLE__ stops it applying the osx "fix"
                         // that causes the bug mentioned above
        #include <pyport.h>
        #define __APPLE__ APPLE_TEMP
        #undef APPLE_TEMP
    #endif

    #include <boost/python.hpp>

#endif
