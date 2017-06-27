#include "File.h"


namespace digi {

File::~File()
{
}

void File::flush()
{
}

/*
bool File::copy(const std::string& dst, const std::string& src)
{
	Pointer<IODevice> srcFile = open(src, READ);
	if (srcFile == null)
		return false;
	
	Pointer<IODevice> dstFile = open(dst, WRITE_NEW);
	if (dstFile == null)
		return false;
	
	uint8_t buffer[8192];
	
	while (!srcFile->isEndOfInput() && !srcFile->isError() && !dstFile->isError())
	{
		size_t numRead = srcFile->readData(buffer, 8192);
		dstFile->writeData(buffer, numRead);
	}
	
	srcFile->close();
	dstFile->close();
	
	return !srcFile->isError() && !dstFile->isError();
}
*/

} // namespace digi
