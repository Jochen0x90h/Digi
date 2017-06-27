#ifndef digi_Data_XMLHeader_h
#define digi_Data_XMLHeader_h


namespace digi {

/// @addtogroup Data
/// @{

/// XML header
struct XMLHeader
{
	// version, e.g. '1.0'
	std::string version;

	// encoding, e.g. 'UTF-8'
	std::string encoding;

	// document type
	std::string docType;
};

/// @}

} // namespace digi

#endif
