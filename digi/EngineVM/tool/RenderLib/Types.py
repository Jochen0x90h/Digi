#
# vector types
#
types = [
	{'type':'bool',   'base':'bool'},
	{'type':'byte',   'base':'int'},
	{'type':'ubyte',  'base':'int'},
	{'type':'short',  'base':'int'},
	{'type':'ushort', 'base':'int'},
	{'type':'int',    'base':'int'},
	{'type':'uint',   'base':'int'},
	#{'type':'long',   'base':'int'},
	#{'type':'ulong',  'base':'int'},
	{'type':'float',  'base':'float'},
	#{'type':'double', 'base':'float'}
]

prefix = '__attribute__((const)) '

f = open('VectorTypes.inc.h', 'w')
	
# make_<type> for vector 2 to 4
for length in [2, 3, 4]:
	l = str(length)
	for t in types:		
		type = t['type']
		
		args = ''
		params = ''
		for i in range(length):
			if i != 0:
				args += ', '
				params += ', '
			args += 'xyzw'[i]
			params += '(' + 'xyzw'[i] + ')'
			
		f.write('#define make_' + type + l + '(' + args + ') (' + type + l + ')(' + params + ')\n')
		
	f.write('\n')


lengths = [1, 2, 3, 4]

# convert_<type> for scalar and vector
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			if length > 1:
				destType += l
				srcType += l

			if destBase != 'bool': # and srcBase != 'bool':				
				f.write(prefix + destType + ' convert_' + destType + '(' + srcType + ' v);\n')
			
		f.write('\n')
	f.write('\n')


# convert_<type>_rte for scalar and vector
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			if length > 1:
				destType += l
				srcType += l

			if destBase == 'int' and srcBase == 'float':
				f.write(prefix + destType + ' convert_' + destType + '_rte(' + srcType + ' v);\n')

		f.write('\n')
	f.write('\n')


# saturate

# convert_<type>_sat for scalar and vector
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			if length > 1:
				destType += l
				srcType += l

			if destBase != 'bool' and srcBase != 'bool':				
				f.write(prefix + destType + ' convert_' + destType + '_sat(' + srcType + ' v);\n')

		f.write('\n')
	f.write('\n')


# convert_<type>_sat_rte for scalar and vector
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			if length > 1:
				destType += l
				srcType += l

			if destBase == 'int' and srcBase == 'float':
				f.write(prefix + destType + ' convert_' + destType + '_sat_rte(' + srcType + ' v);\n')

		f.write('\n')
	f.write('\n')


# reinterpreting types
bitCasts = [
	{'srcType':'int',    'destType':'float'},
	{'srcType':'uint',   'destType':'float'},
	{'srcType':'float',  'destType':'int'},
	{'srcType':'float',  'destType':'uint'},
	#{'srcType':'long',   'destType':'double'},
	#{'srcType':'ulong',  'destType':'double'},
	#{'srcType':'double', 'destType':'long'},
	#{'srcType':'double', 'destType':'ulong'},
]

for length in lengths:
	l = str(length)
	for bc in bitCasts:
		srcType = bc['srcType']
		destType = bc['destType']
		if length > 1:
			srcType += l
			destType += l
		f.write(prefix + destType + ' as_' + destType + '(' + srcType + ' v);\n')

	f.write('\n')

f.close();


#
# matrix types
#
types = [
	{'type':'float'},
#	{'type':'double'}
]


#lengths1 = [1, 2, 3, 4]
lengths1 = [2, 3, 4]
lengths2 = [2, 3, 4]

f = open('MatrixTypes.inc.h', 'w')

# convert for matrix
for length1 in lengths1:
	l1 = str(length1)
	for length2 in lengths2:
		l2 = str(length2)
	
		for dt in types:
			for st in types:
				destType = dt['type']
				srcType = st['type']

				vectorType = destType;
				if length1 > 1:
					vectorType += l1;

				f.write(prefix + destType + l1 + 'x' + l2 + ' convert_' + destType + l1 + 'x' + l2 + '(' + srcType + l1 + 'x' + l2 + ' m);\n')
				
			f.write('\n')
		f.write('\n')

f.close();
