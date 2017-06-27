
struct MATRIX2
{
	VECTOR x;
	VECTOR y;

	  
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
inline MATRIX2 matrix2(VECTOR c1, VECTOR c2)
{
	MATRIX2 a = {c1, c2};
	return a;
}


// ----------------------------------------------------------------------------
// operators

/// unary plus 
inline MATRIX2 operator +(MATRIX2 a)
{
	return a;
}

/// unary minus, component-wise
inline MATRIX2 operator -(MATRIX2 a)
{
	return matrix2(
		-a.x,
		-a.y);
}

/// binary plus, component-wise
inline MATRIX2 operator +(MATRIX2 a, MATRIX2 b)
{
	return matrix2(
		a.x + b.x,
		a.y + b.y);
}

/// binary minus, component-wise
inline MATRIX2 operator -(MATRIX2 a, MATRIX2 b)
{ 
	return matrix2(
		a.x - b.x,
		a.y - b.y);
}

/// multiply by scalar
inline MATRIX2 operator *(MATRIX2 a, SCALAR b)
{ 
	return matrix2(
		a.x * b,
		a.y * b);  
}

/// multiply by scalar
inline MATRIX2 operator *(SCALAR a, MATRIX2 b)
{ 
	return matrix2(
		a * b.x,
		a * b.y);  
}

/// divide by scalar
inline MATRIX2 operator /(MATRIX2 a, SCALAR b)
{ 
	return matrix2(
		a.x / b,
		a.y / b);  
}


// assignment versions of operators
/*
inline MATRIX2& operator +=(MATRIX2& a, MATRIX2 b)
{ 
	a.x += b.x;
	a.y += b.y;
	return a;
}

inline MATRIX2& operator -=(MATRIX2& a, MATRIX2 b)
{ 
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

inline MATRIX2& operator *=(MATRIX2& a, SCALAR b)
{ 
	a.x *= b;
	a.y *= b;
	return a;
}
*/