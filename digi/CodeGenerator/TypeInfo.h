#ifndef digi_CodeGenerator_TypeInfo_h
#define digi_CodeGenerator_TypeInfo_h

#include <string>


namespace digi {

/// @addtogroup CodeGenerator
/// @{

struct ScalarInfo
{
	enum Type
	{
		INVALID = -1,
		
		BOOL,
		
		BYTE,
		UBYTE,
		SHORT,
		USHORT,
		INT,
		UINT,
		LONG,
		ULONG,
		
		FLOAT,
		DOUBLE,
	};

	Type type;

	ScalarInfo()
		: type(INVALID) {}

	ScalarInfo(Type type)
		: type(type) {}

	// parse and construct from string
	explicit ScalarInfo(const std::string& type);

	// convert type to string (e.g. "float")
	std::string toString() const;

	// returns true if the type is valid
	bool isValid() const {return this->type != INVALID;}
};

static inline bool operator ==(const ScalarInfo& a, const ScalarInfo& b)
{
	return a.type == b.type;
}


// vector type info
struct VectorInfo : public ScalarInfo
{	
	int numRows;

	VectorInfo()
		: numRows(0) {}

	VectorInfo(Type type, int numRows)
		: ScalarInfo(type), numRows(numRows) {}

	// parse and construct from string
	explicit VectorInfo(const std::string& type);

	// convert type to string (e.g. "float3")
	std::string toString() const;

	// get suffix (e.g. "3")
	std::string getSuffix() const;
	
	VectorInfo& operator =(const ScalarInfo& a)
	{
		this->type = a.type;
		this->numRows = 1;
		return *this;
	}
};

static inline bool operator ==(const VectorInfo& a, const VectorInfo& b)
{
	return a.type == b.type && a.numRows == b.numRows;
}


// matrix type info
struct MatrixInfo : public VectorInfo
{
	friend struct ScalarInfo;
	friend struct VectorInfo;

	int numColumns;
	
	MatrixInfo()
		: numColumns(0) {}
	
	MatrixInfo(Type type, int numRows, int numColumns)
		: VectorInfo(type, numRows), numColumns(numColumns) {}
	
	// parse and construct from string
	explicit MatrixInfo(const std::string& type) {*this = MatrixInfo::parse(type);}
	
	// convert type to string (e.g. "float3")
	std::string toString() const {return MatrixInfo::toString(this->type, this->numRows, this->numColumns);}

	// get suffix (e.g. "3x3")
	std::string getSuffix() const {return MatrixInfo::getSuffix(this->numRows, this->numColumns);}

	// get size in bytes
	int getSize();

	MatrixInfo& operator =(const ScalarInfo& a)
	{
		this->type = a.type;
		this->numRows = 1;
		this->numColumns = 1;
		return *this;
	}

	MatrixInfo& operator =(const VectorInfo& a)
	{
		this->type = a.type;
		this->numRows = a.numRows;
		this->numColumns = 1;
		return *this;
	}

protected:

	static MatrixInfo parse(const std::string& type);
	static std::string toString(Type, int numRows, int numColumns);
	static std::string getSuffix(int numRows, int numColumns);
};

static inline bool operator ==(const MatrixInfo& a, const MatrixInfo& b)
{
	return a.type == b.type && a.numRows == b.numRows && a.numColumns == b.numColumns;
}


inline std::string ScalarInfo::toString() const
{
	return MatrixInfo::toString(this->type, 1, 1);
}

inline std::string VectorInfo::toString() const
{
	return MatrixInfo::toString(this->type, this->numRows, 1);
}

inline std::string VectorInfo::getSuffix() const
{
	return MatrixInfo::getSuffix(this->numRows, 1);
}

/// @}

} // namespace digi

#endif
