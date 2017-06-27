#include <CoreFoundation/CoreFoundation.h>

#include <digi/Utility/StringUtility.h>
#include "../Resource.h"


namespace digi {

fs::path getResource(StringRef resource)
{
	const UInt8* base = (const UInt8*)resource.data();
	ptrdiff_t baseLength = find(resource, '.');

	const UInt8* ext = base + baseLength + 1;
	ptrdiff_t extLength = resource.length() - baseLength - 1;

	CFStringRef cfBase = CFStringCreateWithBytesNoCopy(
		kCFAllocatorDefault,
		base,
		baseLength,
		kCFStringEncodingUTF8,
		false,
		kCFAllocatorNull);
	
	CFStringRef cfExt = CFStringCreateWithBytesNoCopy(
		kCFAllocatorDefault,
		ext,
		extLength,
		kCFStringEncodingUTF8,
		false,
		kCFAllocatorNull);

	// get url of resource
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef url;
	url = CFBundleCopyResourceURL(mainBundle, cfBase, cfExt, NULL);
	if (url == NULL)
		return fs::path();

	// copy into buffer
	char buffer[2048];
	CFURLGetFileSystemRepresentation(url, true, (UInt8*)buffer, sizeof(buffer));
	return buffer;
}

} // namespace digi
