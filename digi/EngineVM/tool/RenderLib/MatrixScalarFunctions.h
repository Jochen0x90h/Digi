/**
	diag(matrix) vector of diagonal elements
	det(matrix)
	inv(matrix)
	transpose(matrix)
	adjointTranspose(matrix) calculates the transpose of the adjoint matrix

	<SCALAR>2x2_(...) 2x2 matrix form scalars
	<SCALAR>3x3_(...) 3x3 matrix form scalars
	<SCALAR>4x4_(...) 4x4 matrix form scalars
	
	<SCALAR>Identity() 2x2 identity matrix
	<SCALAR>Identity() 3x3 identity matrix
	<SCALAR>Identity() 4x4 identity matrix
*/

// ----------------------------------------------------------------------------
// create from components

// matrix2x2
inline MATRIX2X2 matrix2x2(
	SCALAR m11, SCALAR m21,
	SCALAR m12, SCALAR m22)
{
	MATRIX2X2 a =
	{
		{m11, m21},
		{m12, m22}
	};
	return a;
}

// matrix3x2
inline MATRIX3X2 matrix3x2(
	SCALAR m11, SCALAR m21, SCALAR m31,
	SCALAR m12, SCALAR m22, SCALAR m32)
{
	MATRIX3X2 a =
	{
		{m11, m21, m31},
		{m12, m22, m32}
	};
	return a;
}

// matrix4x2
inline MATRIX4X2 matrix4x2(
	SCALAR m11, SCALAR m21, SCALAR m31, SCALAR m41,
	SCALAR m12, SCALAR m22, SCALAR m32, SCALAR m42)
{
	MATRIX4X2 a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42}
	};
	return a;
}

// matrix2x3
inline MATRIX2X3 matrix2x3(
	SCALAR m11, SCALAR m21,
	SCALAR m12, SCALAR m22,
	SCALAR m13, SCALAR m23)
{
	MATRIX2X3 a =
	{
		{m11, m21},
		{m12, m22},
		{m13, m23}
	};
	return a;
}

// matrix3x3
inline MATRIX3X3 matrix3x3(
	SCALAR m11, SCALAR m21, SCALAR m31,
	SCALAR m12, SCALAR m22, SCALAR m32,
	SCALAR m13, SCALAR m23, SCALAR m33)
{
	MATRIX3X3 a =
	{
		{m11, m21, m31},
		{m12, m22, m32},
		{m13, m23, m33}
	};
	return a;
}

// matrix4x3
inline MATRIX4X3 matrix4x3(
	SCALAR m11, SCALAR m21, SCALAR m31, SCALAR m41,
	SCALAR m12, SCALAR m22, SCALAR m32, SCALAR m42,
	SCALAR m13, SCALAR m23, SCALAR m33, SCALAR m43)
{
	MATRIX4X3 a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42},
		{m13, m23, m33, m43}
	};
	return a;
}


// matrix2x4
inline MATRIX2X4 matrix2x4(
	SCALAR m11, SCALAR m21,
	SCALAR m12, SCALAR m22,
	SCALAR m13, SCALAR m23,
	SCALAR m14, SCALAR m24)
{
	MATRIX2X4 a =
	{
		{m11, m21},
		{m12, m22},
		{m13, m23},
		{m14, m24}
	};
	return a;
}

// matrix3x4
inline MATRIX3X4 matrix3x4(
	SCALAR m11, SCALAR m21, SCALAR m31,
	SCALAR m12, SCALAR m22, SCALAR m32,
	SCALAR m13, SCALAR m23, SCALAR m33,
	SCALAR m14, SCALAR m24, SCALAR m34)
{
	MATRIX3X4 a =
	{
		{m11, m21, m31},
		{m12, m22, m32},
		{m13, m23, m33},
		{m14, m24, m34}
	};
	return a;
}

