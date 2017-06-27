#include <digi/Utility/StringUtility.h>

#include "TypeInfo.h"


namespace digi {

// ScalarInfo

ScalarInfo::ScalarInfo(const std::string& type)
	: type(INVALID)
{
	MatrixInfo info = MatrixInfo::parse(type);
	if (info.numRows <= 1 && info.numColumns <= 1)
		*this = info;
}


// VectorInfo

VectorInfo::VectorInfo(const std::string& type)
	: numRows(0)
{
	MatrixInfo info = MatrixInfo::parse(type);
	if (info.numColumns <= 1)
		*this = info;
}


// MatrixInfo

int MatrixInfo::getSize()
{
	int s;
	switch (this->type)
	{
	case BOOL:
	case BYTE:
	case UBYTE:
		s = 1;
		break;
	case SHORT:
	case USHORT:
		s = 2;
		break;
	case INT:
	case UINT:
		s = 4;
		break;
	case LONG:
	case ULONG:
		s = 8;
		break;
	case FLOAT:
		s = 4;
		break;
	case DOUBLE:
		s = 8;
		break;
	default:
		s = 0;
	}

	return s * this->numRows * this->numColumns;
}

MatrixInfo MatrixInfo::parse(const std::string& typeName)
{
	int s;
	Type type;
	if (startsWith(typeName, "bool"))
	{
		s = 4;
		type = BOOL;
	}
	else if (startsWith(typeName, "byte"))
	{
		s = 4;
		type = BYTE;
	}
	else if (startsWith(typeName, "ubyte"))
	{
		s = 5;
		type = UBYTE;
	}
	else if (startsWith(typeName, "short"))
	{
		s = 5;
		type = SHORT;
	}
	else if (startsWith(typeName, "ushort"))
	{
		s = 6;
		type = USHORT;
	}
	else if (startsWith(typeName, "int"))
	{
		s = 3;
		type = INT;
	}
	else if (startsWith(typeName, "uint"))
	{
		s = 4;
		type = UINT;
	}
	else if (startsWith(typeName, "long"))
	{
		s = 4;
		type = LONG;
	}
	else if (startsWith(typeName, "ulong"))
	{
		s = 5;
		type = ULONG;
	}
	else if (startsWith(typeName, "float"))
	{
		s = 5;
		type = FLOAT;
	}
	else if (startsWith(typeName, "double"))
	{
		s = 6;
		type = DOUBLE;
	}
	else
		return MatrixInfo();

	int len = int(typeName.length());
	if (s == len)
		return MatrixInfo(type, 1, 1);
	
	char ch = typeName[s];
	if (ch < '1' || ch > '4')
		return MatrixInfo();
	
	int numRows = ch - '0';
	++s;
	if (s == len)
		return MatrixInfo(type, numRows, 1);
	
	if (typeName[s] != 'x')
		return MatrixInfo();
	++s;
	
	ch = typeName[s];
	if (ch < '1' || ch > '4')
		return MatrixInfo();
	
	int numColumns = ch - '0';
	++s;
	if (s == len)
		return MatrixInfo(type, numRows, numColumns);
	
	return MatrixInfo();	
}

std::string MatrixInfo::toString(Type type, int numRows, int numColumns)
{
	std::string s;
	switch (type)
	{
		case INVALID:
			break;
		case BOOL:
			s = "bool";
			break;
		case BYTE:
			s = "byte";
			break;
		case UBYTE:
			s = "ubyte";
			break;
		case SHORT:
			s = "short";
			break;
		case USHORT:
			s = "ushort";
			break;
		case INT:
			s = "int";
			break;
		case UINT:
			s = "uint";
			break;
		case LONG:
			s = "long";
			break;
		case ULONG:
			s = "ulong";
			break;
		case FLOAT:
			s = "float";
			break;
		case DOUBLE:
			s = "double";
			break;
	}
	if (numRows > 1 || numColumns > 1)
	{
		s += '0' + numRows;
	}
	if (numColumns > 1)
	{
		s += 'x';
		s += '0' + numColumns;
	}	
	return s;
}

std::string MatrixInfo::getSuffix(int numRows, int numColumns)
{
	std::string s;
	if (numRows > 1 || numColumns > 1)
	{
		s += '0' + numRows;
	}
	if (numColumns > 1)
	{
		s += 'x';
		s += '0' + numColumns;
	}	
	return s;
}


} // namespace digi
