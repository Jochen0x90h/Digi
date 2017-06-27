#ifndef digi_Data_EbmlHeader_h
#define digi_Data_EbmlHeader_h


namespace digi {

/// @addtogroup Data
/// @{

/**
	EBML header. reader and writer functions can be generated using StructiIO from
	this structure definition:
	
	ebml read(1000) : 0x1A45DFA3 : EbmlHeader(
	{
		ebmlVersion write(1) : 0x4286 : ubyte = 1;
		ebmlReadVersion write(1) : 0x42F7 : ubyte = 1;
		ebmlMaxIDLength write(1) : 0x42F2 : ubyte = 4;
		ebmlMaxSizeLength write(1) : 0x42F3 : ubyte = 8;
		
		docType read(100) : 0x4282 : string;
		docTypeVersion write(1) : 0x4287 : ubyte = 1;
		docTypeReadVersion write(1) : 0x4285 : ubyte = 1;
	});	
*/
struct EbmlHeader
{
	uint8_t ebmlVersion;
	uint8_t ebmlReadVersion;
	uint8_t ebmlMaxIDLength;
	uint8_t ebmlMaxSizeLength;
	std::string docType;
	uint8_t docTypeVersion;
	uint8_t docTypeReadVersion;
	
	EbmlHeader()
		: ebmlVersion(1), ebmlReadVersion(1), ebmlMaxIDLength(4), ebmlMaxSizeLength(8),
		docTypeVersion(1), docTypeReadVersion(1) {}
};
	
/// @}

} // namespace digi

#endif
