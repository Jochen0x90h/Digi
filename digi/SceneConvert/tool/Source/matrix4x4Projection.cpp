Projection p;
float aspect;

float4x4 r;

// functions that are not in the VM math lib
inline float4x4 matrix4x4Orthographic(float2 f, float2 o, float znear, float zfar)
{
	float _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		f.x,     0.0f, 0.0f,               0.0f,
		0.0f, f.y,     0.0f,               0.0f,
		0.0f, 0.0f, 2.0f * _n_f,           0.0f,
		o.x,     o.y,     (zfar + znear) * _n_f, 1.0f);
}

inline float4x4 matrix4x4Perspective(float2 f, float2 o, float znear, float zfar)
{
	float _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		f.x,     0.0f, 0.0f,                    0.0f,
		0.0f, f.y,     0.0f,                    0.0f,
		o.x,     o.y,     (zfar + znear) * _n_f,      -1.0f,
		0.0f, 0.0f, 2.0f * zfar * znear * _n_f, 0.0f);
}


extern "C" void main()
{
	// see wikipedia article on focal length.
	// angleOfView = atan(filmSize/2 / focalLength) * 2 * (180 / pi)

	float2 filmSize = p.filmSize;
	
	bool fovX;
	float mode = p.mode;
	float m = mode * mode;
	if (m < 2.0f)
	{
		// fill
		// x aligned fov when the film matches the window in x-direction and exceeds it in y-direction
		// y aligned fov when the film matches the window in y-direction and exceeds it in x-direction
		fovX = aspect * p.filmSize.y > p.filmSize.x;
	}
	else if (m < 5.0f)
	{
		// horizontal
		// always x aligned fov
		fovX = true;
	}
	else if (m < 10.0f)
	{
		// vertical
		// always y aligned fov
		fovX = false;
	}
	else
	{	
		// overscan
		// x aligned fov when the film matches the window in y-direction and does not fill it in x-direction
		// y aligned fov when the film matches the window in x-direction and does not fill it in y-direction
		fovX = aspect * p.filmSize.y < p.filmSize.x;
	}
	
	if (fovX)
	{
		// fov is fixed in x-direction
		filmSize.y = filmSize.x / aspect;
	}
	else
	{
		// fov is fixed in y-direction
		filmSize.x = filmSize.y * aspect;
	}

	float2 f = 2.0f * p.scale / filmSize;
	float2 o = 2.0f * p.filmOffset / filmSize;

	// create orthographic or perspective matrix
	if (mode < 0)
		r = matrix4x4Orthographic(f, o, p.nearClipPlane, p.farClipPlane);
	else
		r = matrix4x4Perspective(f, o, p.nearClipPlane, p.farClipPlane);
}
