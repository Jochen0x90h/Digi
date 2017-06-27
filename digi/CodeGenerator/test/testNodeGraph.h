struct
{
	struct
	{
		float3 _input2;
	} _n1;
	struct
	{
		float3 _attr;
	} _n2;
} State;
struct
{
	struct
	{
		float3 _input1;
		float3 _output;
	} _n1;
	struct
	{
		float3 _input1;
		float3 _output;
	} _n2;
} T1Local;
struct
{
	struct
	{
		float _input1;
		float _output;
	} _n3;
	struct
	{
		float _input1;
		float _output;
	} _n4;
} T2Local;
struct
{
	struct
	{
		float _input1;
	} _n3;
} T1T2;
struct
{
	struct
	{
		float _input1;
		float _input2;
		float _output;
	} _n5;
} T3Local;
struct
{
	struct
	{
		float _input1;
	} _n5;
} T1T3;
struct
{
	struct
	{
		float _input2;
	} _n5;
} T2T3;

// init state
{
	state._n1._input2 = vector3(1.0f, 2.0f, 3.0f);
}

// init g1
{
}

// update g1
{
	
	// 'g1.n1' (ScriptNode)
	{
		local._n1._output = local._n1._input1 + state._n1._input2;
	}
	state._n2._attr = local._n1._output;
	local._n2._input1 = local._n1._output;
	
	// 'g1.n2' (BinOpNode)
	{
		local._n2._output = local._n2._input1 * vector3(2.0f, 2.0f, 2.0f);
	}

	// output to g2
	output.g2._n3._input1 = local._n2._output.x;

	// output to g3
	output.g3._n5._input1 = local._n2._output.y;
}

// init g2
{
}

// update g2
{
	// input from g1
	local._n3._input1 = input._n3._input1;

	
	// 'g2.n3' (BinOpNode)
	{
		local._n3._output = local._n3._input1 / 5.0f;
	}
	local._n4._input1 = local._n3._output;
	
	// 'g2.n4' (BinOpNode)
	{
		local._n4._output = local._n4._input1 * 2.0f;
	}

	// output to g3
	output.g3._n5._input2 = local._n4._output;
}

// init g3
{
}

// update g3
{
	// input from g1
	local._n5._input1 = i1._n5._input1;

	// input from g2
	local._n5._input2 = i2._n5._input2;

	
	// 'g3.n5' (BinOpNode)
	{
		local._n5._output = local._n5._input1 - local._n5._input2;
	}
}