// matrix4x4
inline MATRIX4X4 matrix4x4(
	SCALAR m11, SCALAR m21, SCALAR m31, SCALAR m41,
	SCALAR m12, SCALAR m22, SCALAR m32, SCALAR m42,
	SCALAR m13, SCALAR m23, SCALAR m33, SCALAR m43,
	SCALAR m14, SCALAR m24, SCALAR m34, SCALAR m44)
{
	MATRIX4X4 a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42},
		{m13, m23, m33, m43},
		{m14, m24, m34, m44}
	};
	return a;
}


// ----------------------------------------------------------------------------
/// diag(matrix)

// matrix2x2
inline VECTOR2 diag(MATRIX2X2 a)
{
	return vector2(a.x.x, a.y.y);
}

// matrix3x3
inline VECTOR3 diag(MATRIX3X3 a)
{
	return vector3(a.x.x, a.y.y, a.z.z);
}

// matrix4x4
inline VECTOR4 diag(MATRIX4X4 a)
{
	return vector4(a.x.x, a.y.y, a.z.z, a.w.w);
}


// ----------------------------------------------------------------------------
/// det(matrix)

// matrix2x2
inline SCALAR det(MATRIX2X2 a)
{
	return a.x.x * a.y.y - a.y.x * a.x.y;
}

// matrix3x3
inline SCALAR det(MATRIX3X3 a)
{
	return mac(a.x, cross(a.y, a.z));
}

// matrix4x4
inline SCALAR det(MATRIX4X4 a)
{
	// determinant -> signed volume of the parallelepiped spanned by column vectors
	return mac(a.x, cross3(a.y, a.z, a.w));
}


// ----------------------------------------------------------------------------
/// inv(matrix)

// scalar
inline SCALAR inv(SCALAR a)
{
	return SCALAR(1) / a;
}

// matrix2x2
inline MATRIX2X2 inv(MATRIX2X2 a)
{
	VECTOR2 cx = perp(a.y);
	VECTOR2 cy = -perp(a.x);
	
	SCALAR invdet = inv(mac(a.x, cx));

	return matrix2x2(
		invdet * cx.x, invdet * cy.x,
		invdet * cx.y, invdet * cy.y);
}

// matrix3x3
inline MATRIX3X3 inv(MATRIX3X3 a)
{
	VECTOR3 cx = cross(a.y, a.z);
	VECTOR3 cy = cross(a.z, a.x);
	VECTOR3 cz = cross(a.x, a.y);
	
	SCALAR invdet = inv(mac(a.x, cx));

	return matrix3x3(
		invdet * cx.x, invdet * cy.x, invdet * cz.x,
		invdet * cx.y, invdet * cy.y, invdet * cz.y,
		invdet * cx.z, invdet * cy.z, invdet * cz.z);
}
	
// matrix4x4
inline MATRIX4X4 inv(MATRIX4X4 a)
{
	VECTOR4 cx =  cross3(a.y, a.z, a.w);
	VECTOR4 cy = -cross3(a.z, a.w, a.x);
	VECTOR4 cz =  cross3(a.w, a.x, a.y);
	VECTOR4 cw = -cross3(a.x, a.y, a.z);

	SCALAR invdet = inv(mac(a.x, cx)); 
	
	return matrix4x4(
		invdet * cx.x, invdet * cy.x, invdet * cz.x, invdet * cw.x,
		invdet * cx.y, invdet * cy.y, invdet * cz.y, invdet * cw.y,
		invdet * cx.z, invdet * cy.z, invdet * cz.z, invdet * cw.z,
		invdet * cx.w, invdet * cy.w, invdet * cz.w, invdet * cw.w);
}


// ----------------------------------------------------------------------------
/// transpose(matrix)

// matrix2x2
inline MATRIX2X2 transpose(MATRIX2X2 a)
{
	return matrix2x2(
		a.x.x, a.y.x,
		a.x.y, a.y.y);
}

