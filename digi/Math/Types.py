#
# vector types
#
types = [
	{'type':'bool',   'base':'bool' , 'range':0, 'signed':0, 'size':1},
	{'type':'byte',   'base':'int'  , 'range':1, 'signed':1, 'size':1},
	{'type':'ubyte',  'base':'int'  , 'range':1, 'signed':0, 'size':1},
	{'type':'short',  'base':'int'  , 'range':2, 'signed':1, 'size':2},
	{'type':'ushort', 'base':'int'  , 'range':2, 'signed':0, 'size':2},
	{'type':'int',    'base':'int'  , 'range':3, 'signed':1, 'size':4},
	{'type':'uint',   'base':'int'  , 'range':3, 'signed':0, 'size':4},
	{'type':'long',   'base':'int'  , 'range':4, 'signed':1, 'size':8},
	{'type':'ulong',  'base':'int'  , 'range':4, 'signed':0, 'size':8},
	{'type':'half',   'base':'float', 'range':5, 'signed':1, 'size':2},
	{'type':'float',  'base':'float', 'range':6, 'signed':1, 'size':4},
	{'type':'double', 'base':'float', 'range':7, 'signed':1, 'size':8}
]

cTypes = {
	'bool'  :'bool',
	'byte'  :'int8_t', 
	'ubyte' :'uint8_t', 
	'short' :'int16_t',
	'ushort':'uint16_t', 
	'int'   :'int32_t', 
	'uint'  :'uint32_t', 
	'long'  :'int64_t', 
	'ulong' :'uint64_t', 
	'half'  :'half', 
	'float' :'float', 
	'double':'double', 
}

lengths = [1, 2, 3, 4]


define = open('TypesDefine.inc.h', 'w')
undef = open('TypesUndef.inc.h', 'w')
f = open('VectorTypes.inc.h', 'w')



# typedefs for scalar and vector
for length in lengths:
	l = str(length)
	for t in types:
		type = t['type']
		cType = cTypes[type]
		if length == 1:
			if type != cType:
				define.write('#define ' + type + ' ' + cType + '\n')
				undef.write('#undef ' + type + '\n')
		else:
			f.write('typedef Vector' + l + '<' + cType + '> ' + type + l + ';\n')
	f.write('\n')


# typedefs for packed vectors
for length in [2, 3, 4]:
	l = str(length)
	for t in types:
		type = t['type']
		f.write('typedef ' + type + l + ' packed_' + type + l + ';\n')
	f.write('\n')


# typedefs and make_<type> for vector 2 to 4
for length in [2, 3, 4]:
	l = str(length)
	for t in types:		
		type = t['type']
		cType = cTypes[type]
				
		templateArgs = ''
		args = ''
		params = ''
		for i in range(length):
			if i != 0:
				templateArgs += ', '
				args += ', '
				params += ', '
			templateArgs += 'typename T' + str(i)
			args += 'T' + str(i) + ' ' + 'xyzw'[i]
			params += cType + '(' + 'xyzw'[i] + ')'
			
		f.write('template <' + templateArgs + '> ' + type + l + ' make_'
			+ type + l + '(' + args + ') {return vector' + l + '(' + params + ');}\n')
		
	f.write('\n')


# convert_<type>
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			destCType = cTypes[destType]
			srcCType = cTypes[srcType]
			if length == 1:
				destCVType = destCType;
				srcCVType = srcCType;
			else:
				destType += l;
				srcType += l;
				destCVType = destType;
				srcCVType = srcType;

			if destBase != 'bool': # and srcBase != 'bool':
				f.write('inline ' + destCVType + ' convert_' + destType + '(' + srcCVType + ' v) {return ')

				if destType == srcType:
					f.write('v');
				else:
					if length == 1:
						f.write(destCType + '(v)')
					else:
						params = ''
						for i in range(length):
							if i != 0:
								params += ', '
							params += destCType + '(v.' + 'xyzw'[i] + ')'

						f.write('vector' + l + '(' + params + ')')

				f.write(';}\n')
			
		f.write('\n')
	f.write('\n')


# convert_<type>_rte, only implemented for floating point to integer
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			destCType = cTypes[destType]
			srcCType = cTypes[srcType]
			if length == 1:
				destCVType = destCType;
				srcCVType = srcCType;
			else:
				destType += l;
				srcType += l;
				destCVType = destType;
				srcCVType = srcType;

			if destBase == 'int' and srcBase == 'float':
				f.write('inline ' + destCVType + ' convert_' + destType + '_rte(' + srcCVType + ' v) {return ')

				if length == 1:
					f.write('rint<' + destCType + '>(v)')
				else:
					params = ''
					for i in range(length):
						if i != 0:
							params += ', '

						params += 'rint<' + destCType + '>(v.' + 'xyzw'[i] + ')'

					f.write('vector' + l + '(' + params + ')')

				f.write(';}\n')
			
		f.write('\n')
	f.write('\n')


