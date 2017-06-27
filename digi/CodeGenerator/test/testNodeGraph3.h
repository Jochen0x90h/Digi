struct
{
	struct
	{
		float3 _input2;
	} _n1;
} state;

struct
{
	struct
	{
		float3 _input1;
		float3 _input2;
		float3 _output;
	} _n3;
	struct
	{
		float3 _input1;
		float3 _output;
	} _n2;
	struct
	{
		float3 _output;
		float3 _input1;
	} _n1;
} s1;

// init state
{
	state._n1._input2 = vector3(1.0f, 2.0f, 3.0f);
}

// init scope 1
{
}

// update state and scope 1
{
	
	// 'g.n1' (ScriptNode)
	{
		s1._n1._output = s1._n1._input1 + state._n1._input2;
	}
	s1._n2._input1 = s1._n1._output;
	
	// 'g.n2' (Node)
	{
		struct
		{
			struct
			{
				float3 _input1;
			} _n2;
			struct
			{
				float3 _output;
				float3 _input1;
			} _n1;
		} inner;
		
		// 'g.n1' (ScriptNode)
		{
			inner._n1._output = inner._n1._input1 + state._n1._input2;
		}
		inner._n2._input1 = inner._n1._output;
		float3 _input1 = inner._n2._input1;
		s1._n2._output = _input1;
	}
	s1._n3._input1 = s1._n2._input1;
	s1._n3._input2 = s1._n2._output;
	
	// 'g.n3' (BinOpNode)
	{
		s1._n3._output = s1._n3._input1 * s1._n3._input2;
	}
	
}
