struct
{
	struct
	{
		float3 _input2;
	} _s1n1;
} State;
struct
{
	struct
	{
		float3 _input1;
		float3 _output;
	} _s1n1;
	struct
	{
		float3 _attr;
		float3 _input1;
		float3 _output;
	} _s1n2;
	struct
	{
		float _input1;
	} _s2n1;
	struct
	{
		float _input1;
	} _s3n1;
} T1;
struct
{
	struct
	{
		float _output;
	} _s2n1;
	struct
	{
		float3 _input1;
		float3 _output;
	} _s2n2;
	struct
	{
		float _input2;
	} _s3n1;
} T2;
struct
{
	struct
	{
		float _input1;
	} _s2n1;
	struct
	{
		float3 _output;
	} _s1n2;
} T12;
struct
{
	struct
	{
		float _output;
	} _s3n1;
} T3;
struct
{
	struct
	{
		float _input1;
	} _s3n1;
} T13;
struct
{
	struct
	{
		float _input2;
	} _s3n1;
} T23;

// init state
{
}

// init scope 1
{
}

// update state and scope 1
{
	
	// 'g.s1n1' (ScriptNode)
	{
		scope1._s1n1._output = scope1._s1n1._input1 + state._s1n1._input2;
	}
	scope1._s1n2._attr = scope1._s1n1._output;
	scope1._s1n2._input1 = scope1._s1n1._output;
	
	// 'g.s1n2' (BinOpNode)
	{
		scope1._s1n2._output = scope1._s1n2._input1 * vector3(2.0f, 2.0f, 2.0f);
	}
	scope1._s2n1._input1 = scope1._s1n1._output.x;
	scope1._s3n1._input1 = state._s1n1._input2.y;

	// output to scope 2
	output._s2n1._input1 = scope1._s2n1._input1;
	output._s1n2._output = scope1._s1n2._output;

	// output to scope 3
	output._s3n1._input1 = scope1._s3n1._input1;
}

// init scope 2
{
}

// update scope 2
{
	
	// 'g.s2n1' (BinOpNode)
	{
		scope2._s2n1._output = scope1._s2n1._input1 / 5.0f;
	}
	scope2._s2n2._input1.z = scope1._s1n2._output.y;
	scope2._s2n2._input1.y = scope1._s1n2._output.x;
	scope2._s2n2._input1.x = scope2._s2n1._output;
	
	// 'g.s2n2' (BinOpNode)
	{
		scope2._s2n2._output = scope2._s2n2._input1 * vector3(1.0f, 2.0f, 3.0f);
	}
	scope2._s3n1._input2 = scope2._s2n2._output.x;

	// output to scope 3
	output._s3n1._input2 = scope2._s3n1._input2;
}

// init scope 3
{
}

// update scope 3
{
	
	// 'g.s3n1' (BinOpNode)
	{
		scope3._s3n1._output = scope1._s3n1._input1 - scope2._s3n1._input2;
	}
}
