#include "../Resource.h"

#include "windows.h"


namespace digi {

fs::path getResource(StringRef resource)
{
	// get path to this executable
	wchar_t exePath[MAX_PATH + 1];
	int length = GetModuleFileNameW(NULL, exePath, MAX_PATH + 1);
	
	// only use path (remove file name)
//#ifdef _WIN32_WCE
	int numRemove = 1;
//#else
//	int numRemove = 2;
//#endif
	for (int i = length - 1; i >= 0; --i)
	{
		if (exePath[i] == L'\\')
		{
			exePath[i] = 0;
			if (--numRemove <= 0)
				break;
		}
	}
	return fs::path(exePath) / "Resources" / resource; 
}

} // namespace digi
