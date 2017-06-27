// mesh, shader 'shader1'
static const char* vsm_shader1 =
	"precision mediump float;\n"
	"uniform vec4 _e[4];\n"
	"uniform vec4 _h[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _i;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0 * vec3(1.2207e-5, 1.2207e-5, 0.0) + vec3(-0.399993, -0.399993, 0.0);\n"
	"	vec3 b = _h[0].xyz * a.x + _h[1].xyz * a.y + _h[2].xyz * a.z + _h[3].xyz;\n" // 10
	"	gl_Position = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n"
	"	_i = _1;\n"
	"}\n"
;
static const char* psm_shader1 =
	"precision lowp float;\n"
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

// mesh, shader 'shader3'
static const char* vsm_shader3 =
	"precision mediump float;\n"
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
static const char* psm_shader3 =
	"precision lowp float;\n"
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

// sprites, deformer 'particleDeformer', shader 'shader2'
static const char* vsp_particleDeformer_shader2 =
	"precision mediump float;\n"
	"uniform vec4 _e[5];\n"
	"uniform vec4 _g[4];\n"
	"attribute vec3 _0;\n"
	"varying vec4 _h;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0;\n"
	"	vec3 b = _g[0].xyz * a.x + _g[1].xyz * a.y + _g[2].xyz * a.z + _g[3].xyz;\n"
	"	vec4 c = _e[0] * b.x + _e[1] * b.y + _e[2] * b.z + _e[3];\n" // 10
	"	gl_Position = c;\n"
	"	vec2 d = _e[4].xy / c.w * _e[4].zw;\n"
	"	float e = d.x * 0.995004;\n"
	"	float f = d.x * 0.0998334;\n"
	"	float g = d.y * -0.0998334;\n"
	"	float h = d.y * 0.995004;\n"
	"	vec2 i = vec2(e, f);\n"
	"	vec2 j = vec2(g, h);\n"
	"	vec2 k = abs(i + j);\n"
	"	vec2 l = abs(i - j);\n" // 20
	"	float m = max(max(k.x, l.x), max(k.y, l.y));\n"
	"	gl_PointSize = m;\n"
	"	float n = -g;\n"
	"	vec2 o = -vec2(f, -e);\n"
	"	vec2 p = i * vec2(h, n);\n"
	"	float q = 1.0 / (p.x + p.y);\n"
	"	_h.xy = vec2(q * h, q * o.x) * m;\n"
	"	_h.zw = -(vec2(q * n, q * o.y) * m);\n"
	"}\n"
;
static const char* psp_particleDeformer_shader2 =
	"precision lowp float;\n"
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

// mesh, deformer 'deformer', shader 'shader2'
static const char* vsm_deformer_shader2 =
	"precision mediump float;\n"
	"uniform vec4 _e[5];\n"
	"uniform vec4 _g[4];\n"
	"attribute vec3 _0;\n"
	"attribute vec2 _1;\n"
	"varying vec2 _h;\n"
	"void main()\n"
	"{\n"
	"	vec3 a = _0 * _e[0].x + _e[0].yzw;\n"
	"	vec3 b = _g[0].xyz * a.x + _g[1].xyz * a.y + _g[2].xyz * a.z + _g[3].xyz;\n" // 10
	"	gl_Position = _e[1] * b.x + _e[2] * b.y + _e[3] * b.z + _e[4];\n"
	"	_h = _1;\n"
	"}\n"
;
static const char* psm_deformer_shader2 =
	"precision lowp float;\n"
	"uniform sampler2D _f;\n"
	"varying vec2 _h;\n"
	"void main()\n"
	"{\n"
	"	vec4 a = texture2D(_f, (_h * 1.4 + -0.2) * vec2(1.0, -1.0) + vec2(0.0, 1.0));\n"
	"	gl_FragColor = vec4(a.xyz, 1.0);\n"
	"}\n"
;

