#ifndef digi_Data_TextReader_h
#define digi_Data_TextReader_h

#include "BufferedReader.h"


namespace digi {

/// @addtogroup Video
/// @{

/**
	text reader. Windows/Unix line endings are detected automatically. use like this to read a text file line by line:

	TextReader r(path);
	while (!r.end())
	{
		std::string line = r.read();
	}
*/
class TextReader : public BufferedReader
{
public:
	TextReader(Pointer<IODevice> dev, int bufferSize = 1024)
		: BufferedReader(dev, bufferSize), e() {}
	TextReader(const fs::path& path, int bufferSize = 1024)
		: BufferedReader(path, bufferSize), e() {}


	/// read a line of text
	std::string read();
	
	/// indicates if last read() hit the end of input (end of file)
	bool isEnd() {return e;}
	
protected:

	bool e;
};

/// @}

} // namespace digi

#endif
