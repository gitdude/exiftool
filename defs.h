#ifndef DEFS_H
#define DEFS_H

#ifdef WIN32
	#define int32 unsigned __int32
	#define int16 __int16
#else
	#include <stdint.h>
	#define int32 uint32_t
	#define int16 int16_t
#endif

#ifdef VERBOSE
	#define DEBUG(x) { debugstream.str(""); debugstream << x; cout << debugstream.str() << "\n"; }
#else
	#define DEBUG(x)
#endif

#define SET_INT32(x, y)  { memcpy(&x, &y, 4); if (conversionNeeded) chEndianess(x); }

#endif //DEFS_H