# convert_<type>_sat
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']
			destRange = dt['range']
			srcRange = st['range']
			destSigned = dt['signed']
			srcSigned = st['signed']

			destCType = cTypes[destType]
			srcCType = cTypes[srcType]
			if length == 1:
				destCVType = destCType;
				srcCVType = srcCType;
			else:
				destType += l;
				srcType += l;
				destCVType = destType;
				srcCVType = srcType;

			if destBase != 'bool' and destBase != 'float' and srcBase != 'bool':
				
				f.write('inline ' + destCVType + ' convert_' + destType + '_sat(' + srcCVType + ' v) {return ')
				
				if destType == srcType:
					f.write('v');
					
				else:
					if destBase == 'float':
						minVal = '-std::numeric_limits<' + destCType + '>::max()'
					else:
						minVal = 'std::numeric_limits<' + destCType + '>::min()'
					maxVal = 'std::numeric_limits<' + destCType + '>::max()'
					
					if destRange < srcRange:
						# destination range is smaller: clamp
						f.write('clamp_rtz(v, '
							+ minVal + ', '
							+ maxVal + ')')
					elif destRange == srcRange:
						# destination has same range as source: check sign
						if destSigned == 0 and srcSigned == 1:
							f.write('max_rtz(v, ' + destCType + '(0))')
						else:
							f.write('min_rtz(v, ' + maxVal + ')')
					else:
						# destination range is bigger: only convert
						f.write('convert_' + destType + '(v)')
					
				f.write(';}\n')

		f.write('\n')
	f.write('\n')


# convert_<type>_sat_rte
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']

			destCType = cTypes[destType]
			srcCType = cTypes[srcType]
			if length == 1:
				destCVType = destCType;
				srcCVType = srcCType;
			else:
				destType += l;
				srcType += l;
				destCVType = destType;
				srcCVType = srcType;

			if destBase == 'int' and srcBase == 'float':
				
				f.write('inline ' + destCVType + ' convert_' + destType + '_sat_rte(' + srcCVType + ' v) {return ')
				
				if destBase == 'float':
					minVal = '-std::numeric_limits<' + destCType + '>::max()'
				else:
					minVal = 'std::numeric_limits<' + destCType + '>::min()'
				maxVal = 'std::numeric_limits<' + destCType + '>::max()'
				
				f.write('clamp_rte(v, '
					+ minVal + ', '
					+ maxVal + ')')
				
				f.write(';}\n')

		f.write('\n')
	f.write('\n')


# as_<type> to reinterpret types
for length in lengths:
	l = str(length)
	for dt in types:
		for st in types:
			destType = dt['type']
			srcType = st['type']
			destBase = dt['base']
			srcBase = st['base']
			destSize = dt['size']
			srcSize = st['size']

			destCType = cTypes[destType]
			srcCType = cTypes[srcType]
			if length == 1:
				destCVType = destCType;
				srcCVType = srcCType;
			else:
				destType += l;
				srcType += l;
				destCVType = destType;
				srcCVType = srcType;

			if destType != srcType and destBase != 'bool' and srcBase != 'bool' and destSize == srcSize:		
				f.write('inline ' + destCVType + ' as_' + destType + '(' + srcCVType + ' v) {return ')
				f.write('*(' + destCVType + '*)&v;}\n')
			
		f.write('\n')

define.close();
undef.close();
f.close();


# defines for virtual machine (almost opencl emulation)
#~ define = open('TypesDefine.inc.h', 'w')
#~ undef = open('TypesUndef.inc.h', 'w')

#~ for t in types:
	#~ type = t['type']

	#~ if type != cTypes[type]:

		#~ define.write('#define ' + type + ' ' + type + '1\n')
		#~ undef.write('#undef ' + type + '\n')

		#~ if type != 'bool':
			#~ define.write('#define convert_' + type + ' convert_' + type + '1\n')
			#~ define.write('#define convert_' + type + '_rte convert_' + type + '1_rte\n')
			#~ define.write('#define convert_' + type + '_sat convert_' + type + '1_sat\n')
			#~ define.write('#define convert_' + type + '_sat_rte convert_' + type + '1_sat_rte\n')

			#~ undef.write('#undef convert_' + type + '\n')
			#~ undef.write('#undef convert_' + type + '_rte\n')
			#~ undef.write('#undef convert_' + type + '_sat\n')
			#~ undef.write('#undef convert_' + type + '_sat_rte\n')

#~ for bc in bitCasts:
	#~ destType = bc['destType']
	
	#~ if destType != cTypes[destType]:
		#~ define.write('#define as_' + destType + ' as_' + destType + '1\n')
		#~ undef.write('#undef as_' + destType + '\n')


#~ define.close();
#~ undef.close();


#
# matrix types
#
types = [
	{'type':'float'},
	{'type':'double'}
]


lengths1 = [2, 3, 4]
lengths2 = [2, 3, 4]

f = open('MatrixTypes.inc.h', 'w')

# typedef for matrix
for length1 in lengths1:
	l1 = str(length1)
	for length2 in lengths2:
		l2 = str(length2)

		for t in types:
			type = t['type']
			
			vectorType = type
			if length1 > 1:
				vectorType += l1
				
			f.write('typedef Matrix' + l2 + '<' + vectorType + '> ' + type + l1 + 'x' + l2 + ';\n')
		f.write('\n')

# convert for matrix
for length1 in lengths1:
	l1 = str(length1)
	for length2 in lengths2:
		l2 = str(length2)
	
		for dt in types:
			for st in types:
				destType = dt['type']
				srcType = st['type']

				vectorType = destType
				if length1 > 1:
					vectorType += l1

				f.write('inline ' + destType + l1 + 'x' + l2 + ' convert_' + destType + l1 + 'x' + l2 + '(const ' + srcType + l1 + 'x' + l2 + '& m) {return ')

				if destType == srcType:
					f.write('m')

				else:
					params = ''
					for i in range(length2):
						if i != 0:
							params += ', '
						params += 'convert_' + vectorType + '(m.' + 'xyzw'[i] + ')'

					f.write('matrix' + l2 + '(' + params + ')')

				f.write(';}\n')
				
			f.write('\n')
		f.write('\n')

f.close();
