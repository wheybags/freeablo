#ifndef HELPER_2D_H
#define HELPER_2D_H

#include <stdlib.h>

namespace Misc
{
    ///
    /// Class to help turn one dimensional storage into 2D, intended to be used as the
    /// return value of an operator[] method, to allow object.[x][y] style addressing
    ///
    template <class From, class Retval> class Helper2D
    {
        private:
            From& parent;
            int32_t x;
            Retval (*func)(int32_t, int32_t, From&); 
        public:    
            Helper2D(From& _p, int32_t _x, Retval (*_func)(int32_t, int32_t, From&)) : parent(_p), x(_x), func(_func) {}
            
            Retval operator[](int32_t y){ return func(x, y, parent); }
    };
}

#endif