// matrix3x2
inline MATRIX2X3 transpose(MATRIX3X2 a)
{
	return matrix2x3(
		a.x.x, a.y.x,
		a.x.y, a.y.y,
		a.x.z, a.y.z);
}

// matrix4x2
inline MATRIX2X4 transpose(MATRIX4X2 a)
{
	return matrix2x4(
		a.x.x, a.y.x,
		a.x.y, a.y.y,
		a.x.z, a.y.z,
		a.x.w, a.y.w);
}

// matrix2x3
inline MATRIX3X2 transpose(MATRIX2X3 a)
{
	return matrix3x2(
		a.x.x, a.y.x, a.z.x,
		a.x.y, a.y.y, a.z.y);
}

// matrix3x3
inline MATRIX3X3 transpose(MATRIX3X3 a)
{
	return matrix3x3(
		a.x.x, a.y.x, a.z.x,
		a.x.y, a.y.y, a.z.y,
		a.x.z, a.y.z, a.z.z);
}

// matrix4x3
inline MATRIX3X4 transpose(MATRIX4X3 a)
{
	return matrix3x4(
		a.x.x, a.y.x, a.z.x,
		a.x.y, a.y.y, a.z.y,
		a.x.z, a.y.z, a.z.z,
		a.x.w, a.y.w, a.z.w);
}

// matrix2x4
inline MATRIX4X2 transpose(MATRIX2X4 a)
{
	return matrix4x2(
		a.x.x, a.y.x, a.z.x, a.w.x,
		a.x.y, a.y.y, a.z.y, a.w.y);
}

// matrix3x4
inline MATRIX4X3 transpose(MATRIX3X4 a)
{
	return matrix4x3(
		a.x.x, a.y.x, a.z.x, a.w.x,
		a.x.y, a.y.y, a.z.y, a.w.y,
		a.x.z, a.y.z, a.z.z, a.w.z);
}

// matrix4x4
inline MATRIX4X4 transpose(MATRIX4X4 a)
{
	return matrix4x4(
		a.x.x, a.y.x, a.z.x, a.w.x,
		a.x.y, a.y.y, a.z.y, a.w.y,
		a.x.z, a.y.z, a.z.z, a.w.z,
		a.x.w, a.y.w, a.z.w, a.w.w);
}


// ----------------------------------------------------------------------------
/// adjointTranspose(matrix)

/*
// matrix3x3
MATRIX3X3 adjointTranspose(MATRIX3X3 a)
{
	return matrix3x3(
		(a.y.y * a.z.z - a.y.z * a.z.y),
		(a.y.z * a.z.x - a.y.x * a.z.z),
		(a.y.x * a.z.y - a.y.y * a.z.x),

		(a.z.y * a.x.z - a.z.z * a.x.y),
		(a.z.z * a.x.x - a.z.x * a.x.z),
		(a.z.x * a.x.y - a.z.y * a.x.x),

		(a.x.y * a.y.z - a.x.z * a.y.y),
		(a.x.z * a.y.x - a.x.x * a.y.z),
		(a.x.x * a.y.y - a.x.y * a.y.x));
}
*/


// ----------------------------------------------------------------------------
// identity

inline MATRIX2X2 CONCAT(MATRIX2X2, Identity)()
{
	return matrix2(
		(VECTOR2)(1, 0),
		(VECTOR2)(0, 1)
	);
}

inline MATRIX3X3 CONCAT(MATRIX3X3, Identity)()
{
	return matrix3(
		(VECTOR3)(1, 0, 0),
		(VECTOR3)(0, 1, 0),
		(VECTOR3)(0, 0, 1)
	);
}

inline MATRIX4X4 CONCAT(MATRIX4X4, Identity)()
{
	return matrix4(
		(VECTOR4)(1, 0, 0, 0),
		(VECTOR4)(0, 1, 0, 0),
		(VECTOR4)(0, 0, 1, 0),
		(VECTOR4)(0, 0, 0, 1)
	);
}
