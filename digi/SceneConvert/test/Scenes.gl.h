// mesh, shader 'shader1', layer 'color'
static const char* vsm_shader1_0 =
	"#version 120\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _h[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _i;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _h[0].xyz * a.x + _h[1].xyz * a.y + _h[2].xyz * a.z + _h[3].xyz;\n" // 10
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"	_i = _1;\n"
	"}\n"
;
static const char* psm_shader1_0 =
	"#version 120\n"
	"uniform vec4 _f;\n"
	"uniform sampler2D _g;\n"
	"varying vec2 _i;\n"
	"void main()\n"
	"{\n"
	"	vec4 a = texture2D(_g, _i * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	vec3 b = _f.xyz * a.xyz * a.w;\n"
	"	if (max(max(b.x, b.y), b.z) < 0.01 && a.w < 0.01)\n"
	"	{\n" // 10
	"		discard;\n"
	"	}\n"
	"	gl_FragColor = vec4(b, a.w);\n"
	"}\n"
;

// mesh, shader 'shader1', layer 'pick'
static const char* vsm_shader1_1 =
	"#version 120\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _h[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _j;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _h[0].xyz * a.x + _h[1].xyz * a.y + _h[2].xyz * a.z + _h[3].xyz;\n" // 10
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"	_j = _1;\n"
	"}\n"
;
static const char* psm_shader1_1 =
	"#version 120\n"
	"uniform vec4 _f;\n"
	"uniform sampler2D _g;\n"
	"uniform vec4 _i;\n"
	"varying vec2 _j;\n"
	"void main()\n"
	"{\n"
	"	vec4 a = texture2D(_g, _j * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	vec3 b = _f.xyz * a.xyz * a.w;\n"
	"	if (max(max(b.x, b.y), b.z) < 0.01 && a.w < 0.01)\n" // 10
	"	{\n"
	"		discard;\n"
	"	}\n"
	"	gl_FragColor = _i;\n"
	"}\n"
;

// mesh, shader 'shader3', layer 'color'
static const char* vsm_shader3_0 =
	"#version 120\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _g[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _h;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _g[0].xyz * a.x + _g[1].xyz * a.y + _g[2].xyz * a.z + _g[3].xyz;\n" // 10
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"	_h = _1;\n"
	"}\n"
;
static const char* psm_shader3_0 =
	"#version 120\n"
	"uniform sampler2D _f;\n"
	"varying vec2 _h;\n"
	"void main()\n"
	"{\n"
	"	vec4 a = texture2D(_f, _h * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	if (max(max(a.x, a.y), a.z) < 0.01 && a.x < 0.01)\n"
	"	{\n"
	"		discard;\n"
	"	}\n" // 10
	"	gl_FragColor = a.xyzx;\n"
	"}\n"
;

// mesh, shader 'shader3', layer 'pick'
static const char* vsm_shader3_1 =
	"#version 120\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _g[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _i;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _g[0].xyz * a.x + _g[1].xyz * a.y + _g[2].xyz * a.z + _g[3].xyz;\n" // 10
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"	_i = _1;\n"
	"}\n"
;
static const char* psm_shader3_1 =
	"#version 120\n"
	"uniform sampler2D _f;\n"
	"uniform vec4 _h;\n"
	"varying vec2 _i;\n"
	"void main()\n"
	"{\n"
	"	vec4 a = texture2D(_f, _i * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	if (max(max(a.x, a.y), a.z) < 0.01 && a.x < 0.01)\n"
	"	{\n"
	"		discard;\n" // 10
	"	}\n"
	"	gl_FragColor = _h;\n"
	"}\n"
;

// mesh, shader 'shader2', layer 'color'
static const char* vsm_shader2_0 =
	"#version 120\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _g[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _h;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _g[0].xyz * a.x + _g[1].xyz * a.y + _g[2].xyz * a.z + _g[3].xyz;\n" // 10
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"	_h = _1;\n"
	"}\n"
;
static const char* psm_shader2_0 =
	"#version 120\n"
	"uniform sampler2D _f;\n"
	"varying vec2 _h;\n"
	"void main()\n"
	"{\n"
	"	vec4 a = texture2D(_f, _h * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	gl_FragColor = vec4(a.xyz, 1.0);\n"
	"}\n"
;

// mesh, shader 'shader2', layer 'pick'
static const char* vsm_shader2_1 =
	"#version 120\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _f[4];\n"
	"attribute vec3 _0;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _f[0].xyz * a.x + _f[1].xyz * a.y + _f[2].xyz * a.z + _f[3].xyz;\n"
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"}\n" // 10
;
static const char* psm_shader2_1 =
	"#version 120\n"
	"uniform vec4 _g;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = _g;\n"
	"}\n"
;

// sprites, shader 'shader2', layer 'color'
static const char* vsp_shader2_0 =
	"#version 120\n"
	"uniform vec4 _e[5];\n"
	"uniform vec4 _g[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec3 _1;\n"
	"varying vec4 _h;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _1;\n" // 10
	"	vec3 c = _g[0].xyz * a.x + _g[1].xyz * a.y + _g[2].xyz * a.z + _g[3].xyz;\n"
	"	vec4 d = _e[0];\n"
	"	vec4 e = _e[1];\n"
	"	vec4 f = d * c.x + e * c.y + _e[2] * c.z + _e[3];\n"
	"	gl_Position = f;\n"
	"	vec2 g = (d * b.x + e * b.y).xy / f.w * _e[4].xy;\n"
	"	float h = cos(b.z);\n"
	"	float i = sin(b.z);\n"
	"	float j = h * g.x;\n"
	"	float k = i * g.x;\n" // 20
	"	float l = g.y * -i;\n"
	"	float m = h * g.y;\n"
	"	vec2 n = vec2(j, k);\n"
	"	vec2 o = vec2(l, m);\n"
	"	vec2 p = abs(n + o);\n"
	"	vec2 q = abs(n - o);\n"
	"	float r = max(max(p.x, q.x), max(p.y, q.y));\n"
	"	gl_PointSize = r;\n"
	"	float s = -l;\n"
	"	vec2 t = -vec2(k, -j);\n" // 30
	"	vec2 u = n * vec2(m, s);\n"
	"	float v = 1.0 / (u.x + u.y);\n"
	"	_h.xy = vec2(v * m, v * t.x) * r;\n"
	"	_h.zw = -(vec2(v * s, v * t.y) * r);\n"
	"}\n"
;
static const char* psp_shader2_0 =
	"#version 120\n"
	"uniform sampler2D _f;\n"
	"varying vec4 _h;\n"
	"void main()\n"
	"{\n"
	"	vec2 a = gl_PointCoord + -0.5;\n"
	"	vec2 b = (_h.xy) * a.x + (_h.zw) * a.y;\n"
	"	vec2 c = abs(b);\n"
	"	if (!(max(c.x, c.y) < 0.5))\n"
	"	{\n" // 10
	"		discard;\n"
	"	}\n"
	"	vec4 d = texture2D(_f, (b + 0.5) * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	gl_FragColor = vec4(d.xyz, 1.0);\n"
	"}\n"
;

