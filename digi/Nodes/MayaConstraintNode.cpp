#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "MayaConstraintNode.h"


namespace digi {


MayaConstraintNode::MayaConstraintNode(Type type, int numTargets)
	: type(type), targets(numTargets), constraintRotateMode(MayaTransformNode::ROTATE_EULER_XYZ)
{
}

MayaConstraintNode::~MayaConstraintNode()
{
}

std::string MayaConstraintNode::getNodeType()
{
	return "MayaConstraintNode";
}

void MayaConstraintNode::writeUpdateCodeThis(NodeWriter& w)
{
	// http://en.wikipedia.org/wiki/QR_decomposition
	// http://de.wikipedia.org/wiki/Givens-Rotation
	// http://en.wikipedia.org/wiki/Euler_angles

	bool haveTranslate = false;
	bool haveRotate = false;
	bool haveScale = false;
	
	if (this->type == AIM)
	{
		haveTranslate = true;
	}
	else
	{
		haveTranslate = this->hasAttribute("outputTranslate");
		haveRotate = this->hasAttribute("outputRotate");
		haveScale =  this->hasAttribute("outputScale");
	}
	 
	size_t numTargets = this->targets.size();

	if (haveRotate || haveScale)
	{
		w << "float3x3 $Q;\n";
		w << "float3x3 $R;\n";
	}
	
	// step 1: calculate translate and rotate in world space for all targets
	for (size_t i = 0; i < numTargets; ++i)
	{
		Target& target = this->targets[i];
		std::string ts = "$.target" + toString(i) + ".";
		
		if (haveRotate || haveScale)
		{
			// decompose target parent matrix using QR decomposition
			w << "qrDecomposition(getMatrix3x3(" << ts << "parentMatrix), $Q, $R);\n";
		}
		
		if (haveRotate)
		{
			// calc rotation quaternion for target local rotation (targetLocalRotateX =
			// target.jointOrientation * target.rotate)
			w << "floatQuaternion $targetLocalRotate" << i << " = ";
			if (getAngle(target.jointOrientation) > 1e-6f)
				w << target.jointOrientation << "\n\t* ";	
			w << "quaternionEuler" << MayaTransformNode::getEulerOrder(target.rotateMode) << "(" << ts + "rotate);\n";
			
			// calc total rotation in world space (targetRotateX =
			// (rotate of parent matrix) * targetLocalRotateX * target.rotateOffset)
			w << "floatQuaternion $targetRotate";
			if (numTargets > 1)
				w << i;
			w << " = getRotation($Q) * $targetLocalRotate" << i;
			if (getAngle(target.rotateOffset) > 1e-6f)
				w << "\n\t* " << target.rotateOffset;
			w << ";\n";
		}

		if (haveTranslate)
		{
			// calc total translate in world space (targetTranslateX = 
			// parentMatrix * (target.pivotTranslate + target.translate + targetLocalRotateX * target.translateOffset))
			w << "float3 $targetTranslate";
			if (numTargets > 1)
				w << i;
			w << " = transformPosition(" << ts << "parentMatrix,\n\t";
			if (length(target.pivotTranslate) > 1e-6f)
				w << target.pivotTranslate << " + ";
			w << ts << "translate";
			if (length(target.translateOffset) > 1e-6f)
				w << "\n\t+ matrix3x3Rotate($targetLocalRotate" << i << ") * " << target.translateOffset;			
			w << ");\n";
		}
		
		if (haveScale)
		{
			// calc total scale (targetScaleX = diag(scale of parent matrix) * target.scale)
			w << "float3 $targetScale";
			if (numTargets > 1)
				w << i;
			w << " = diag($R) * " << ts << "scale;\n";
		}		
	}
		
	// step 2: weighted average
	if (numTargets > 1)
	{
		if (haveTranslate)
			w << "float3 $targetTranslate = $translate0 * $.target0.weight;\n";
		if (haveRotate)
			w << "floatQuaternion $targetRotate = $rotate0;\n";
		if (haveScale)
			w << "float3 $targetScale = $scale0 * $.target0.weight;\n";
		
		w << "float $weight = $.target0.weight;\n";
		for (size_t i = 1; i < numTargets; ++i)
		{
			w.push(Path(this, "$.target" + toString(i)));
			//std::string ts = "$.target" + toString(i) + ".";
			
			// accumulate weighted translate
			if (haveTranslate)
				//w << "$targetTranslate += $targetTranslate" << i << " * " << ts << "weight;\n";
				w << "$targetTranslate += $targetTranslate" << i << " * $.weight;\n";
			
			// interpolate rotate
			if (haveRotate)
				//w << "$targetRotate = slerpShortest($targetRotate, $targetScale" << i << ", $weight / ($weight + " << ts << "weight));\n";
				w << "$targetRotate = slerpShortest($targetRotate, $targetScale" << i << ", $weight / ($weight + $.weight));\n";
			
			// accumulate weighted scale
			if (haveScale)
				//w << "$targetScale += $targetScale" << i << " * " << ts << "weight;\n";
				w << "$targetScale += $targetScale" << i << " * $.weight;\n";
			
			// accumulate weight
			//w << "$weight += " << ts << "weight;\n";
			w << "$weight += $.weight;\n";

			w.pop();
		}
		
		// normalize translate by total weight
		if (haveTranslate)
			w << "$targetTranslate /= $weight;\n";

		// normalize scale by total weight
		if (haveScale)
			w << "$targetScale /= $weight;\n";
	}
	
	// step 3: apply to target
	if (this->hasAttribute(".constraintParentInverseMatrix"))
		w << "float4x4 $constraintParentInverseMatrix = $.constraintParentInverseMatrix;\n";
	else
		w << "float4x4 $constraintParentInverseMatrix = inv($.constraintParentMatrix);\n";

	if (haveTranslate)
	{
		w << "float3 $translateOffset = ";
		if (this->hasAttribute("constraintTranslate"))
		{
			if (length(this->translateOffset) > 1e-6f)
				w << this->translateOffset << " - ";
			else
				w << '-';
			w << "$.constraintTranslate";
		}
		else
		{
			w << this->translateOffset;
		}
		w << ";\n";

		if (this->type != AIM)
		{
			// calc translate in constraint space
			w << "$.outputTranslate = transformPosition($constraintParentInverseMatrix, $targetTranslate)"
				" + $translateOffset;\n";
		}
	}
		
	if (haveRotate || haveScale)
	{
		// decompose constraint parent inverse matrix using QR decomposition
		w << "qrDecomposition(getMatrix3x3($constraintParentInverseMatrix), $Q, $R);\n";
	}
			
	if (haveRotate)
	{
		// calc rotate in constraint space (rotate * constraint rotate offset * Q^-1)
		w << "$.outputRotate = getEuler" << MayaTransformNode::getEulerOrder(this->constraintRotateMode) << "(\n\t";
		if (getAngle(this->constraintJointOrientation) > 1e-6f)
			w << conj(this->constraintJointOrientation) << "\n\t* ";
		w << "getRotation($Q) * $targetRotate";
		if (getAngle(this->rotateOffset) > 1e-6f)
			w << "\n\t* " << this->rotateOffset;	
		w << ");\n";
	}	

	if (haveScale)
	{
		// calc scale
		w << "$.outputScale = vector3($R.x.x, $R.y.y, $R.z.z) * $targetScale";
		if (length(this->scaleOffset - 1.0f) > 1e-6f)
			w << "\n\t* " << this->scaleOffset;
		w << ";\n";
	}

	if (this->type == AIM)
	{
		// calc actual aim vector in space of constrained transform node
		w << "float3 $aimVector = transformPosition($constraintParentInverseMatrix, $targetTranslate)"
			" + $translateOffset;\n";
		
		if (!this->hasAttribute("aimVector"))
		{
			// use up vector
			
			// calc up vector in space of constrained transform node
			if (this->hasAttribute("worldUpMatrix"))
			{
				// up vector is position of a transform node (worldUpMatrix) relative to constrained transform node
				w << "float3 $upVector = transformPosition($constraintParentInverseMatrix, $.worldUpMatrix.w.xyz)"
					" + $translateOffset;\n";
			}
			else
			{
				// up vector directly given in world space
				w << "float3 $upVector = transformDirection($constraintParentInverseMatrix, $.worldUpVector);\n";
			}
		
			// transformation order is (current aim) * (twist about x) * inv(desired aim)

			// calc current aim matrix
			w << "float3 $x = normalize($aimVector);\n";
			w << "float3 $z = normalize(cross($x, $upVector));\n";
			w << "float3 $y = cross($z, $x);\n";
			w << "floatQuaternion $aimRotate = getRotation(matrix3($x, $y, $z))";
			if (this->hasAttribute("twist"))
				w << " * quaternionEulerX($.twist)";
			w << ";\n";
		}
		else
		{
			// no up vector

			// calc rotation that aligns actual with desired aim vector
			w << "float3 $a = normalize($.aimVector);\n";
			w << "float3 $b = normalize($aimVector);\n";
			w << "floatQuaternion $aimRotate = sqrtUnit(quaternion(cross($a, $b), dot($a, $b)));\n";
		}

		w << "$.outputRotate = getEuler" << MayaTransformNode::getEulerOrder(this->constraintRotateMode) << "(\n\t";
		if (getAngle(this->constraintJointOrientation) > 1e-6f)
			w << conj(this->constraintJointOrientation) << "\n\t* ";
		w << "$aimRotate";
		if (getAngle(this->rotateOffset) > 1e-6f)
			w << "\n\t* " << this->rotateOffset;	
		w << ");\n";
	}
}


} // namespace digi
