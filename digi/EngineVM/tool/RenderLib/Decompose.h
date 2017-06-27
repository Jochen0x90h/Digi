/*
	qrDecomposition(A, out Q, out R): QR decomposition
*/


// ----------------------------------------------------------------------------
/// QR decomposition

#define DIGI_QR_GIVENS(c, s, a, b) \
	SCALAR c = 1.0f; \
	SCALAR s = 0.0f; \
	{ \
		SCALAR r = sqrt(sqr(a) + sqr(b)); \
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
		SCALAR x = a; \
		SCALAR y = b; \
		a = c * x + s * y; \
		b = -s * x + c * y; \
	}

inline void qrDecomposition(MATRIX3X3 Ain, MATRIX3X3& Q, MATRIX3X3& R)
{
	// http://en.wikipedia.org/wiki/QR_decomposition
	// http://en.wikipedia.org/wiki/Givens_rotation
	
	bool detLessZero = det(Ain) < 0.0f;
	MATRIX3X3 A = detLessZero ? -Ain : Ain;
	
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
