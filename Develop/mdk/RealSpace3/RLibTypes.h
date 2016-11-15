#ifndef __RLIB_TYPES_H
#define __RLIB_TYPES_H

#include "RLib.h"

namespace rs3 {
class RToolApplication;
}

RS_API rs3::RToolApplication* createToolApplication();
RS_API void destroyToolApplication();

#pragma warning( disable : 4251 )
#pragma warning( disable : 4231 )


#endif // #ifndef __RLIB_TYPES_H
