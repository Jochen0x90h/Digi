
float fabs(float x)
{
	union {
		int i;
		float f;
	} xx;

	xx.f = x;
	xx.i = xx.i & 0x7FFFFFFF;
	return xx.f;	
}

float floor(float x)
{
	int n;
	float r;	
	n = (int) x;
	r = (float) n;
	r = r - (r > x);
	return r;
}

float ceil(float x)
{
	int n;
	float r;	
	n = (int) x;
	r = (float) n;
	r = r + (x > r);
	return r;
}

float fmod(float x, float y)
{
	int n;
	union {
		float f;
		int   i;
	} yinv;
	float a;
	
	//fast reciporical approximation (4x Newton)
	yinv.f = y;
	n = 0x3F800000 - (yinv.i & 0x7F800000);
	yinv.i = yinv.i + n;
	yinv.f = 1.41176471f - 0.47058824f * yinv.f;
	yinv.i = yinv.i + n;
	a = 2.0 - yinv.f * y;
	yinv.f = yinv.f * a;	
	a = 2.0 - yinv.f * y;
	yinv.f = yinv.f * a;
	a = 2.0 - yinv.f * y;
	yinv.f = yinv.f * a;
	a = 2.0 - yinv.f * y;
	yinv.f = yinv.f * a;
	
	n = (int)(x * yinv.f);
	x = x - ((float)n) * y;
	return x;	
}

float sqrt(float x)
{
	float b, c;
	int m;
	union {
		float 	f;
		int 	i;
	} a;
	
	//fast invsqrt approx
	a.f = x;
	a.i = 0x5F3759DF - (a.i >> 1);		//VRSQRTE
	c = x * a.f;
	b = (3.0f - c * a.f) * 0.5;		//VRSQRTS
	a.f = a.f * b;		
	c = x * a.f;
	b = (3.0f - c * a.f) * 0.5;
	a.f = a.f * b;	

	//fast inverse approx
	x = a.f;
	m = 0x3F800000 - (a.i & 0x7F800000);
	a.i = a.i + m;
	a.f = 1.41176471f - 0.47058824f * a.f;
	a.i = a.i + m;
	b = 2.0 - a.f * x;
	a.f = a.f * b;	
	b = 2.0 - a.f * x;
	a.f = a.f * b;

	return a.f;
}

static const float __expf_rng[2] = {
	1.442695041f,
	0.693147180f
};

static const float __expf_lut[8] = {
	0.9999999916728642f,		//p0
	0.04165989275009526f, 	//p4
	0.5000006143673624f, 	//p2
	0.0014122663401803872f, 	//p6
	1.000000059694879f, 		//p1
	0.008336936973260111f, 	//p5
	0.16666570253074878f, 	//p3
	0.00019578093328483123f	//p7
};

float exp(float x)
{
	float a, b, c, d, xx;
	int m;
	
	union {
		float   f;
		int 	i;
	} r;
		
	//Range Reduction:
	m = (int) (x * __expf_rng[0]);
	x = x - ((float) m) * __expf_rng[1];	
	
	//Taylor Polynomial (Estrins)
	a = (__expf_lut[4] * x) + (__expf_lut[0]);
	b = (__expf_lut[6] * x) + (__expf_lut[2]);
	c = (__expf_lut[5] * x) + (__expf_lut[1]);
	d = (__expf_lut[7] * x) + (__expf_lut[3]);
	xx = x * x;
	a = a + b * xx; 
	c = c + d * xx;
	xx = xx* xx;
	r.f = a + c * xx; 
	
	//multiply by 2 ^ m 
	m = m << 23;
	r.i = r.i + m;

	return r.f;	
}

static const float __powf_rng[2] = {
	1.442695041f,
	0.693147180f
};

static const float __powf_lut[16] = {
	-2.295614848256274f, 	//p0	log
	-2.470711633419806f, 	//p4
	-5.686926051100417f, 	//p2
	-0.165253547131978f, 	//p6
	+5.175912446351073f, 	//p1
	+0.844006986174912f, 	//p5
	+4.584458825456749f, 	//p3
	+0.014127821926000f,		//p7
	0.9999999916728642f,		//p0	exp
	0.04165989275009526f, 	//p4
	0.5000006143673624f, 	//p2
	0.0014122663401803872f, 	//p6
	1.000000059694879f, 		//p1
	0.008336936973260111f, 	//p5
	0.16666570253074878f, 	//p3
	0.00019578093328483123f	//p7
};