#define macroanimXValues(x) (x * 4.5777e-5f + 0.0f)
#define macroanimXValuesInv(x) (x * 21845.0f + 0.0f)
#define macroanimKeys(x) (x * 1.5259e-5f + 0.0f)
#define macroanimKeysInv(x) (x * 65535.0f + 0.0f)
struct Buffers
{
	ushort* animXValues;
	ushort* animKeys;
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

struct ShaderGlobalm_shader1
{
	GLuint program;
	GLuint _e;
	GLuint _f;
	GLuint _h;
};
struct ShaderUniformm_shader1
{
	float4 _e[4];
	float4 _f;
	Texture2D _g;
};
struct ShaderTransferm_shader1
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader1
{
	ShaderUniformm_shader1 uniform;
	ShaderTransferm_shader1 transfer;
};

struct ShaderGlobalm_shader3
{
	GLuint program;
	GLuint _e;
	GLuint _g;
};
struct ShaderUniformm_shader3
{
	float4 _e[4];
	Texture2D _f;
};
struct ShaderTransferm_shader3
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_shader3
{
	ShaderUniformm_shader3 uniform;
	ShaderTransferm_shader3 transfer;
};

struct ShaderGlobalp_particleDeformer_shader2
{
	GLuint program;
	GLuint _e;
	GLuint _g;
};
struct ShaderUniformp_particleDeformer_shader2
{
	float4 _e[5];
	Texture2D _f;
};
struct ShaderTransferp_particleDeformer_shader2
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderp_particleDeformer_shader2
{
	ShaderUniformp_particleDeformer_shader2 uniform;
	ShaderTransferp_particleDeformer_shader2 transfer;
	RenderJob* renderJobs;
};

struct ShaderGlobalm_deformer_shader2
{
	GLuint program;
	GLuint _e;
	GLuint _g;
};
struct ShaderUniformm_deformer_shader2
{
	float4 _e[5];
	Texture2D _f;
};
struct ShaderTransferm_deformer_shader2
{
	float4 _a;
	float4 _b;
	float4 _c;
	float4 _d;
};
struct Shaderm_deformer_shader2
{
	ShaderUniformm_deformer_shader2 uniform;
	ShaderTransferm_deformer_shader2 transfer;
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
	GLuint indexBuffers[1];
	GLuint quads;
	ShaderGlobalm_shader1 m_shader1;
	ShaderGlobalm_shader3 m_shader3;
	ShaderGlobalp_particleDeformer_shader2 p_particleDeformer_shader2;
	ShaderGlobalm_deformer_shader2 m_deformer_shader2;
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
	Shaderm_shader1 m_shader1;
	Shaderm_shader3 m_shader3;
	Shaderp_particleDeformer_shader2 p_particleDeformer_shader2;
	Shaderm_deformer_shader2 m_deformer_shader2;
	Transform transforms[4];
	BoundingBox boundingBoxes[1];
	int sceneSequence;
	int deformerSequence;
	int renderSequence;
	int seed;
	GLuint dynamicBuffers[2];
	ubyte* tempBuffer;
};

void initGlobal(void* pGlobal, ubyte* data)
{
	Global& global = *(Global*)pGlobal;

	global.buffers.animXValues = (ushort*)(data + 0);
	global.buffers.animKeys = (ushort*)(data + 8);

	glGenBuffers(1, global.shaderInputBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 72u, data + 28, GL_STATIC_DRAW);

	glGenBuffers(1, global.indexBuffers);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3u, data + 100, GL_STATIC_DRAW);

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
		ShaderGlobalm_shader1& shader = global.m_shader1;
		GLuint vertexShader = createVertexShader(vsm_shader1, "mesh, shader 'shader1'");
		GLuint pixelShader = createPixelShader(psm_shader1, "mesh, shader 'shader1'");
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
		ShaderGlobalm_shader3& shader = global.m_shader3;
		GLuint vertexShader = createVertexShader(vsm_shader3, "mesh, shader 'shader3'");
		GLuint pixelShader = createPixelShader(psm_shader3, "mesh, shader 'shader3'");
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
		ShaderGlobalp_particleDeformer_shader2& shader = global.p_particleDeformer_shader2;
		GLuint vertexShader = createVertexShader(vsp_particleDeformer_shader2, "sprites, deformer 'particleDeformer', shader 'shader2'");
		GLuint pixelShader = createPixelShader(psp_particleDeformer_shader2, "sprites, deformer 'particleDeformer', shader 'shader2'");
		GLuint program = shader.program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "_0");
		glLinkProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		shader._e = glGetUniformLocation(program, "_e");
		glUniform1i(glGetUniformLocation(program, "_f"), 0);
		shader._g = glGetUniformLocation(program, "_g");
	}
	{
		ShaderGlobalm_deformer_shader2& shader = global.m_deformer_shader2;
		GLuint vertexShader = createVertexShader(vsm_deformer_shader2, "mesh, deformer 'deformer', shader 'shader2'");
		GLuint pixelShader = createPixelShader(psm_deformer_shader2, "mesh, deformer 'deformer', shader 'shader2'");
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
}

