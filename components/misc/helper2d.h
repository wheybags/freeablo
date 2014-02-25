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
            size_t x;
            Retval (*func)(size_t, size_t, From&); 
        public:    
            Helper2D(From& _p, size_t _x, const Retval& (*_func)(size_t, size_t, From&)) : parent(_p), x(_x), func(_func) {}
            
            Retval operator[](size_t y){ return func(x, y, parent); }
    };
}

#endif