float pow(float x, float n)
{
	float a, b, c, d, xx;
	int m;
	
	union {
		float   f;
		int 	i;
	} r;
	
	//extract exponent
	r.f = x;
	m = (r.i >> 23);
	m = m - 127;
	r.i = r.i - (m << 23);
	
	//Taylor Polynomial (Estrins)
	xx = r.f * r.f;
	a = (__powf_lut[4] * r.f) + (__powf_lut[0]);
	b = (__powf_lut[6] * r.f) + (__powf_lut[2]);
	c = (__powf_lut[5] * r.f) + (__powf_lut[1]);
	d = (__powf_lut[7] * r.f) + (__powf_lut[3]);
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//add exponent
	r.f = r.f + ((float) m) * __powf_rng[1];

	r.f = r.f * n;


	//Range Reduction:
	m = (int) (r.f * __powf_rng[0]);
	r.f = r.f - ((float) m) * __powf_rng[1];	
	
	//Taylor Polynomial (Estrins)
	a = (__powf_lut[12] * r.f) + (__powf_lut[8]);
	b = (__powf_lut[14] * r.f) + (__powf_lut[10]);
	c = (__powf_lut[13] * r.f) + (__powf_lut[9]);
	d = (__powf_lut[15] * r.f) + (__powf_lut[11]);
	xx = r.f * r.f;
	a = a + b * xx; 
	c = c + d * xx;
	xx = xx* xx;
	r.f = a + c * xx; 
	
	//multiply by 2 ^ m 
	m = m << 23;
	r.i = r.i + m;

	return r.f;

}

static const float __logf_rng =  0.693147180f;

static const float __logf_lut[8] = {
	-2.295614848256274f, 	//p0
	-2.470711633419806f, 	//p4
	-5.686926051100417f, 	//p2
	-0.165253547131978f, 	//p6
	+5.175912446351073f, 	//p1
	+0.844006986174912f, 	//p5
	+4.584458825456749f, 	//p3
	+0.014127821926000f	//p7
};

float log(float x)
{
	float a, b, c, d, xx;
	int m;
	
	union {
		float   f;
		int 	i;
	} r;
	
	//extract exponent
	r.f = x;
	m = (r.i >> 23);
	m = m - 127;
	r.i = r.i - (m << 23);
		
	//Taylor Polynomial (Estrins)
	xx = r.f * r.f;
	a = (__logf_lut[4] * r.f) + (__logf_lut[0]);
	b = (__logf_lut[6] * r.f) + (__logf_lut[2]);
	c = (__logf_lut[5] * r.f) + (__logf_lut[1]);
	d = (__logf_lut[7] * r.f) + (__logf_lut[3]);
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//add exponent
	r.f = r.f + ((float) m) * __logf_rng;

	return r.f;
}

static const float __log10f_rng =  0.3010299957f;

static const float __log10f_lut[8] = {
	-0.99697286229624f, 		//p0
	-1.07301643912502f, 		//p4
	-2.46980061535534f, 		//p2
	-0.07176870463131f, 		//p6
	2.247870219989470f, 		//p1
	0.366547581117400f, 		//p5
	1.991005185100089f, 		//p3
	0.006135635201050f,		//p7
};

float log10(float x)
{
	float a, b, c, d, xx;
	int m;
	
	union {
		float   f;
		int 	i;
	} r;
	
	//extract exponent
	r.f = x;
	m = (r.i >> 23);
	m = m - 127;
	r.i = r.i - (m << 23);
		
	//Taylor Polynomial (Estrins)
	xx = r.f * r.f;
	a = (__log10f_lut[4] * r.f) + (__log10f_lut[0]);
	b = (__log10f_lut[6] * r.f) + (__log10f_lut[2]);
	c = (__log10f_lut[5] * r.f) + (__log10f_lut[1]);
	d = (__log10f_lut[7] * r.f) + (__log10f_lut[3]);
	a = a + b * xx;
	c = c + d * xx;
	xx = xx * xx;
	r.f = a + c * xx;

	//add exponent
	r.f = r.f + ((float) m) * __log10f_rng;

	return r.f;
}

static const float __sinf_rng[2] = {
	2.0f / M_PI,
	M_PI / 2.0f
};

static const float __sinf_lut[4] = {
	-0.00018365f,	//p7
	-0.16664831f,	//p3
	+0.00830636f,	//p5
	+0.99999661f,	//p1
};

