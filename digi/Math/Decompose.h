#ifndef digi_Math_Decompose_h
#define digi_Math_Decompose_h

#include "Matrix3.h"
#include "Quaternion.h"


/*
	polarDecomposition(A, out Q): polar decomposition by Ken Shoemake A = Q*S (Q orthonormal rotation matix, S stretch matrix)
	polarDecomposition(A, out Q, out S): polar decomposition
	qrDecomposition(A, out Q, out R): QR decomposition
*/

namespace digi {

/// @addtogroup Math
/// @{

// ----------------------------------------------------------------------------
/// polar decomposition

template <typename Type>
Type polarDecompositionInternal(DIGI_MATRIX3X3(Type)& Mk)
{
	// Mk - IN/OUT at start transpose of A, at exit rotational component of A (i.e. Q)
	
	DIGI_MATRIX3X3(Type) MadjTk, Ek;
	Type determ, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;

	Type M_one = norm1(Mk);
	Type M_inf = normInf(Mk);
	do 
	{
		MadjTk = adjointTranspose(Mk);
		determ = dot(Mk.x, MadjTk.x);
		if (determ == 0)
			return 0;// {do_rank2(Mk, MadjTk, Mk); break;}
		MadjT_one = norm1(MadjTk);
		MadjT_inf = normInf(MadjTk);
		gamma = sqrt(sqrt((MadjT_one * MadjT_inf) / (M_one * M_inf)) / abs(determ));
		g1 = gamma * 0.5f;
		g2 = 0.5f / (gamma * determ);
		Ek = Mk;
		Mk = Mk * g1 + g2 * MadjTk;
		Ek = Ek - Mk;
		E_one = norm1(Ek);
		M_one = norm1(Mk);
		M_inf = normInf(Mk);
	} while (E_one > M_one * 1e-6f);

	return determ;
}

template <typename Type>
Type polarDecomposition(const DIGI_MATRIX3X3(Type)& A, DIGI_MATRIX3X3(Type)& Q)
{
	DIGI_MATRIX3X3(Type) Mk = transpose(A);
	Type det = polarDecompositionInternal(Mk);
	Q = transpose(Mk);
	return det;
}

template <typename Type>
Type polarDecomposition(const DIGI_MATRIX3X3(Type)& A, DIGI_MATRIX3X3(Type)& Q, DIGI_MATRIX3X3(Type)& S)
{
	DIGI_MATRIX3X3(Type) Mk = transpose(A);
	Type det = polarDecompositionInternal(Mk);
	S = Mk * A;
	Q = transpose(Mk);
	return det;
}


// ----------------------------------------------------------------------------
/// QR decomposition

#define DIGI_QR_GIVENS(c, s, a, b) \
	Type c = 1.0f; \
	Type s = 0.0f; \
	{ \
		Type r = sqrt(sqr(a) + sqr(b)); \
		if (r > 1e-6f) \
		{ \
			c = a / r; \
			s = b / r; \
		} \
		a = r; \
		b = 0.0f; \
	}

#define DIGI_QR_ROTATE(c, s, a, b) \
	{ \
		Type x = a; \
		Type y = b; \
		a = c * x + s * y; \
		b = -s * x + c * y; \
	}

template <typename Type>
void qrDecomposition(const DIGI_MATRIX3X3(Type)& Ain, DIGI_MATRIX3X3(Type)& Q, DIGI_MATRIX3X3(Type)& R)
{
	// http://en.wikipedia.org/wiki/QR_decomposition
	// http://en.wikipedia.org/wiki/Givens_rotation
	
	bool detLessZero = det(Ain) < 0.0f;
	DIGI_MATRIX3X3(Type) A = detLessZero ? -Ain : Ain;
	
	// zero a31
	DIGI_QR_GIVENS(c1, s1, A.x.y, A.x.z);
	DIGI_QR_ROTATE(c1, s1, A.y.y, A.y.z);
	DIGI_QR_ROTATE(c1, s1, A.z.y, A.z.z);
	
	// zero a21
	DIGI_QR_GIVENS(c2, s2, A.x.x, A.x.y);
	DIGI_QR_ROTATE(c2, s2, A.y.x, A.y.y);
	DIGI_QR_ROTATE(c2, s2, A.z.x, A.z.y);
	
	// zero a32
	DIGI_QR_GIVENS(c3, s3, A.y.y, A.y.z);
	DIGI_QR_ROTATE(c3, s3, A.z.y, A.z.z);
	
	R = detLessZero ? -A : A;
	
	/*
		Q1^T * Q2^T * Q3^T =

		|1   0   0| | c2 -s2  0| |1   0   0|
		|0  c1 -s1|*| s2  c2  0|*|0  c3 -s3| =
		|0  s1  c1| |  0   0  1| |0  s3  c3|

		|   c2   -s2   0| |1   0   0|
		| c1s2  c1c2 -s1|*|0  c3 -s3| =
		| s1s2  s1c2  c1| |0  s3  c3|

		|   c2        -s2c3         s2s3|
		| c1s2  c1c2c3-s1s3 -c1c2s3-s1c3|
		| s1s2  s1c2c3+c1s3 -s1c2s3+c1c3|
	*/
	Q = matrix3x3(
		c2,     c1*s2,           s1*s2,
		-s2*c3, c1*c2*c3-s1*s3,  s1*c2*c3+c1*s3,
		s2*s3,  -c1*c2*s3-s1*c3, -s1*c2*s3+c1*c3);
}

/// @}

} // namespace digi

#endif
