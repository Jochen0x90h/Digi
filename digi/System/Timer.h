#ifndef digi_System_Timer_h
#define digi_System_Timer_h

#include <digi/Utility/Standard.h>


#if defined(_WIN32)
	// Windows	
/*
	#define NOMINMAX 
	#include <Windows.h>
	#include <Mmsystem.h>
	#undef interface
*/	
	extern "C"
	{
		#ifdef _WIN32_WCE
			unsigned long __cdecl timeGetTime(void);
			void __cdecl Sleep(unsigned long);
		#else
			__declspec(dllimport) unsigned long __stdcall timeGetTime(void);
			__declspec(dllimport) void __stdcall Sleep(unsigned long);
		#endif
	}
#elif defined(__APPLE__)
	// MacOS X
	#include <CoreServices/CoreServices.h>
#else
	// POSIX
	#include <sys/time.h>
#endif


namespace digi {

/// @addtogroup System
/// @{

namespace Timer
{
	inline int getMilliSeconds()
	{
		#if defined(_WIN32)
			// Win32
			return timeGetTime();
		#elif defined(__APPLE__)
			// OSX
			int64_t us;
			Microseconds((UnsignedWide*)&us);
			return us / 1000L;
		#else
			// POSIX
			struct timeval time;
			gettimeofday(&time, NULL);
			return time.tv_sec * 1000 + time.tv_usec / 1000.0;
		#endif
	}

	inline void milliSleep(uint milliSeconds)
	{
		#if defined(_WIN32)
			// Win32
			Sleep(milliSeconds);
		#else
			// POSIX including OSX
			struct timespec t;
			t.tv_sec = milliSeconds / 1000;
			t.tv_nsec = (milliSeconds % 1000) * 1000000;
			nanosleep(&t, NULL);
		#endif		
	}
}

/// @}

} // namespace digi

#endif
