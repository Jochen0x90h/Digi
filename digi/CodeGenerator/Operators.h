#ifndef digi_CodeGenerator_Operators_h
#define digi_CodeGenerator_Operators_h

#include <digi/Utility/StringUtility.h>
#include <digi/Math/All.h>

#include "CodeWriter.h"
#include "TypeInfo.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

template <typename Type>
CodeStream& operator <<(CodeStream& w, const Vector2<Type>& v)
{
	w << "vector2(" << v.x << ", " << v.y << ")";
	return w;
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, const Vector3<Type>& v)
{
	w << "vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
	return w;
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, const Vector4<Type>& v)
{
	w << "vector4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return w;
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, const Quaternion<Type>& q)
{
	w << "quaternion(" << q.v.x << ", " << q.v.y << ", " << q.v.z << ", " << q.v.w << ")";
	return w;
}


// matrix

template <typename Type>
CodeStream& operator <<(CodeStream& w, const DIGI_MATRIX2X2(Type)& m)
{
	w << "matrix2x2";
	w.writeArgumentList((Type*)&m, 4);
	return w;
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, const DIGI_MATRIX3X3(Type)& m)
{
	w << "matrix3x3";
	w.writeArgumentList((Type*)&m, 9);
	return w;
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, const DIGI_MATRIX3X4(Type)& m)
{
	w << "matrix3x4";
	w.writeArgumentList((Type*)&m, 12);
	return w;
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, const DIGI_MATRIX4X4(Type)& m)
{
	w << "matrix4x4";
	w.writeArgumentList((Type*)&m, 16);
	return w;
}


// variable length vector

template <typename Type>
struct VarVector
{
	VarVector(const Type& vector, int length, bool scalarize)
		: vector(vector), length(length), scalarize(scalarize) {}

	const Type& vector;
	int length;
	bool scalarize;
};

template <typename Type>
VarVector<Type> varVector(const Type& vector, int length, bool scalarize = false)
{
	return VarVector<Type>(vector, length, scalarize);
}

template <typename Type>
inline CodeStream& operator <<(CodeStream& w, const VarVector<Type>& v)
{
	bool isScalar = true;
	for (int i = 1; i < v.length; ++i)
	{
		if (v.vector[i] != v.vector[0])
		{
			isScalar = false;
			break;
		}
	}

	if (v.length == 1 || (v.scalarize && isScalar))
	{
		// scalar
		w << v.vector[0];
	}
	else
	{
		const char* s = "splat";
		int len = 1;
		if (!isScalar)
		{
			// elements are different: use vector
			s = "vector";
			len = v.length;
		}
		w << s << v.length << '(';
		for (int i = 0; i < len; ++i)
		{
			if (i != 0)
				w << ", ";
			w << v.vector[i];
		}
		w << ')';
	}
	return w;
}

/*
// generic splat

template <typename Type>
struct GenericSplat
{
	GenericSplat(int numRows, int numColums, Type value)
		: numRows(numRows), numColumns(numColumns), value(value) {}
	
	int numRows;
	int numColumns;
	Type value;
};

template <typename Type>
GenericSplat<Type> splat(int numRows, Type value)
{
	return GenericSplat<Type>(numRows, 1, value);
}

template <typename Type>
GenericSplat<Type> splat(int numRows, int numColumns, Type value)
{
	return GenericSplat<Type>(numRows, numColumns, value);
}

template <typename Type>
CodeStream& operator <<(CodeStream& w, GenericSplat<Type> s)
{
	if (s.numRows > 1 || s.numColumns > 1)
	{
		w << "splat";
		w << s.numRows;
		if (s.numColumns > 1)
		{
			w << "x";
			w << s.numColumns;
		}
		w << "(";
	}
	
	w << s.value;
	
	if (s.numRows > 1 || s.numColumns > 1)
		w << ")";
		
	return w;
}
*/

/// @}

} // namespace digi

#endif