void doneGlobal(void* pGlobal)
{
	Global& global = *(Global*)pGlobal;

	glDeleteBuffers(1, global.shaderInputBuffers);
	glDeleteBuffers(1, global.indexBuffers);
	glDeleteProgram(global.m_shader1.program);
	glDeleteProgram(global.m_shader3.program);
	glDeleteProgram(global.p_particleDeformer_shader2.program);
	glDeleteProgram(global.m_deformer_shader2.program);
}

void initInstance(const void* pGlobal, void* pInstance)
{
	Global& global = *(Global*)pGlobal;
	Instance& instance = *(Instance*)pInstance;
	instance.global = &global;

	glGenBuffers(2, instance.dynamicBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 60u, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 1280u, NULL, GL_DYNAMIC_DRAW);
	instance.tempBuffer = (ubyte*)malloc(1280u);

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

	glDeleteBuffers(2, instance.dynamicBuffers);
	free(instance.tempBuffer);
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

static void render_m_shader1_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader1& shader = global.m_shader1;
	glUseProgram(shader.program);
	const ShaderUniformm_shader1& uniform = instance.m_shader1.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glUniform4fv(shader._f, 1, (GLfloat*)&uniform._f);
	glBindTexture(GL_TEXTURE_2D, uniform._g);
	glDisable(GL_CULL_FACE);
	const ShaderTransferm_shader1& transfer = instance.m_shader1.transfer;
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
static void render_m_shader3_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_shader3& shader = global.m_shader3;
	glUseProgram(shader.program);
	const ShaderUniformm_shader3& uniform = instance.m_shader3.uniform;
	glUniform4fv(shader._e, 4, (GLfloat*)uniform._e);
	glBindTexture(GL_TEXTURE_2D, uniform._f);
	glEnable(GL_CULL_FACE);
	const ShaderTransferm_shader3& transfer = instance.m_shader3.transfer;
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
static void render_p_particleDeformer_shader2_0(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalp_particleDeformer_shader2& shader = global.p_particleDeformer_shader2;
	const ShaderTransferp_particleDeformer_shader2& transfer = instance.p_particleDeformer_shader2.transfer;
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
static void render_m_deformer_shader2_1(RenderJob* renderJob)
{
	Instance& instance = *(Instance*)renderJob->instance;
	Global& global = *instance.global;
	ShaderGlobalm_deformer_shader2& shader = global.m_deformer_shader2;
	const ShaderTransferm_deformer_shader2& transfer = instance.m_deformer_shader2.transfer;
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
static void draw_a(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 4, GL_UNSIGNED_SHORT, false, 24, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, true, 24, (GLvoid*)20u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_b(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 12, (GLvoid*)0u);
	glDrawArrays(GL_POINTS, 0, instance._particleSystem.numParticles);
}
static void draw_c(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 20, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 20, (GLvoid*)12u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.quads);
	glDrawElements(GL_TRIANGLES, instance._text.numSymbols * 6, GL_UNSIGNED_BYTE, (GLvoid*)0);
}
static void draw_d(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 4, GL_UNSIGNED_SHORT, false, 24, (GLvoid*)0u);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, true, 24, (GLvoid*)20u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[0]);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (GLvoid*)0u);
}
static void draw_e(void* pInstance, void* pShader)
{
	Instance& instance = *(Instance*)pInstance;
	Global& global = *instance.global;
	glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[0]);
	glVertexAttribPointer(0, 4, GL_SHORT, true, 24, (GLvoid*)8u);
	glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, true, 24, (GLvoid*)16u);
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

			// copy
			{
				int numParticles = instance._particleSystem.numParticles;
				Particle_particleSystem* _g = instance._particleSystem.alive;
				ubyte* result = instance.tempBuffer;
				for (int i_ = 0; i_ < numParticles; ++i_)
				{
					float3 _0 = _g->position;
					*(packed_float3*)(result + 0) = _0;

					_g = _g->next;
					result += 12;
				}
				glBufferSubData(GL_ARRAY_BUFFER, 0u, numParticles * 12, instance.tempBuffer);
			}
		}
		{
			glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[1]);

			// copy
			{
				int numVertices = instance._text.numSymbols * 4;
				TextVertex* _g = instance._text.vertices;
				ubyte* result = instance.tempBuffer;
				for (int i_ = 0; i_ < numVertices; ++i_)
				{
					float3 _0 = _g->position;
					float2 _1 = _g->symbol;
					*(packed_float3*)(result + 0) = _0;
					*(float2*)(result + 12) = _1;

					++_g;
					result += 20;
				}
				glBufferSubData(GL_ARRAY_BUFFER, 0u, numVertices * 20, instance.tempBuffer);
			}
		}
	}

	// mesh, shader 'shader1'
	{
		Material_shader1& material = instance.uniforms._shader1;
		Shaderm_shader1& si = instance.m_shader1;
		ShaderUniformm_shader1& uniform = si.uniform;
		ShaderTransferm_shader1& transfer = si.transfer;
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
	// mesh, shader 'shader3'
	{
		Material_shader3& material = instance.uniforms._shader3;
		Shaderm_shader3& si = instance.m_shader3;
		ShaderUniformm_shader3& uniform = si.uniform;
		ShaderTransferm_shader3& transfer = si.transfer;
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
	// sprites, deformer 'particleDeformer', shader 'shader2'
	{
		Deformer_particleDeformer& deformer = instance.uniforms._particleDeformer;
		Material_shader2& material = instance.uniforms._shader2;
		Shaderp_particleDeformer_shader2& si = instance.p_particleDeformer_shader2;
		ShaderUniformp_particleDeformer_shader2& uniform = si.uniform;
		ShaderTransferp_particleDeformer_shader2& transfer = si.transfer;
		float4 a = projectionMatrix.x;
		float4 b = projectionMatrix.y;
		transfer._a = viewMatrix.x;
		transfer._b = viewMatrix.y;
		transfer._c = viewMatrix.z;
		transfer._d = viewMatrix.w;
		uniform._e[0] = a;
		uniform._e[1] = b;
		uniform._e[2] = projectionMatrix.z;
		uniform._e[3] = projectionMatrix.w;
		uniform._e[4].xy = (a * 0.1f + b * 0.2f).xy;
		uniform._e[4].zw = vector2(viewport[2], viewport[3]) * 0.5f;
		uniform._f = material.sampler;
		si.renderJobs = NULL;
	}
	// mesh, deformer 'deformer', shader 'shader2'
	{
		Deformer_deformer& deformer = instance.uniforms._deformer;
		Material_shader2& material = instance.uniforms._shader2;
		Shaderm_deformer_shader2& si = instance.m_deformer_shader2;
		ShaderUniformm_deformer_shader2& uniform = si.uniform;
		ShaderTransferm_deformer_shader2& transfer = si.transfer;
		transfer._a = viewMatrix.x;
		transfer._b = viewMatrix.y;
		transfer._c = viewMatrix.z;
		transfer._d = viewMatrix.w;
		uniform._e[0].x = deformer.scale;
		uniform._e[0].yzw = deformer.offset;
		uniform._e[1] = projectionMatrix.x;
		uniform._e[2] = projectionMatrix.y;
		uniform._e[3] = projectionMatrix.z;
		uniform._e[4] = projectionMatrix.w;
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
				Shaderm_shader1& shader = instance.m_shader1;
				RenderJob* renderJob;
				renderJob = jobIt++;
				renderJob->next = renderQueues.alphaSort;
				renderQueues.alphaSort = renderJob;
				float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
				renderJob->distance = position.z / position.w;
				renderJob->render = &render_m_shader1_1;
				renderJob->draw = &draw_a;
				renderJob->instance = &instance;
				renderJob->matrix = matrix;
			}
			if (jobIt != jobEnd)
			{
				BoundingBox& bb = instance.boundingBoxes[0u];
				Shaderm_shader1& shader = instance.m_shader1;
				RenderJob* renderJob;
				renderJob = jobIt++;
				renderJob->next = renderQueues.alphaSort;
				renderQueues.alphaSort = renderJob;
				float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
				renderJob->distance = position.z / position.w;
				renderJob->render = &render_m_shader1_1;
				renderJob->draw = &draw_d;
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
				Shaderm_deformer_shader2& shader = instance.m_deformer_shader2;
				RenderJob* renderJob;
				renderJob = --jobEnd;
				renderJob->next = shader.renderJobs;
				shader.renderJobs = renderJob;
				renderJob->render = &render_m_deformer_shader2_1;
				renderJob->draw = &draw_e;
				renderJob->instance = &instance;
				renderJob->matrix = matrix;
			}
			if (jobIt != jobEnd)
			{
				BoundingBox& bb = instance.boundingBoxes[0u];
				Shaderm_deformer_shader2& shader = instance.m_deformer_shader2;
				RenderJob* renderJob;
				renderJob = --jobEnd;
				renderJob->next = shader.renderJobs;
				shader.renderJobs = renderJob;
				renderJob->render = &render_m_deformer_shader2_1;
				renderJob->draw = &draw_e;
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
				Shaderp_particleDeformer_shader2& shader = instance.p_particleDeformer_shader2;
				RenderJob* renderJob;
				renderJob = --jobEnd;
				renderJob->next = shader.renderJobs;
				shader.renderJobs = renderJob;
				renderJob->render = &render_p_particleDeformer_shader2_0;
				renderJob->draw = &draw_b;
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
							Shaderm_shader1& shader = instance.m_shader1;
							RenderJob* renderJob;
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_m_shader1_1;
							renderJob->draw = &draw_a;
							renderJob->instance = &instance;
							renderJob->matrix = matrix;
						}
						if (jobIt != jobEnd)
						{
							BoundingBox& bb = instance.boundingBoxes[0u];
							Shaderm_shader1& shader = instance.m_shader1;
							RenderJob* renderJob;
							renderJob = jobIt++;
							renderJob->next = renderQueues.alphaSort;
							renderQueues.alphaSort = renderJob;
							float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
							renderJob->distance = position.z / position.w;
							renderJob->render = &render_m_shader1_1;
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
						float4x4 matrix = particleMatrix * transform.matrix;
						if (jobIt != jobEnd)
						{
							BoundingBox& bb = instance.boundingBoxes[0u];
							Shaderm_deformer_shader2& shader = instance.m_deformer_shader2;
							RenderJob* renderJob;
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_deformer_shader2_1;
							renderJob->draw = &draw_e;
							renderJob->instance = &instance;
							renderJob->matrix = matrix;
						}
						if (jobIt != jobEnd)
						{
							BoundingBox& bb = instance.boundingBoxes[0u];
							Shaderm_deformer_shader2& shader = instance.m_deformer_shader2;
							RenderJob* renderJob;
							renderJob = --jobEnd;
							renderJob->next = shader.renderJobs;
							shader.renderJobs = renderJob;
							renderJob->render = &render_m_deformer_shader2_1;
							renderJob->draw = &draw_e;
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
				Shaderm_shader3& shader = instance.m_shader3;
				RenderJob* renderJob;
				renderJob = jobIt++;
				renderJob->next = renderQueues.alphaSort;
				renderQueues.alphaSort = renderJob;
				float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));
				renderJob->distance = position.z / position.w;
				renderJob->render = &render_m_shader3_0;
				renderJob->draw = &draw_c;
				renderJob->instance = &instance;
				renderJob->matrix = matrix;
			}
		}
	}
	renderQueues.begin = jobIt;
	{
		ShaderGlobalp_particleDeformer_shader2& shader = global.p_particleDeformer_shader2;
		glUseProgram(shader.program);
		const ShaderUniformp_particleDeformer_shader2& uniform = instance.p_particleDeformer_shader2.uniform;
		glUniform4fv(shader._e, 5, (GLfloat*)uniform._e);
		glBindTexture(GL_TEXTURE_2D, uniform._f);
		glDisableVertexAttribArray(1);
		RenderJob* current = instance.p_particleDeformer_shader2.renderJobs;
		while (current)
		{
			current->render(current);
			current = current->next;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	{
		ShaderGlobalm_deformer_shader2& shader = global.m_deformer_shader2;
		glUseProgram(shader.program);
		const ShaderUniformm_deformer_shader2& uniform = instance.m_deformer_shader2.uniform;
		glUniform4fv(shader._e, 5, (GLfloat*)uniform._e);
		glBindTexture(GL_TEXTURE_2D, uniform._f);
		glDisable(GL_CULL_FACE);
		glEnableVertexAttribArray(1);
		RenderJob* current = instance.m_deformer_shader2.renderJobs;
		while (current)
		{
			current->render(current);
			current = current->next;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