// sprites, shader 'shader2', layer 'pick'
static const char* vsp_shader2_1 =
	"#version 120\n"
	"uniform vec4 _e[5];\n"
	"uniform vec4 _f[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec3 _1;\n"
	"varying vec4 _h;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _1;\n" // 10
	"	vec3 c = _f[0].xyz * a.x + _f[1].xyz * a.y + _f[2].xyz * a.z + _f[3].xyz;\n"
	"	vec4 d = _e[0];\n"
	"	vec4 e = _e[1];\n"
	"	vec4 f = d * c.x + e * c.y + _e[2] * c.z + _e[3];\n"
	"	gl_Position = f;\n"
	"	vec2 g = (d * b.x + e * b.y).xy / f.w * _e[4].xy;\n"
	"	float h = cos(b.z);\n"
	"	float i = sin(b.z);\n"
	"	float j = h * g.x;\n"
	"	float k = i * g.x;\n" // 20
	"	float l = g.y * -i;\n"
	"	float m = h * g.y;\n"
	"	vec2 n = vec2(j, k);\n"
	"	vec2 o = vec2(l, m);\n"
	"	vec2 p = abs(n + o);\n"
	"	vec2 q = abs(n - o);\n"
	"	float r = max(max(p.x, q.x), max(p.y, q.y));\n"
	"	gl_PointSize = r;\n"
	"	float s = -l;\n"
	"	vec2 t = -vec2(k, -j);\n" // 30
	"	vec2 u = n * vec2(m, s);\n"
	"	float v = 1.0 / (u.x + u.y);\n"
	"	_h.xy = vec2(v * m, v * t.x) * r;\n"
	"	_h.zw = -(vec2(v * s, v * t.y) * r);\n"
	"}\n"
;
static const char* psp_shader2_1 =
	"#version 120\n"
	"uniform vec4 _g;\n"
	"varying vec4 _h;\n"
	"void main()\n"
	"{\n"
	"	vec2 a = gl_PointCoord + -0.5;\n"
	"	vec2 b = abs((_h.xy) * a.x + (_h.zw) * a.y);\n"
	"	if (!(max(b.x, b.y) < 0.5))\n"
	"	{\n"
	"		discard;\n" // 10
	"	}\n"
	"	gl_FragColor = _g;\n"
	"}\n"
;

#define macroanimXValues(x) x
#define macroanimXValuesInv(x) x
#define macroanimKeys(x) x
#define macroanimKeysInv(x) x
struct Buffers
{
	float* animXValues;
	float* animKeys;
};

struct State
{
	float time;
	float lastTime;
	struct
	{
		float scale;
		float3 offset;
	} deformer1;
	struct
	{
		float3 color;
		Texture2D texture;
		Texture2D textureSequence[4u];
		int textureIndex;
		float4x4 matrix;
	} node1;
	struct
	{
		Texture2D font;
		string text;
		TextSymbols symbols;
	} text;
};

struct Deformer_particleDeformer
{
};
struct Deformer_deformer
{
	float scale;
	float3 offset;
};

#define macro_text(scale, space, column, align, text, symbols, c, s)\
	TextData& data = instance._text;\
	text2symbols(scale, space, column, align, text, symbols, 16u, data);\
	c = data.center;\
	s = data.size;
struct Emitters_particleSystem
{
	int numNewParticles;
};
struct ParticleSystem_particleSystem
{
	float timeStep;
};
struct Particle_particleSystem
{
	Particle_particleSystem* next;
	float3 position;
	float size;
	int index;
};
struct Particles_particleSystem
{
	Particle_particleSystem* particles;
	Particle_particleSystem* alive;
	Particle_particleSystem* dead;
	int numParticles;
	int id;
};

struct Instancer_instancer
{
};

struct Material_shader1
{
	float3 color;
	Texture2D sampler;
};

struct Material_shader3
{
	Texture2D sampler;
};

struct Material_shader2
{
	Texture2D sampler;
};

struct ShaderGlobalm_shader1_0
{
	GLuint program;
	GLuint _e;
	GLuint _f;
	GLuint _h;
};
struct ShaderUniformm_shader1_0
{
	float4 _e[4];
	float4 _f;
	Texture2D _g;
};
struct ShaderTransferm_shader1_0
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader1_0
{
	ShaderUniformm_shader1_0 uniform;
	ShaderTransferm_shader1_0 transfer;
};

struct ShaderGlobalm_shader1_1
{
	GLuint program;
	GLuint _e;
	GLuint _f;
	GLuint _h;
	GLuint _i;
};
struct ShaderUniformm_shader1_1
{
	float4 _e[4];
	float4 _f;
	Texture2D _g;
};
struct ShaderTransferm_shader1_1
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader1_1
{
	ShaderUniformm_shader1_1 uniform;
	ShaderTransferm_shader1_1 transfer;
	bool sort;
	RenderJob* renderJobs;
};

struct ShaderGlobalm_shader3_0
{
	GLuint program;
	GLuint _e;
	GLuint _g;
};
struct ShaderUniformm_shader3_0
{
	float4 _e[4];
	Texture2D _f;
};
struct ShaderTransferm_shader3_0
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader3_0
{
	ShaderUniformm_shader3_0 uniform;
	ShaderTransferm_shader3_0 transfer;
};

struct ShaderGlobalm_shader3_1
{
	GLuint program;
	GLuint _e;
	GLuint _g;
	GLuint _h;
};
struct ShaderUniformm_shader3_1
{
	float4 _e[4];
	Texture2D _f;
};
struct ShaderTransferm_shader3_1
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader3_1
{
	ShaderUniformm_shader3_1 uniform;
	ShaderTransferm_shader3_1 transfer;
	bool sort;
	RenderJob* renderJobs;
};

struct ShaderGlobalm_shader2_0
{
	GLuint program;
	GLuint _e;
	GLuint _g;
};
struct ShaderUniformm_shader2_0
{
	float4 _e[4];
	Texture2D _f;
};
struct ShaderTransferm_shader2_0
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader2_0
{
	ShaderUniformm_shader2_0 uniform;
	ShaderTransferm_shader2_0 transfer;
	RenderJob* renderJobs;
};

struct ShaderGlobalm_shader2_1
{
	GLuint program;
	GLuint _e;
	GLuint _f;
	GLuint _g;
};
struct ShaderUniformm_shader2_1
{
	float4 _e[4];
};
struct ShaderTransferm_shader2_1
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader2_1
{
	ShaderUniformm_shader2_1 uniform;
	ShaderTransferm_shader2_1 transfer;
	bool sort;
	RenderJob* renderJobs;
};

struct ShaderGlobalp_shader2_0
{
	GLuint program;
	GLuint _e;
	GLuint _g;
};
struct ShaderUniformp_shader2_0
{
	float4 _e[5];
	Texture2D _f;
};
struct ShaderTransferp_shader2_0
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderp_shader2_0
{
	ShaderUniformp_shader2_0 uniform;
	ShaderTransferp_shader2_0 transfer;
	RenderJob* renderJobs;
};

struct ShaderGlobalp_shader2_1
{
	GLuint program;
	GLuint _e;
	GLuint _f;
	GLuint _g;
};
struct ShaderUniformp_shader2_1
{
	float4 _e[5];
};
struct ShaderTransferp_shader2_1
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderp_shader2_1
{
	ShaderUniformp_shader2_1 uniform;
	ShaderTransferp_shader2_1 transfer;
	bool sort;
	RenderJob* renderJobs;
};

struct Uniforms
{
	Deformer_particleDeformer _particleDeformer;
	Deformer_deformer _deformer;
	struct
	{
		Emitters_particleSystem _particleSystem;
	} e;
	ParticleSystem_particleSystem _particleSystem;
	Instancer_instancer _instancer;
	Material_shader1 _shader1;
	Material_shader3 _shader3;
	Material_shader2 _shader2;
};

