/*
	this file contains some utility functions:

	clamp(x, minValue, maxValue)
	saturate(x) clamp(x, 0, 1)

	step(edge, x)
	linstep(start, end, x)
	smoothstep(start, end, x)

	lerp(p1, p2, a)
	hermite(p1, t1, p2, t2, a)
*/

// ----------------------------------------------------------------------------
/// clamp(a, minValue, maxValue)

inline VALUE clamp(VALUE x, VALUE minValue, VALUE maxValue)
{
	return min(max(x, minValue), maxValue);
}

#if VALUE != SCALAR
inline VALUE clamp(VALUE a, SCALAR minValue, SCALAR maxValue)
{
	return min(max(a, minValue), maxValue);
}
#endif


// ----------------------------------------------------------------------------
/// saturate(x)

inline VALUE saturate(VALUE x)
{
	return clamp(x, SCALAR(0), SCALAR(1));
}

// ----------------------------------------------------------------------------
/// step(edge, x)

inline VALUE step(VALUE edge, VALUE x)
{
	return select(VALUE(1), VALUE(0), x < edge);
}

#if VALUE != SCALAR
inline VALUE step(SCALAR edge, VALUE x)
{
	return select(VALUE(1), VALUE(0), x < edge);
}
#endif


// ----------------------------------------------------------------------------
// linstep(start, end, x)

inline VALUE linstep(VALUE start, VALUE end, VALUE x)
{
	VALUE cx = clamp(x, start, end);
	VALUE d = (cx - start) / (end - start);
	return d;
}


// ----------------------------------------------------------------------------
// smoothstep(start, end, x)

inline VALUE smoothstep(VALUE start, VALUE end, VALUE x)
{
	VALUE cx = clamp(x, start, end);
	VALUE d = (cx - start) / (end - start);
	return (VALUE(-2) * d + VALUE(3)) * d * d;
}


// ----------------------------------------------------------------------------
// lerp(p1, p2, a)

inline VALUE lerp(VALUE p1, VALUE p2, VALUE a)
{
	return p1 + a * (p2 - p1);
}


#if VALUE != SCALAR
inline VALUE lerp(VALUE p1, VALUE p2, SCALAR a)
{
	return p1 + a * (p2 - p1);
}
#endif


// ----------------------------------------------------------------------------
// hermite(p1, t1, p2, t2, a)

inline VALUE hermite(VALUE p1, VALUE t1, VALUE p2, VALUE t2, SCALAR a)
{
	SCALAR a2 = a * a;
	SCALAR a3 = a2 * a;

	return p1 * (2.0f * a3 - 3.0f * a2 + 1.0f)
		+ t1 * (a3 - 2.0f * a2 + a)
		+ p2 * (-2.0f * a3 + 3.0f * a2)
		+ t2 * (a3 - a2);
}
