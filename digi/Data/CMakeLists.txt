# public header files (visible to users of this library)
set(HEADERS
	All.h
	BigEndianReader.h
	BufferedReader.h
	BufferedWriter.h
	DataException.h
	DataReader.h
	DataWriter.h
	EbmlHeader.h
	EbmlReader.h
	EbmlWriter.h
	IffReader.h
	JsonReader.h
	JsonWriter.h
	LittleEndianReader.h
	LittleEndianWriter.h
	MatFileReader.h
	MatFileWriter.h
	NativeWriter.h
	Parser.h
	ReadFunctions.h
	ReadHelper.h
	RiffReader.h
	TextReader.h
	WriteFunctions.h
	XmlHeader.h
	XmlReader.h
	XmlWriter.h
)

# source files
set(FILES
	All.cpp
	BufferedReader.cpp
	BufferedWriter.cpp
	DataException.cpp
	EbmlReader.cpp
	EbmlWriter.cpp
	JsonReader.cpp
	JsonWriter.cpp
	MatFileReader.cpp
	MatFileWriter.cpp
	TextReader.cpp
	XmlReader.cpp
	XmlWriter.cpp
)

# add definitions
add_definitions(${Data_DEFINITIONS})
 
# add library digiData
CREATE_LIBRARY(digi.Data HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