struct Global
{
	Buffers buffers;
	GLuint shaderInputBuffers[1];
	ubyte* deformerInputBuffers[1];
	GLuint indexBuffers[1];
	GLuint quads;
	ShaderGlobalm_shader1_0 m_shader1_0;
	ShaderGlobalm_shader1_1 m_shader1_1;
	ShaderGlobalm_shader3_0 m_shader3_0;
	ShaderGlobalm_shader3_1 m_shader3_1;
	ShaderGlobalm_shader2_0 m_shader2_0;
	ShaderGlobalm_shader2_1 m_shader2_1;
	ShaderGlobalp_shader2_0 p_shader2_0;
	ShaderGlobalp_shader2_1 p_shader2_1;
	int sequence;
};

struct Instance
{
	Global* global;
	int ids[5];
	State state;
	Uniforms uniforms;
	TextData _text;
	Particles_particleSystem _particleSystem;
	Shaderm_shader1_0 m_shader1_0;
	Shaderm_shader1_1 m_shader1_1;
	Shaderm_shader3_0 m_shader3_0;
	Shaderm_shader3_1 m_shader3_1;
	Shaderm_shader2_0 m_shader2_0;
	Shaderm_shader2_1 m_shader2_1;
	Shaderp_shader2_0 p_shader2_0;
	Shaderp_shader2_1 p_shader2_1;
	Transform transforms[4];
	BoundingBox boundingBoxes[1];
	int sceneSequence;
	int deformerSequence;
	int renderSequence;
	int seed;
	GLuint dynamicBuffers[3];
};

