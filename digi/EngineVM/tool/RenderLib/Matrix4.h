
struct MATRIX4
{
	VECTOR x;
	VECTOR y;
	VECTOR z;
	VECTOR w;

	  
	// data access
	// ------------------------

	/// returns the column with the given index
	//VECTOR& operator [](int index) {return (&this->x)[index];}

	/// returns the column with the given index
	//const VECTOR& operator [](int index) const {return (&this->x)[index];}
};


// ----------------------------------------------------------------------------
// creators

// create from column vectors
inline MATRIX4 matrix4(VECTOR c1, VECTOR c2, VECTOR c3, VECTOR c4)
{
	MATRIX4 a = {c1, c2, c3, c4};
	return a;
}


// ----------------------------------------------------------------------------
// operators

/// unary plus 
inline MATRIX4 operator +(MATRIX4 a)
{
	return a;
}

/// unary minus, component-wise
inline MATRIX4 operator -(MATRIX4 a)
{
	return matrix4(
		-a.x,
		-a.y,
		-a.z,
		-a.w);
}

/// binary plus, component-wise
inline MATRIX4 operator +(MATRIX4 a, MATRIX4 b)
{
	return matrix4(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w);
}

/// binary minus, component-wise
inline MATRIX4 operator -(MATRIX4 a, MATRIX4 b)
{ 
	return matrix4(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w);
}

/// multiply by scalar
inline MATRIX4 operator *(MATRIX4 a, SCALAR b)
{ 
	return matrix4(
		a.x * b,
		a.y * b,
		a.z * b,
		a.w * b);  
}

/// multiply by scalar
inline MATRIX4 operator *(SCALAR a, MATRIX4& b)
{ 
	return matrix4(
		a * b.x,
		a * b.y,
		a * b.z,
		a * b.w);  
}

/// divide by scalar
inline MATRIX4 operator /(MATRIX4 a, SCALAR b)
{ 
	return matrix4(
		a.x / b,
		a.y / b,
		a.z / b,
		a.w / b);  
}


// assignment versions of operators
/*
inline MATRIX4& operator +=(MATRIX4& a, MATRIX4& b)
{ 
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}

inline MATRIX4& operator -=(MATRIX4& a, MATRIX4& b)
{ 
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}

inline MATRIX4& operator *=(MATRIX4& a, SCALAR b)
{ 
	a.x *= b;
	a.y *= b;
	a.z *= b;
	a.w *= b;
	return a;
}
*/