/**
	int functions
	
	abs(a) absolute value
	min(a, b)
	max(a, b)
	mod(a, b)
	select(a, b, c) c ? b : a
	any(a)
	all(a)
*/


// ----------------------------------------------------------------------------
/// abs(a)

#ifndef IS_UNSIGNED
	inline SCALAR abs(SCALAR a)
	{
		return (a >= 0) ? a : -a;
	}
#endif

// ----------------------------------------------------------------------------
// min(a, b)

inline SCALAR min(SCALAR a, SCALAR b)
{
	return (a < b) ? a : b; 
}


// ----------------------------------------------------------------------------
// max(a, b)

inline SCALAR max(SCALAR a, SCALAR b)
{
	return (a > b) ? a : b; 
}


// ----------------------------------------------------------------------------
// mod(a, b)

inline SCALAR mod(SCALAR a, SCALAR b)
{
	return a % b;
}


// ----------------------------------------------------------------------------
// select(a, b, c)

inline SCALAR select(SCALAR a, SCALAR b, bool c)
{
	return c ? b : a;
}


// ----------------------------------------------------------------------------
// any(a)

#ifndef IS_UNSIGNED
	inline bool any(VECTOR2 a)
	{
		return a.x | a.y;
	}

	inline bool any(VECTOR3 a)
	{
		return a.x | a.y | a.z;
	}

	inline bool any(VECTOR4 a)
	{
		return a.x | a.y | a.z | a.w;
	}
#endif

// ----------------------------------------------------------------------------
// all(a)

#ifndef IS_UNSIGNED
	inline bool all(VECTOR2 a)
	{
		return a.x & a.y;
	}

	inline bool all(VECTOR3 a)
	{
		return a.x & a.y & a.z;
	}

	inline bool all(VECTOR4 a)
	{
		return a.x & a.y & a.z & a.w;
	}
#endif