void initGlobal(void* pGlobal, ubyte* data)
{
	Global& global = *(Global*)pGlobal;

	global.buffers.animXValues = (float*)(data + 0);
	global.buffers.animKeys = (float*)(data + 16);

	glGenBuffers(1, global.shaderInputBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 84u, data + 56, GL_STATIC_DRAW);

	global.deformerInputBuffers[0] = data + 140;

	glGenBuffers(1, global.indexBuffers);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3u, data + 158, GL_STATIC_DRAW);

	{
		glGenBuffers(1, &global.quads);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.quads);
		ubyte* quads = (ubyte*)malloc(96);
		for (int v = 0, i = 0; v < 64; v += 4, i += 6)
		{
			quads[i + 0] = v + 0;
			quads[i + 1] = v + 1;
			quads[i + 2] = v + 3;
			quads[i + 3] = v + 1;
			quads[i + 4] = v + 2;
			quads[i + 5] = v + 3;
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 96, quads, GL_STATIC_DRAW);
		free(quads);
	}

	{
		ShaderGlobalm_shader1_0& shader = global.m_shader1_0;
		GLuint vertexShader = createVertexShader(vsm_shader1_0, "mesh, shader 'shader1', layer 'color'");
		GLuint pixelShader = createPixelShader(psm_shader1_0, "mesh, shader 'shader1', layer 'color'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		shader._f = glGetUniformLocation(program, "_f");
		glUniform1i(glGetUniformLocation(program, "_g"), 0);
		shader._h = glGetUniformLocation(program, "_h");
	}
	{
		ShaderGlobalm_shader1_1& shader = global.m_shader1_1;
		GLuint vertexShader = createVertexShader(vsm_shader1_1, "mesh, shader 'shader1', layer 'pick'");
		GLuint pixelShader = createPixelShader(psm_shader1_1, "mesh, shader 'shader1', layer 'pick'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		shader._f = glGetUniformLocation(program, "_f");
		glUniform1i(glGetUniformLocation(program, "_g"), 0);
		shader._h = glGetUniformLocation(program, "_h");
		shader._i = glGetUniformLocation(program, "_i");
	}
	{
		ShaderGlobalm_shader3_0& shader = global.m_shader3_0;
		GLuint vertexShader = createVertexShader(vsm_shader3_0, "mesh, shader 'shader3', layer 'color'");
		GLuint pixelShader = createPixelShader(psm_shader3_0, "mesh, shader 'shader3', layer 'color'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		glUniform1i(glGetUniformLocation(program, "_f"), 0);
		shader._g = glGetUniformLocation(program, "_g");
	}
	{
		ShaderGlobalm_shader3_1& shader = global.m_shader3_1;
		GLuint vertexShader = createVertexShader(vsm_shader3_1, "mesh, shader 'shader3', layer 'pick'");
		GLuint pixelShader = createPixelShader(psm_shader3_1, "mesh, shader 'shader3', layer 'pick'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		glUniform1i(glGetUniformLocation(program, "_f"), 0);
		shader._g = glGetUniformLocation(program, "_g");
		shader._h = glGetUniformLocation(program, "_h");
	}
	{
		ShaderGlobalm_shader2_0& shader = global.m_shader2_0;
		GLuint vertexShader = createVertexShader(vsm_shader2_0, "mesh, shader 'shader2', layer 'color'");
		GLuint pixelShader = createPixelShader(psm_shader2_0, "mesh, shader 'shader2', layer 'color'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		glUniform1i(glGetUniformLocation(program, "_f"), 0);
		shader._g = glGetUniformLocation(program, "_g");
	}
	{
		ShaderGlobalm_shader2_1& shader = global.m_shader2_1;
		GLuint vertexShader = createVertexShader(vsm_shader2_1, "mesh, shader 'shader2', layer 'pick'");
		GLuint pixelShader = createPixelShader(psm_shader2_1, "mesh, shader 'shader2', layer 'pick'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		shader._e = glGetUniformLocation(program, "_e");
		shader._f = glGetUniformLocation(program, "_f");
		shader._g = glGetUniformLocation(program, "_g");
	}
	{
		ShaderGlobalp_shader2_0& shader = global.p_shader2_0;
		GLuint vertexShader = createVertexShader(vsp_shader2_0, "sprites, shader 'shader2', layer 'color'");
		GLuint pixelShader = createPixelShader(psp_shader2_0, "sprites, shader 'shader2', layer 'color'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		glUniform1i(glGetUniformLocation(program, "_f"), 0);
		shader._g = glGetUniformLocation(program, "_g");
	}
	{
		ShaderGlobalp_shader2_1& shader = global.p_shader2_1;
		GLuint vertexShader = createVertexShader(vsp_shader2_1, "sprites, shader 'shader2', layer 'pick'");
		GLuint pixelShader = createPixelShader(psp_shader2_1, "sprites, shader 'shader2', layer 'pick'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glBindAttribLocation(program, 1, "_1");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		shader._e = glGetUniformLocation(program, "_e");
		shader._f = glGetUniformLocation(program, "_f");
		shader._g = glGetUniformLocation(program, "_g");
	}
}

void doneGlobal(void* pGlobal)
{
	Global& global = *(Global*)pGlobal;

	glDeleteBuffers(1, global.shaderInputBuffers);
	glDeleteBuffers(1, global.indexBuffers);
	glDeleteProgram(global.m_shader1_0.program);
	glDeleteProgram(global.m_shader1_1.program);
	glDeleteProgram(global.m_shader3_0.program);
	glDeleteProgram(global.m_shader3_1.program);
	glDeleteProgram(global.m_shader2_0.program);
	glDeleteProgram(global.m_shader2_1.program);
	glDeleteProgram(global.p_shader2_0.program);
	glDeleteProgram(global.p_shader2_1.program);
}

void initInstance(const void* pGlobal, void* pInstance)
{
	Global& global = *(Global*)pGlobal;
	Instance& instance = *(Instance*)pInstance;
	instance.global = &global;

	glGenBuffers(3, instance.dynamicBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 36u, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 1280u, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, 120u, NULL, GL_DYNAMIC_DRAW);

	{
		TextData& text = instance._text;
		text.vertices = (TextVertex*)malloc(64u * sizeof(TextVertex));
	}
	{
		float time = 0.0f;
		Particles_particleSystem& pp = instance._particleSystem;
		Particle_particleSystem* p = (Particle_particleSystem*)malloc(5 * sizeof(Particle_particleSystem));
		pp.particles = p;
		Particle_particleSystem* end = p + 5;
		Particle_particleSystem* dead = NULL;
		for (; p != end; ++p)
		{
			p->next = dead;
			dead = p;
		}
		pp.dead = dead;
	}
	{
		State& state = instance.state;
		state.node1.color = make_float3(1, 1, 1);
		state.node1.matrix = float4x4Identity();
	}
}

void doneInstance(void* pInstance)
{
	Instance& instance = *(Instance*)pInstance;

	glDeleteBuffers(3, instance.dynamicBuffers);
	free(instance._text.vertices);
	free(instance._particleSystem.particles);
}

void addClip(void* pInstance, int index, float* tracks, float time, float weight)
{
}

void update(void* pInstance)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	++instance.sceneSequence;

	State& state = instance.state;
	const Buffers& buffers = global.buffers;
	Uniforms& uniforms = instance.uniforms;
	Transform* transforms = instance.transforms;
	BoundingBox* boundingBoxes = instance.boundingBoxes;
	int& seed = instance.seed;

	state.deformer1.scale = evalBezierTrack(buffers.animXValues, buffers.animKeys, 4, mod(state.time, 3.0f));
	float3 center, size;
	macro_text(vector2(0.1f, 0.1f), vector3(0.4f, 0.0f, 1.0f), splat4(8.0f), vector2(0, 0), state.text.text, state.text.symbols, center, size);
	float timeStep = state.time - state.lastTime;
	uniforms._particleSystem.timeStep = timeStep;
	uniforms.e._particleSystem.numNewParticles = 1;
	transforms[0].matrix = state.node1.matrix;
	transforms[0].visible = true;
	transforms[1].matrix = matrix4x4Translate(vector3(0.7f, 0.0f, 0.0f)) * state.node1.matrix;
	transforms[1].visible = true;
	transforms[2].matrix = matrix4x4Translate(vector3(-7.0f, 0.0f, 0.0f)) * state.node1.matrix;
	transforms[2].visible = true;
	transforms[3].matrix = matrix4x4Translate(vector3(-0.95f, 0.0f, 0.0f)) * state.node1.matrix;
	transforms[3].visible = true;
	boundingBoxes[0].center = 0.0f;
	boundingBoxes[0].size = 1.0f;
	state.lastTime = state.time;
	uniforms._deformer.scale = state.deformer1.scale;
	uniforms._deformer.offset = state.deformer1.offset;
	uniforms._shader1.color = state.node1.color;
	uniforms._shader1.sampler = state.node1.texture;
	uniforms._shader3.sampler = state.text.font;
	uniforms._shader2.sampler = (state.node1.textureIndex >= 0 && state.node1.textureIndex < 4) ? state.node1.textureSequence[state.node1.textureIndex] : 0;
	{
		Particles_particleSystem& pp = instance._particleSystem;
		ParticleSystem_particleSystem& uniform = uniforms._particleSystem;
		Particle_particleSystem* it = pp.alive;
		Particle_particleSystem** last = &pp.alive;
		Particle_particleSystem* dead = pp.dead;
		int numParticles = pp.numParticles;
		while (it)
		{
			Particle_particleSystem& particle = *it;
			bool alive = true;
			{
				particle.position.y += uniform.timeStep * 0.05f;
			}
			if (alive)
			{
				*last = it;
				last = &it->next;
				it = it->next;
			}
			else
			{
				Particle_particleSystem* next = it->next;
				it->next = dead;
				dead = it;
				it = next;
				--numParticles;
			}
		}

		{
			Emitters_particleSystem& emitter = instance.uniforms.e._particleSystem;
			int index = emitter.numNewParticles;
			int id = pp.id;
			while (dead && index > 0)
			{
				--index;
				Particle_particleSystem& particle = *dead;
				{
				}
				{
					particle.position = vector3(-0.2f + 0.1f * float(id), 0.0f, 0.0f);
					particle.size = 0.1f;
					particle.index = 0;
				}
				++numParticles;
				++id;
				*last = dead;
				last = &dead->next;
				dead = dead->next;
			}
			pp.id = id;
		}
		*last = NULL;
		pp.dead = dead;
		pp.numParticles = numParticles;
	}
}

void getBoundingBox(void* pInstance, float4x2& boundingBox)
{
	Instance& instance = *(Instance*)pInstance;
	Transform* transforms = instance.transforms;
	BoundingBox* boundingBoxes = instance.boundingBoxes;
	float3 minP = {};
	float3 maxP = {};
	{
		BoundingBox& bb = boundingBoxes[0];
		float3 bbs = bb.size;
		{
			float4x4& m = transforms[0].matrix;
			float3 c = transformPosition(m, bb.center);
			float3 s = abs(m.x.xyz) * bbs.x + abs(m.y.xyz) * bbs.y + abs(m.z.xyz) * bbs.z;
			minP = c - s;
			maxP = c + s;
		}
		{
			float4x4& m = transforms[1].matrix;
			float3 c = transformPosition(m, bb.center);
			float3 s = abs(m.x.xyz) * bbs.x + abs(m.y.xyz) * bbs.y + abs(m.z.xyz) * bbs.z;
			minP = min(minP, c - s);
			maxP = max(maxP, c + s);
		}
		{
			float4x4& m = transforms[2].matrix;
			float3 c = transformPosition(m, bb.center);
			float3 s = abs(m.x.xyz) * bbs.x + abs(m.y.xyz) * bbs.y + abs(m.z.xyz) * bbs.z;
			minP = min(minP, c - s);
			maxP = max(maxP, c + s);
		}
		{
			float4x4& m = transforms[3].matrix;
			float3 c = transformPosition(m, bb.center);
			float3 s = abs(m.x.xyz) * bbs.x + abs(m.y.xyz) * bbs.y + abs(m.z.xyz) * bbs.z;
			minP = min(minP, c - s);
			maxP = max(maxP, c + s);
		}
	}
	boundingBox.x.xyz = (maxP + minP) * 0.5f;
	boundingBox.y.xyz = (maxP - minP) * 0.5f;
}

static void render_m_shader1_0_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader1_0& shader = global.m_shader1_0;
	glUseProgram(shader.program);
	const ShaderUniformm_shader1_0& uniform = instance.m_shader1_0.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glUniform4fv(shader._f, 1, (GLfloat*)&uniform._f);
	glBindTexture(GL_TEXTURE_2D, uniform._g);
	glDisable(GL_CULL_FACE);
	const ShaderTransferm_shader1_0& transfer = instance.m_shader1_0.transfer;
	float4x4& matrix = renderJob->matrix;
	float4 _h[4];
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	_h[0].xyz = (e * a.x + f * a.y + g * a.z + h * a.w).xyz;
	_h[1].xyz = (e * b.x + f * b.y + g * b.z + h * b.w).xyz;
	_h[2].xyz = (e * c.x + f * c.y + g * c.z + h * c.w).xyz;
	_h[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	glUniform4fv(shader._h, 4, (GLfloat*)_h);
	renderJob->draw(&instance, &shader);
	glBindTexture(GL_TEXTURE_2D, 0);
}
static void render_m_shader1_1_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader1_1& shader = global.m_shader1_1;
	const ShaderTransferm_shader1_1& transfer = instance.m_shader1_1.transfer;
	float4x4& matrix = renderJob->matrix;
	int objectId = renderJob->id;
	float4 _h[4];
	float4 _i;
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	int i = objectId;
	_h[0].xyz = (e * a.x + f * a.y + g * a.z + h * a.w).xyz;
	_h[1].xyz = (e * b.x + f * b.y + g * b.z + h * b.w).xyz;
	_h[2].xyz = (e * c.x + f * c.y + g * c.z + h * c.w).xyz;
	_h[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	_i = vector4(convert_float(i & 255) * 0.0039216f, convert_float(i >> 8 & 255) * 0.0039216f, convert_float(objectId >> 16) * 0.0039216f, 1.0f);
	glUniform4fv(shader._h, 4, (GLfloat*)_h);
	glUniform4fv(shader._i, 1, (GLfloat*)&_i);
	renderJob->draw(&instance, &shader);
}
static void render_M_shader1_1_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader1_1& shader = global.m_shader1_1;
	glUseProgram(shader.program);
	const ShaderUniformm_shader1_1& uniform = instance.m_shader1_1.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glUniform4fv(shader._f, 1, (GLfloat*)&uniform._f);
	glBindTexture(GL_TEXTURE_2D, uniform._g);
	glDisable(GL_CULL_FACE);
	render_m_shader1_1_1(renderJob);
	glBindTexture(GL_TEXTURE_2D, 0);
}
static void render_m_shader3_0_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader3_0& shader = global.m_shader3_0;
	glUseProgram(shader.program);
	const ShaderUniformm_shader3_0& uniform = instance.m_shader3_0.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glBindTexture(GL_TEXTURE_2D, uniform._f);
	glEnable(GL_CULL_FACE);
	const ShaderTransferm_shader3_0& transfer = instance.m_shader3_0.transfer;
	float4x4& matrix = renderJob->matrix;
	bool flip;
	float4 _g[4];
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	float4 i = e * a.x + f * a.y + g * a.z + h * a.w;
	float4 j = e * b.x + f * b.y + g * b.z + h * b.w;
	float4 k = e * c.x + f * c.y + g * c.z + h * c.w;
	float3 l = i.xyz * (j.yzx * k.zxy - j.zxy * k.yzx);
	flip = l.x + l.y + l.z < 0.0f;
	_g[0].xyz = i.xyz;
	_g[1].xyz = j.xyz;
	_g[2].xyz = k.xyz;
	_g[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	glUniform4fv(shader._g, 4, (GLfloat*)_g);
	glCullFace(flip ? GL_FRONT : GL_BACK);
	renderJob->draw(&instance, &shader);
	glBindTexture(GL_TEXTURE_2D, 0);
}
static void render_m_shader3_1_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader3_1& shader = global.m_shader3_1;
	const ShaderTransferm_shader3_1& transfer = instance.m_shader3_1.transfer;
	float4x4& matrix = renderJob->matrix;
	bool flip;
	int objectId = renderJob->id;
	float4 _g[4];
	float4 _h;
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	float4 i = e * a.x + f * a.y + g * a.z + h * a.w;
	float4 j = e * b.x + f * b.y + g * b.z + h * b.w;
	float4 k = e * c.x + f * c.y + g * c.z + h * c.w;
	float3 l = i.xyz * (j.yzx * k.zxy - j.zxy * k.yzx);
	flip = l.x + l.y + l.z < 0.0f;
	int m = objectId;
	_g[0].xyz = i.xyz;
	_g[1].xyz = j.xyz;
	_g[2].xyz = k.xyz;
	_g[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	_h = vector4(convert_float(m & 255) * 0.0039216f, convert_float(m >> 8 & 255) * 0.0039216f, convert_float(objectId >> 16) * 0.0039216f, 1.0f);
	glUniform4fv(shader._g, 4, (GLfloat*)_g);
	glUniform4fv(shader._h, 1, (GLfloat*)&_h);
	glCullFace(flip ? GL_FRONT : GL_BACK);
	renderJob->draw(&instance, &shader);
}
static void render_M_shader3_1_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader3_1& shader = global.m_shader3_1;
	glUseProgram(shader.program);
	const ShaderUniformm_shader3_1& uniform = instance.m_shader3_1.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glBindTexture(GL_TEXTURE_2D, uniform._f);
	glEnable(GL_CULL_FACE);
	render_m_shader3_1_0(renderJob);
	glBindTexture(GL_TEXTURE_2D, 0);
}
static void render_m_shader2_0_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader2_0& shader = global.m_shader2_0;
	const ShaderTransferm_shader2_0& transfer = instance.m_shader2_0.transfer;
	float4x4& matrix = renderJob->matrix;
	float4 _g[4];
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	_g[0].xyz = (e * a.x + f * a.y + g * a.z + h * a.w).xyz;
	_g[1].xyz = (e * b.x + f * b.y + g * b.z + h * b.w).xyz;
	_g[2].xyz = (e * c.x + f * c.y + g * c.z + h * c.w).xyz;
	_g[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	glUniform4fv(shader._g, 4, (GLfloat*)_g);
	renderJob->draw(&instance, &shader);
}
static void render_m_shader2_1_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader2_1& shader = global.m_shader2_1;
	const ShaderTransferm_shader2_1& transfer = instance.m_shader2_1.transfer;
	float4x4& matrix = renderJob->matrix;
	int objectId = renderJob->id;
	float4 _f[4];
	float4 _g;
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	int i = objectId;
	_f[0].xyz = (e * a.x + f * a.y + g * a.z + h * a.w).xyz;
	_f[1].xyz = (e * b.x + f * b.y + g * b.z + h * b.w).xyz;
	_f[2].xyz = (e * c.x + f * c.y + g * c.z + h * c.w).xyz;
	_f[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	_g = vector4(convert_float(i & 255) * 0.0039216f, convert_float(i >> 8 & 255) * 0.0039216f, convert_float(objectId >> 16) * 0.0039216f, 1.0f);
	glUniform4fv(shader._f, 4, (GLfloat*)_f);
	glUniform4fv(shader._g, 1, (GLfloat*)&_g);
	renderJob->draw(&instance, &shader);
}
static void render_M_shader2_1_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader2_1& shader = global.m_shader2_1;
	glUseProgram(shader.program);
	const ShaderUniformm_shader2_1& uniform = instance.m_shader2_1.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glDisable(GL_CULL_FACE);
	glDisableVertexAttribArray(1);
	render_m_shader2_1_1(renderJob);
	glEnableVertexAttribArray(1);
}
static void render_p_shader2_0_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalp_shader2_0& shader = global.p_shader2_0;
	const ShaderTransferp_shader2_0& transfer = instance.p_shader2_0.transfer;
	float4x4& matrix = renderJob->matrix;
	float4 _g[4];
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	_g[0].xyz = (e * a.x + f * a.y + g * a.z + h * a.w).xyz;
	_g[1].xyz = (e * b.x + f * b.y + g * b.z + h * b.w).xyz;
	_g[2].xyz = (e * c.x + f * c.y + g * c.z + h * c.w).xyz;
	_g[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	glUniform4fv(shader._g, 4, (GLfloat*)_g);
	renderJob->draw(&instance, &shader);
}
static void render_p_shader2_1_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalp_shader2_1& shader = global.p_shader2_1;
	const ShaderTransferp_shader2_1& transfer = instance.p_shader2_1.transfer;
	float4x4& matrix = renderJob->matrix;
	int objectId = renderJob->id;
	float4 _f[4];
	float4 _g;
	float4 a = matrix.x;
	float4 b = matrix.y;
	float4 c = matrix.z;
	float4 d = matrix.w;
	float4 e = transfer._a;
	float4 f = transfer._b;
	float4 g = transfer._c;
	float4 h = transfer._d;
	int i = objectId;
	_f[0].xyz = (e * a.x + f * a.y + g * a.z + h * a.w).xyz;
	_f[1].xyz = (e * b.x + f * b.y + g * b.z + h * b.w).xyz;
	_f[2].xyz = (e * c.x + f * c.y + g * c.z + h * c.w).xyz;
	_f[3].xyz = (e * d.x + f * d.y + g * d.z + h * d.w).xyz;
	_g = vector4(convert_float(i & 255) * 0.0039216f, convert_float(i >> 8 & 255) * 0.0039216f, convert_float(objectId >> 16) * 0.0039216f, 1.0f);
	glUniform4fv(shader._f, 4, (GLfloat*)_f);
	glUniform4fv(shader._g, 1, (GLfloat*)&_g);
	renderJob->draw(&instance, &shader);
}
static void render_P_shader2_1_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalp_shader2_1& shader = global.p_shader2_1;
	glUseProgram(shader.program);
	const ShaderUniformp_shader2_1& uniform = instance.p_shader2_1.uniform;
	glUniform4fv(shader._e, 5, (GLfloat*)uniform._e);
	render_p_shader2_1_0(renderJob);
}
static void draw_a(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 28, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 28, (GLvoid*)20u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_b(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 28, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 28, (GLvoid*)20u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_c(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 24, (GLvoid*)0u);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 24, (GLvoid*)12u);
	glDrawArrays(GL_POINTS, 0, instance._particleSystem.numParticles);
}
static void draw_d(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 24, (GLvoid*)0u);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 24, (GLvoid*)12u);
	glDrawArrays(GL_POINTS, 0, instance._particleSystem.numParticles);
}
static void draw_e(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 20, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 20, (GLvoid*)12u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.quads);
	glDrawElements(GL_TRIANGLES, instance._text.numSymbols * 6, GL_UNSIGNED_BYTE, (GLvoid*)0);
}
static void draw_f(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 20, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 20, (GLvoid*)12u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.quads);
	glDrawElements(GL_TRIANGLES, instance._text.numSymbols * 6, GL_UNSIGNED_BYTE, (GLvoid*)0);
}
static void draw_g(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 28, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 28, (GLvoid*)20u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_h(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 28, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 28, (GLvoid*)20u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_i(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 12, (GLvoid*)0u);
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 28, (GLvoid*)12u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_j(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 12, (GLvoid*)0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}

void render(void* pInstance, const float4x4& viewMatrix, const float4x4& projectionMatrix, int layerIndex, RenderQueues& renderQueues)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	instance.renderSequence = ++global.sequence;
	float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;
	float viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);

	if (instance.sceneSequence != instance.deformerSequence)
	{
		instance.deformerSequence = instance.sceneSequence;

		{
			glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[0]);
			ubyte* outputData = (ubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// deformer 'deformer'
			{
				Deformer_deformer& deformer = instance.uniforms._deformer;
				float _a;
				float3 _b;
				_a = deformer.scale;
				_b = deformer.offset;
				ubyte* vertex0 = global.deformerInputBuffers[0] + 0u;
				ubyte* result = outputData + 0u;
				for (int i_ = 0; i_ < 3; ++i_)
				{
					float3 _0 = convert_float3(*(packed_short3*)(vertex0 + 0));
					*(packed_float3*)(result + 0) = _0 * 3.05185e-5f * _a + _b;

					vertex0 += 6;
					result += 12;
				}
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		{
			glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);
			ubyte* outputData = (ubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// copy
			{
				int numVertices = instance._text.numSymbols * 4;
				TextVertex* _g = instance._text.vertices;
				ubyte* result = outputData + 0u;
				for (int i_ = 0; i_ < numVertices; ++i_)
				{
					float3 _0 = _g->position;
					float2 _1 = _g->symbol;
					*(packed_float3*)(result + 0) = _0;
					*(float2*)(result + 12) = _1;

					++_g;
					result += 20;
				}
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		{
			glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[2]);
			ubyte* outputData = (ubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

			// deformer 'particleDeformer'
			{
				Deformer_particleDeformer& deformer = instance.uniforms._particleDeformer;
				int numParticles = instance._particleSystem.numParticles;
				Particle_particleSystem* _g = instance._particleSystem.alive;
				ubyte* result = outputData + 0u;
				for (int i_ = 0; i_ < numParticles; ++i_)
				{
					float3 _0 = _g->position;
					*(packed_float3*)(result + 0) = vector3(0.1f, 0.2f, 0.1f);
					*(packed_float3*)(result + 12) = _0;

					_g = _g->next;
					result += 24;
				}
			}
			glUnmapBuffer(GL_ARRAY_BUFFER);
		}
	}

	switch (layerIndex)
	{
	case 0:
		{
			// mesh, shader 'shader1', layer 'color'
			{
				Material_shader1& material = instance.uniforms._shader1;
				Shaderm_shader1_0& si = instance.m_shader1_0;
				ShaderUniformm_shader1_0& uniform = si.uniform;
				ShaderTransferm_shader1_0& transfer = si.transfer;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._f.xyz = material.color;
				uniform._g = material.sampler;
			}
			// mesh, shader 'shader3', layer 'color'
			{
				Material_shader3& material = instance.uniforms._shader3;
				Shaderm_shader3_0& si = instance.m_shader3_0;
				ShaderUniformm_shader3_0& uniform = si.uniform;
				ShaderTransferm_shader3_0& transfer = si.transfer;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._f = material.sampler;
			}
			// mesh, shader 'shader2', layer 'color'
			{
				Material_shader2& material = instance.uniforms._shader2;
				Shaderm_shader2_0& si = instance.m_shader2_0;
				ShaderUniformm_shader2_0& uniform = si.uniform;
				ShaderTransferm_shader2_0& transfer = si.transfer;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._f = material.sampler;
				si.renderJobs = NULL;
			}
			// sprites, shader 'shader2', layer 'color'
			{
				Material_shader2& material = instance.uniforms._shader2;
				Shaderp_shader2_0& si = instance.p_shader2_0;
				ShaderUniformp_shader2_0& uniform = si.uniform;
				ShaderTransferp_shader2_0& transfer = si.transfer;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._e[4].xy = vector2(viewport[2], viewport[3]) * 0.5f;
				uniform._f = material.sampler;
				si.renderJobs = NULL;
			}

			RenderJob* jobIt = renderQueues.begin;
			RenderJob* jobEnd = renderQueues.end;
			{
				Transform& transform = instance.transforms[0];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader1_0& shader = instance.m_shader1_0;
						RenderJob* renderJob;
						renderJob = jobIt++;
						renderJob->next = renderQueues.alphaSort;
						renderQueues.alphaSort = renderJob;
						float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
						renderJob->distance = position.z / position.w;
						renderJob->render = &render_m_shader1_0_1;
						renderJob->draw = &draw_a;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader1_0& shader = instance.m_shader1_0;
						RenderJob* renderJob;
						renderJob = jobIt++;
						renderJob->next = renderQueues.alphaSort;
						renderQueues.alphaSort = renderJob;
						float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
						renderJob->distance = position.z / position.w;
						renderJob->render = &render_m_shader1_0_1;
						renderJob->draw = &draw_g;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			{
				Transform& transform = instance.transforms[0];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader2_0& shader = instance.m_shader2_0;
						RenderJob* renderJob;
						renderJob = --jobEnd;
						renderJob->next = shader.renderJobs;
						shader.renderJobs = renderJob;
						renderJob->render = &render_m_shader2_0_1;
						renderJob->draw = &draw_i;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader2_0& shader = instance.m_shader2_0;
						RenderJob* renderJob;
						renderJob = --jobEnd;
						renderJob->next = shader.renderJobs;
						shader.renderJobs = renderJob;
						renderJob->render = &render_m_shader2_0_1;
						renderJob->draw = &draw_i;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			{
				Transform& transform = instance.transforms[1];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderp_shader2_0& shader = instance.p_shader2_0;
						RenderJob* renderJob;
						renderJob = --jobEnd;
						renderJob->next = shader.renderJobs;
						shader.renderJobs = renderJob;
						renderJob->render = &render_p_shader2_0_0;
						renderJob->draw = &draw_c;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			{
				Transform& transform = instance.transforms[2];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					Instancer_instancer& uniform = instance.uniforms._instancer;
					Particle_particleSystem* it = instance._particleSystem.alive;
					while (it)
					{
						Particle_particleSystem& particle = *it;
						int index = 0;
						float4x4 particleMatrix;
						{
							particleMatrix = matrix4x4TranslateScale(particle.position, splat3(particle.size));
						}
						{
							Transform& transform = instance.transforms[2];
							if (transform.visible)
							{
								float4x4 matrix = particleMatrix * transform.matrix;
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader1_0& shader = instance.m_shader1_0;
									RenderJob* renderJob;
									renderJob = jobIt++;
									renderJob->next = renderQueues.alphaSort;
									renderQueues.alphaSort = renderJob;
									float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
									renderJob->distance = position.z / position.w;
									renderJob->render = &render_m_shader1_0_1;
									renderJob->draw = &draw_a;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader1_0& shader = instance.m_shader1_0;
									RenderJob* renderJob;
									renderJob = jobIt++;
									renderJob->next = renderQueues.alphaSort;
									renderQueues.alphaSort = renderJob;
									float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
									renderJob->distance = position.z / position.w;
									renderJob->render = &render_m_shader1_0_1;
									renderJob->draw = &draw_g;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
							}
						}
						{
							Transform& transform = instance.transforms[2];
							if (transform.visible)
							{
								float4x4 matrix = particleMatrix * transform.matrix;
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader2_0& shader = instance.m_shader2_0;
									RenderJob* renderJob;
									renderJob = --jobEnd;
									renderJob->next = shader.renderJobs;
									shader.renderJobs = renderJob;
									renderJob->render = &render_m_shader2_0_1;
									renderJob->draw = &draw_i;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader2_0& shader = instance.m_shader2_0;
									RenderJob* renderJob;
									renderJob = --jobEnd;
									renderJob->next = shader.renderJobs;
									shader.renderJobs = renderJob;
									renderJob->render = &render_m_shader2_0_1;
									renderJob->draw = &draw_i;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
							}
						}
						it = it->next;
					}
				}
			}
			{
				Transform& transform = instance.transforms[3];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader3_0& shader = instance.m_shader3_0;
						RenderJob* renderJob;
						renderJob = jobIt++;
						renderJob->next = renderQueues.alphaSort;
						renderQueues.alphaSort = renderJob;
						float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
						renderJob->distance = position.z / position.w;
						renderJob->render = &render_m_shader3_0_0;
						renderJob->draw = &draw_e;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			renderQueues.begin = jobIt;
			{
				ShaderGlobalm_shader2_0& shader = global.m_shader2_0;
				glUseProgram(shader.program);
				const ShaderUniformm_shader2_0& uniform = instance.m_shader2_0.uniform;
				glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
				glBindTexture(GL_TEXTURE_2D, uniform._f);
				glDisable(GL_CULL_FACE);
				RenderJob* current = instance.m_shader2_0.renderJobs;
				while (current)
				{
					current->render(current);
					current = current->next;
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			{
				ShaderGlobalp_shader2_0& shader = global.p_shader2_0;
				glUseProgram(shader.program);
				const ShaderUniformp_shader2_0& uniform = instance.p_shader2_0.uniform;
				glUniform4fv(shader._e, 5, (GLfloat*)uniform._e);
				glBindTexture(GL_TEXTURE_2D, uniform._f);
				RenderJob* current = instance.p_shader2_0.renderJobs;
				while (current)
				{
					current->render(current);
					current = current->next;
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}
		break;
	case -1:
		{
			// mesh, shader 'shader1', layer 'pick'
			{
				Material_shader1& material = instance.uniforms._shader1;
				Shaderm_shader1_1& si = instance.m_shader1_1;
				ShaderUniformm_shader1_1& uniform = si.uniform;
				ShaderTransferm_shader1_1& transfer = si.transfer;
				bool sort = false;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._f.xyz = material.color;
				uniform._g = material.sampler;
				si.sort = sort;
				si.renderJobs = NULL;
			}
			// mesh, shader 'shader3', layer 'pick'
			{
				Material_shader3& material = instance.uniforms._shader3;
				Shaderm_shader3_1& si = instance.m_shader3_1;
				ShaderUniformm_shader3_1& uniform = si.uniform;
				ShaderTransferm_shader3_1& transfer = si.transfer;
				bool sort = false;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._f = material.sampler;
				si.sort = sort;
				si.renderJobs = NULL;
			}
			// mesh, shader 'shader2', layer 'pick'
			{
				Material_shader2& material = instance.uniforms._shader2;
				Shaderm_shader2_1& si = instance.m_shader2_1;
				ShaderUniformm_shader2_1& uniform = si.uniform;
				ShaderTransferm_shader2_1& transfer = si.transfer;
				bool sort = false;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				si.sort = sort;
				si.renderJobs = NULL;
			}
			// sprites, shader 'shader2', layer 'pick'
			{
				Material_shader2& material = instance.uniforms._shader2;
				Shaderp_shader2_1& si = instance.p_shader2_1;
				ShaderUniformp_shader2_1& uniform = si.uniform;
				ShaderTransferp_shader2_1& transfer = si.transfer;
				bool sort = false;
				transfer._a = viewMatrix.x;
				transfer._b = viewMatrix.y;
				transfer._c = viewMatrix.z;
				transfer._d = viewMatrix.w;
				uniform._e[0] = projectionMatrix.x;
				uniform._e[1] = projectionMatrix.y;
				uniform._e[2] = projectionMatrix.z;
				uniform._e[3] = projectionMatrix.w;
				uniform._e[4].xy = vector2(viewport[2], viewport[3]) * 0.5f;
				si.sort = sort;
				si.renderJobs = NULL;
			}

			RenderJob* jobIt = renderQueues.begin;
			RenderJob* jobEnd = renderQueues.end;
			{
				Transform& transform = instance.transforms[0];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader1_1& shader = instance.m_shader1_1;
						RenderJob* renderJob;
						if (!shader.sort)
						{
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_shader1_1_1;
							renderJob->distance = -2e30f;
						}
						else
						{
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_M_shader1_1_1;
						}
						renderJob->id = instance.ids[0];
						renderJob->draw = &draw_b;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader1_1& shader = instance.m_shader1_1;
						RenderJob* renderJob;
						if (!shader.sort)
						{
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_shader1_1_1;
							renderJob->distance = -2e30f;
						}
						else
						{
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_M_shader1_1_1;
						}
						renderJob->id = instance.ids[0];
						renderJob->draw = &draw_h;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			{
				Transform& transform = instance.transforms[0];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader2_1& shader = instance.m_shader2_1;
						RenderJob* renderJob;
						if (!shader.sort)
						{
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_shader2_1_1;
							renderJob->distance = -2e30f;
						}
						else
						{
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_M_shader2_1_1;
						}
						renderJob->id = instance.ids[1];
						renderJob->draw = &draw_j;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader2_1& shader = instance.m_shader2_1;
						RenderJob* renderJob;
						if (!shader.sort)
						{
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_shader2_1_1;
							renderJob->distance = -2e30f;
						}
						else
						{
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_M_shader2_1_1;
						}
						renderJob->id = instance.ids[1];
						renderJob->draw = &draw_j;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			{
				Transform& transform = instance.transforms[1];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderp_shader2_1& shader = instance.p_shader2_1;
						RenderJob* renderJob;
						if (!shader.sort)
						{
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_p_shader2_1_0;
							renderJob->distance = -2e30f;
						}
						else
						{
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_P_shader2_1_0;
						}
						renderJob->id = instance.ids[2];
						renderJob->draw = &draw_d;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			{
				Transform& transform = instance.transforms[2];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					Instancer_instancer& uniform = instance.uniforms._instancer;
					Particle_particleSystem* it = instance._particleSystem.alive;
					while (it)
					{
						Particle_particleSystem& particle = *it;
						int index = 0;
						float4x4 particleMatrix;
						{
							particleMatrix = matrix4x4TranslateScale(particle.position, splat3(particle.size));
						}
						{
							Transform& transform = instance.transforms[2];
							if (transform.visible)
							{
								float4x4 matrix = particleMatrix * transform.matrix;
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader1_1& shader = instance.m_shader1_1;
									RenderJob* renderJob;
									if (!shader.sort)
									{
										renderJob = --jobEnd;
										renderJob->next = shader.renderJobs;
										shader.renderJobs = renderJob;
										renderJob->render = &render_m_shader1_1_1;
										renderJob->distance = -2e30f;
									}
									else
									{
										renderJob = jobIt++;
										renderJob->next = renderQueues.alphaSort;
										renderQueues.alphaSort = renderJob;
										float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
										renderJob->distance = position.z / position.w;
										renderJob->render = &render_M_shader1_1_1;
									}
									renderJob->id = instance.ids[3];
									renderJob->draw = &draw_b;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader1_1& shader = instance.m_shader1_1;
									RenderJob* renderJob;
									if (!shader.sort)
									{
										renderJob = --jobEnd;
										renderJob->next = shader.renderJobs;
										shader.renderJobs = renderJob;
										renderJob->render = &render_m_shader1_1_1;
										renderJob->distance = -2e30f;
									}
									else
									{
										renderJob = jobIt++;
										renderJob->next = renderQueues.alphaSort;
										renderQueues.alphaSort = renderJob;
										float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
										renderJob->distance = position.z / position.w;
										renderJob->render = &render_M_shader1_1_1;
									}
									renderJob->id = instance.ids[3];
									renderJob->draw = &draw_h;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
							}
						}
						{
							Transform& transform = instance.transforms[2];
							if (transform.visible)
							{
								float4x4 matrix = particleMatrix * transform.matrix;
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader2_1& shader = instance.m_shader2_1;
									RenderJob* renderJob;
									if (!shader.sort)
									{
										renderJob = --jobEnd;
										renderJob->next = shader.renderJobs;
										shader.renderJobs = renderJob;
										renderJob->render = &render_m_shader2_1_1;
										renderJob->distance = -2e30f;
									}
									else
									{
										renderJob = jobIt++;
										renderJob->next = renderQueues.alphaSort;
										renderQueues.alphaSort = renderJob;
										float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
										renderJob->distance = position.z / position.w;
										renderJob->render = &render_M_shader2_1_1;
									}
									renderJob->id = instance.ids[3];
									renderJob->draw = &draw_j;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
								if (jobIt != jobEnd)
								{
									BoundingBox& bb = instance.boundingBoxes[0u];
									Shaderm_shader2_1& shader = instance.m_shader2_1;
									RenderJob* renderJob;
									if (!shader.sort)
									{
										renderJob = --jobEnd;
										renderJob->next = shader.renderJobs;
										shader.renderJobs = renderJob;
										renderJob->render = &render_m_shader2_1_1;
										renderJob->distance = -2e30f;
									}
									else
									{
										renderJob = jobIt++;
										renderJob->next = renderQueues.alphaSort;
										renderQueues.alphaSort = renderJob;
										float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
										renderJob->distance = position.z / position.w;
										renderJob->render = &render_M_shader2_1_1;
									}
									renderJob->id = instance.ids[3];
									renderJob->draw = &draw_j;
									renderJob->instance = &instance;
									renderJob->matrix = matrix;
								}
							}
						}
						it = it->next;
					}
				}
			}
			{
				Transform& transform = instance.transforms[3];
				if (transform.visible)
				{
					float4x4& matrix = transform.matrix;
					if (jobIt != jobEnd)
					{
						BoundingBox& bb = instance.boundingBoxes[0u];
						Shaderm_shader3_1& shader = instance.m_shader3_1;
						RenderJob* renderJob;
						if (!shader.sort)
						{
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_shader3_1_0;
							renderJob->distance = -2e30f;
						}
						else
						{
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_M_shader3_1_0;
						}
						renderJob->id = instance.ids[4];
						renderJob->draw = &draw_f;
						renderJob->instance = &instance;
						renderJob->matrix = matrix;
					}
				}
			}
			renderQueues.begin = jobIt;
			{
				ShaderGlobalm_shader1_1& shader = global.m_shader1_1;
				glUseProgram(shader.program);
				const ShaderUniformm_shader1_1& uniform = instance.m_shader1_1.uniform;
				glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
				glUniform4fv(shader._f, 1, (GLfloat*)&uniform._f);
				glBindTexture(GL_TEXTURE_2D, uniform._g);
				glDisable(GL_CULL_FACE);
				RenderJob* current = instance.m_shader1_1.renderJobs;
				while (current)
				{
					current->render(current);
					current = current->next;
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			{
				ShaderGlobalm_shader3_1& shader = global.m_shader3_1;
				glUseProgram(shader.program);
				const ShaderUniformm_shader3_1& uniform = instance.m_shader3_1.uniform;
				glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
				glBindTexture(GL_TEXTURE_2D, uniform._f);
				glEnable(GL_CULL_FACE);
				RenderJob* current = instance.m_shader3_1.renderJobs;
				while (current)
				{
					current->render(current);
					current = current->next;
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			{
				ShaderGlobalm_shader2_1& shader = global.m_shader2_1;
				glUseProgram(shader.program);
				const ShaderUniformm_shader2_1& uniform = instance.m_shader2_1.uniform;
				glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
				glDisable(GL_CULL_FACE);
				glDisableVertexAttribArray(1);
				RenderJob* current = instance.m_shader2_1.renderJobs;
				while (current)
				{
					current->render(current);
					current = current->next;
				}
			}
			{
				ShaderGlobalp_shader2_1& shader = global.p_shader2_1;
				glUseProgram(shader.program);
				const ShaderUniformp_shader2_1& uniform = instance.p_shader2_1.uniform;
				glUniform4fv(shader._e, 5, (GLfloat*)uniform._e);
				glEnableVertexAttribArray(1);
				RenderJob* current = instance.p_shader2_1.renderJobs;
				while (current)
				{
					current->render(current);
					current = current->next;
				}
			}
		}
		break;
	}
}

