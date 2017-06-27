#ifndef digi_Base_Platform_h
#define digi_Base_Platform_h

// Config.h gets created by CMake from Config.h.cmake
#include "Config.h"

#include <stddef.h> // NULL, size_t, wchar_t

/*
	include stdint.h which provides the following:
	- fixed width types: int8_t, int16_t, int32_t int64t, uint8_t, uint16_t, uint32_t, uint64_t
	- macros for fixed width constants when __STDC_CONSTANT_MACROS is defined: INT8_C, INT16_C, INT32_C, INT64_C, UINT8_C, UINT16_C, UINT32_C, UINT64_C
	
	example: uint64_t myConstant = UINT64_C(1000000000000000);
*/
#ifdef HAVE_STDINT_H
	#include <stdint.h> 
#else
	// no stdint.h, let's improvise
	#ifdef _MSC_VER
		// http://msinttypes.googlecode.com/svn/trunk/ provides a reference implementation of stdint.h for Visual Studio
		typedef signed __int8  int8_t;
		typedef signed __int16 int16_t;
		typedef signed __int32 int32_t;
		typedef signed __int64 int64_t;
		
		typedef unsigned __int8  uint8_t;
		typedef unsigned __int16 uint16_t;
		typedef unsigned __int32 uint32_t;
		typedef unsigned __int64 uint64_t;

		#ifdef __STDC_CONSTANT_MACROS
			#ifndef INT8_C
				#define INT8_C(value)  value##i8
			#endif
			#ifndef INT16_C
				#define INT16_C(value) value##i16
			#endif
			#ifndef INT32_C
				#define INT32_C(value) value##i32
			#endif
			#ifndef INT64_C
				#define INT64_C(value) value##i64
			#endif

			#ifndef UINT8_C
				#define UINT8_C(value)  value##ui8
			#endif
			#ifndef UINT16_C
				#define UINT16_C(value) value##ui16
			#endif
			#ifndef UINT32_C
				#define UINT32_C(value) value##ui32
			#endif
			#ifndef UINT64_C
				#define UINT64_C(value) value##ui64
			#endif
		#endif
	#else
		#error "error: no <stdint.h> available"
	#endif
#endif


// undef some macros
#undef ushort
#undef uint
#undef ulong


/*
	compiler specific macros:

	ALIGN(x): data alignment. can only increase the alignment, e.g. ALIGN(16) int data[4];
		therefore ALIGN(1) does NOT have the same effect as #pragma pack(1)

	print preprocessor definitions for MacOS on console: "cpp -dM", then ctrl-D
*/

// Microsoft Visual C++
#ifdef _MSC_VER

	// warning C4996: 'identifier' was declared deprecated
	#pragma warning(disable: 4996)

	#ifndef ALIGN
		#define ALIGN(x) __declspec(align(x))
	#endif

#endif

// GCC
#ifdef __GNUC__

	#ifndef ALIGN
		#define ALIGN(x) __attribute__((__aligned__(x)))
	#endif

#endif


/*
	platform specific macros:
	
	DIGI_EXPORT: export a symbol
	DIGI_IMPORT: import a symbol
*/
#ifdef _WIN32

	#define DIGI_EXPORT __declspec(dllexport)
	#define DIGI_IMPORT __declspec(dllimport)

#else

	#define DIGI_EXPORT __attribute__((visibility("default")))
	#define DIGI_IMPORT

#endif


#endif