float sin(float x)
{
	union {
		float 	f;
		int 	i;
	} ax;
	
	float r, a, b, xx;
	int m, n;
	
	ax.f = fabs(x);

	//Range Reduction:
	m = (int) (ax.f * __sinf_rng[0]);	
	ax.f = ax.f - (((float)m) * __sinf_rng[1]);

	//Test Quadrant
	n = m & 1;
	ax.f = ax.f - n * __sinf_rng[1];	
	m = m >> 1;
	n = n ^ m;
	m = (x < 0.0);
	n = n ^ m;	
	n = n << 31;
	ax.i = ax.i ^ n; 

	//Taylor Polynomial (Estrins)
	xx = ax.f * ax.f;	
	a = (__sinf_lut[0] * ax.f) * xx + (__sinf_lut[2] * ax.f);
	b = (__sinf_lut[1] * ax.f) * xx + (__sinf_lut[3] * ax.f);
	xx = xx * xx;
	r = b + a * xx;

	return r;
}

float cos(float x)
{
	return sin(x + M_PI_2);
}


static const float __tanf_rng[2] = {
        2.0f / M_PI,
        M_PI / 2.0f
};

static const float __tanf_lut[4] = {
        -0.00018365f,   //p7
        -0.16664831f,   //p3
        +0.00830636f,   //p5
        +0.99999661f,   //p1
};
 
float tan(float x)
{
	union {
		float f;
		int i;
	} ax, c;

	float r, a, b, xx, cc, cx;
	int m;

	ax.f = fabs(x);

	//Range Reduction:
	m = (int) (ax.f * __tanf_rng[0]);       
	ax.f = ax.f - (((float)m) * __tanf_rng[1]);

	//Test Quadrant
	ax.f = ax.f - (m & 1) * __tanf_rng[1];
	ax.i = ax.i ^ ((*(int*)&x) & 0x80000000);
		
	//Taylor Polynomial (Estrins)
	xx = ax.f * ax.f;       
	a = (__tanf_lut[0] * ax.f) * xx + (__tanf_lut[2] * ax.f);
	b = (__tanf_lut[1] * ax.f) * xx + (__tanf_lut[3] * ax.f);
	xx = xx * xx;
	r = b + a * xx;

	//cosine
	c.f = 1.0 - r * r;

	//fast invsqrt approximation (2x newton iterations)
	cc = c.f;
	c.i = 0x5F3759DF - (c.i >> 1);          //VRSQRTE
	cx = cc * c.f;
	a = (3.0f - cx * c.f) / 2;                      //VRSQRTS
	c.f = c.f * a;          
	cx = cc * c.f;
	a = (3.0f - cx * c.f) / 2;
	c.f = c.f * a;      

	r = r * c.f;

	return r;
}


static const float __atan2f_lut[4] = {
	-0.0443265554792128f,	//p7
	-0.3258083974640975f,	//p3
	+0.1555786518463281f,	//p5
	+0.9997878412794807f  	//p1
}; 
 
static const float __atan2f_pi_2 = M_PI_2;

float atan2(float y, float x)
{
	float a, b, c, r, xx;
	int m;
	union {
		float f;
		int i;
	} xinv;

	//fast inverse approximation (2x newton)
	xx = fabs(x);
	xinv.f = xx;
	m = 0x3F800000 - (xinv.i & 0x7F800000);
	xinv.i = xinv.i + m;
	xinv.f = 1.41176471f - 0.47058824f * xinv.f;
	xinv.i = xinv.i + m;
	b = 2.0 - xinv.f * xx;
	xinv.f = xinv.f * b;	
	b = 2.0 - xinv.f * xx;
	xinv.f = xinv.f * b;
	
	c = fabs(y * xinv.f);

	//fast inverse approximation (2x newton)
	xinv.f = c;
	m = 0x3F800000 - (xinv.i & 0x7F800000);
	xinv.i = xinv.i + m;
	xinv.f = 1.41176471f - 0.47058824f * xinv.f;
	xinv.i = xinv.i + m;
	b = 2.0 - xinv.f * c;
	xinv.f = xinv.f * b;	
	b = 2.0 - xinv.f * c;
	xinv.f = xinv.f * b;
	
	//if |x| > 1.0 -> ax = -1/ax, r = pi/2
	xinv.f = xinv.f + c;
	a = (c > 1.0f);
	c = c - a * xinv.f;
	r = a * __atan2f_pi_2;
	
	//polynomial evaluation
	xx = c * c;	
	a = (__atan2f_lut[0] * c) * xx + (__atan2f_lut[2] * c);
	b = (__atan2f_lut[1] * c) * xx + (__atan2f_lut[3] * c);
	xx = xx * xx;
	r = r + a * xx; 
	r = r + b;

	//determine quadrant and test for small x.
	b = M_PI;
	b = b - 2.0f * r;
	r = r + (x < 0.0f) * b;
	b = (fabs(x) < 0.000001f);
	c = !b;
	r = c * r;
	r = r + __atan2f_pi_2 * b;
	b = r + r;
	r = r - (y < 0.0f) * b;
	
	return r; 
}
