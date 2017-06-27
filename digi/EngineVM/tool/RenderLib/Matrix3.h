
struct MATRIX3
{
	VECTOR x;
	VECTOR y;
	VECTOR z;

	  
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
inline MATRIX3 matrix3(VECTOR c1, VECTOR c2, VECTOR c3)
{
	MATRIX3 a = {c1, c2, c3};
	return a;
}


// ----------------------------------------------------------------------------
// operators

/// unary plus 
inline MATRIX3 operator +(MATRIX3 a)
{
	return a;
}

/// unary minus, component-wise
inline MATRIX3 operator -(MATRIX3 a)
{
	return matrix3(
		-a.x,
		-a.y,
		-a.z);
}

/// binary plus, component-wise
inline MATRIX3 operator +(MATRIX3 a, MATRIX3 b)
{
	return matrix3(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z);
}

/// binary minus, component-wise
inline MATRIX3 operator -(MATRIX3 a, MATRIX3 b)
{ 
	return matrix3(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z);
}

/// multiply by scalar
inline MATRIX3 operator *(MATRIX3 a, SCALAR b)
{ 
	return matrix3(
		a.x * b,
		a.y * b,
		a.z * b);  
}

/// multiply by scalar
inline MATRIX3 operator *(SCALAR a, MATRIX3 b)
{ 
	return matrix3(
		a * b.x,
		a * b.y,
		a * b.z);  
}

/// divide by scalar
inline MATRIX3 operator /(MATRIX3 a, SCALAR b)
{ 
	return matrix3(
		a.x / b,
		a.y / b,
		a.z / b);  
}


// assignment versions of operators
/*
inline MATRIX3& operator +=(MATRIX3& a, MATRIX3 b)
{ 
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

inline MATRIX3& operator -=(MATRIX3& a, MATRIX3 b)
{ 
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

inline MATRIX3& operator *=(MATRIX3& a, SCALAR b)
{ 
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}
*/