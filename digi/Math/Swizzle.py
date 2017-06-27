d = open('SwizzleDefine.inc.h', 'w')
u = open('SwizzleUndef.inc.h', 'w')

d.write("#ifndef BUILTIN_VECTORS\n")
u.write("#ifndef BUILTIN_VECTORS\n")

# iterate over input vector lengths
for inLength in [2, 3, 4]:

	inLenStr = str(inLength)
	f = open('Vector' + inLenStr + '.inc.h', 'w')
	g = open('Vector' + inLenStr + 'Ref.inc.h', 'w')

	# iterate over output vector lengths
	for outLength in [2, 3, 4]:

		outLenStr = str(outLength)
		
		# iterate over all swizzle combinations for the current vector lengths
		# ** is a floating point operation, therefore use - 0.5
		for i in range(int(inLength ** outLength - 0.5)):

			xyzw = 'xyzw'
			
			# calc swizzle indices
			j = i
			xi = j % inLength
			j /= inLength
			yi = j % inLength
			j /= inLength
			zi = j % inLength
			j /= inLength
			wi = j % inLength

			x = xyzw[xi]
			y = xyzw[yi]
			z = xyzw[zi]
			w = xyzw[wi]
			
			# get swizzle, e.g. "xyz"
			swizzle = (x + y + z + w)[0:outLength]
			# print f "$swizzle ";
			
			constructor = ('this->' + x + ', this->' + y + ', this->' + z + ', this->' + w)[0:outLength*9 - 2]
			
			f.write('const Vector' + outLenStr + '<Type> ' + swizzle + '() const {Vector' + outLenStr + '<Type> a = {' + constructor + '}; return a;}\n')
			
			# generate a vector of references
			# for all swizzles without duplicate components (e.g. .xyz)
			returnRef = 1
			if xi == yi:
				returnRef = 0
			if outLength >= 3 and (xi == zi or yi == zi):
				returnRef = 0
			if outLength >= 4 and (xi == wi or yi == wi or zi == wi):
				returnRef = 0

			if returnRef != 0:
				f.write('Vector' + outLenStr + 'Ref<Type> ' + swizzle + '() {return Vector' + outLenStr + 'Ref<Type>(' + constructor + ');}\n')

				# swizzle for vector reference
				constructor = ('this->' + x + 'Ref, this->' + y + 'Ref, this->' + z + 'Ref, this->' + w + 'Ref')[0:outLength*12 - 2]
				g.write('Vector' + outLenStr + 'Ref<Type> ' + swizzle + '() {return Vector' + outLenStr + 'Ref<Type>(' + constructor + ');}\n')

			if inLength == 4:
				d.write('#define ' + swizzle + ' ' + swizzle + '()\n')
				u.write('#undef ' + swizzle + '\n')
	f.close()
	g.close()
	
d.write('#endif\n')
u.write('#endif\n')

d.close()
u.close()
