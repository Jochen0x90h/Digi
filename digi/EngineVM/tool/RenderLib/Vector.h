
// ----------------------------------------------------------------------------
// typedefs

typedef SCALAR __attribute__((ext_vector_type(2))) VECTOR2;
typedef SCALAR __attribute__((ext_vector_type(3))) VECTOR3;
typedef SCALAR __attribute__((ext_vector_type(4))) VECTOR4;


// ----------------------------------------------------------------------------
// packed vectors

typedef SCALAR __attribute__((ext_vector_type(2), aligned(SIZE))) CONCAT(packed_, VECTOR2);

struct CONCAT(packed_, VECTOR3)
{
	SCALAR x;
	SCALAR y;
	SCALAR z;
	
	CONCAT(packed_, VECTOR3)& operator =(VECTOR3 v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	
	operator VECTOR3()
	{
		return (VECTOR3)(x, y, z);
	}
};

typedef SCALAR __attribute__((ext_vector_type(4), aligned(SIZE))) CONCAT(packed_, VECTOR4);


// ----------------------------------------------------------------------------
// splat


inline VECTOR2 splat2(SCALAR splat)
{
	return VECTOR2(splat);
}


inline VECTOR3 splat3(SCALAR splat)
{
	return VECTOR3(splat);
}

inline VECTOR4 splat4(SCALAR splat)
{
	return VECTOR4(splat);
}


// ----------------------------------------------------------------------------
// create vector 2

inline VECTOR2 vector2(SCALAR x, SCALAR y)
{
	return (VECTOR2){x, y};
}


// ----------------------------------------------------------------------------
// create vector 3

inline VECTOR3 vector3(SCALAR x, SCALAR y, SCALAR z)
{
	return (VECTOR3){x, y, z};
}

inline VECTOR3 vector3(VECTOR2 xy, SCALAR z)
{
	return (VECTOR3){xy.x, xy.y, z};
}

inline VECTOR3 vector3(SCALAR x, VECTOR2 yz)
{
	return (VECTOR3){x, yz.x, yz.y};
}


// ----------------------------------------------------------------------------
// create vector 4

inline VECTOR4 vector4(SCALAR x, SCALAR y, SCALAR z, SCALAR w)
{
	return (VECTOR4){x, y, z, w};
}

inline VECTOR4 vector4(VECTOR3 xyz, SCALAR w)
{
	return (VECTOR4){xyz.x, xyz.y, xyz.z, w};
}

inline VECTOR4 vector4(SCALAR x, VECTOR3 yzw)
{
	return (VECTOR4){x, yzw.x, yzw.y, yzw.z};
}

inline VECTOR4 vector4(VECTOR2 xy, VECTOR2 zw)
{
	return (VECTOR4){xy.x, xy.y, zw.x, zw.y};
}

inline VECTOR4 vector4(VECTOR2 xy, SCALAR z, SCALAR w)
{
	return (VECTOR4){xy.x, xy.y, z, w};
}


// ----------------------------------------------------------------------------
// create from array
/*
inline VECTOR2 vector2(const SCALAR* other)
{
	return (VECTOR2)(other[0], other[1]);
}

inline VECTOR3 vector3(const SCALAR* other)
{
	return (VECTOR3)(other[0], other[1], other[2]);
}

inline VECTOR4 vector4(const SCALAR* other)
{
	return (VECTOR4)(other[0], other[1], other[2], other[3]);
}
*/
