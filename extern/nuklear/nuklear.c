// nuklear is a header-only library.
// The fucntions are defined in the header, but disabled
// unless NK_IMPLEMENTATION is defined.
// We need to define them in one translation unit so they
// can be linked to, and that is this file.
// We have actually used a python script to split the
// implemenntation into the "nuklear_impl.h" header though.

#include <misc/assert.h>

#define NK_ASSERT release_assert
#define NK_IMPLEMENTATION

#include "fa_nuklear.h"
#include "nuklear_impl.h"
