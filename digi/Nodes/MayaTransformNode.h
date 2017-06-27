#ifndef digi_Nodes_MayaTransformNode_h
#define digi_Nodes_MayaTransformNode_h

#include <digi/Math/All.h>

#include "MayaDagNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Maya transform node that takes an array of parent matrices and translate/rotate/scale
	parameters as inputs to calculate an array of world matrices

	inputs:
		visibility (bool)
		translate (float3)
		rotate (float3)
		shear (shear, only transform)
		scale (float3)
		parentScale (float3, only joint)
		instance
			0
				parentMatrix (float4x4)
				parentVisibility (bool)
			1
				...

	outputs:
		matrix (float4x4)
		instance
			0
				worldMatrix (float4x4)
				worldVisibility (bool)
				parentInverseMatrix (float4x4, optional)
			1
				...
*/
class MayaTransformNode : public MayaDagNode
{
	public:
		
		enum Type
		{
			TYPE_TRANSFORM,
			TYPE_JOINT
		};
		
		enum RotateMode
		{
			ROTATE_EULER_XYZ,
			ROTATE_EULER_YZX,
			ROTATE_EULER_ZXY,
			ROTATE_EULER_XZY,
			ROTATE_EULER_YXZ,
			ROTATE_EULER_ZYX
		};

		MayaTransformNode(Type type = TYPE_TRANSFORM);

		virtual ~MayaTransformNode();

		virtual std::string getNodeType();
		
		virtual void optimize(OptimizePhase phase);

		virtual void writeUpdateCodeThis(NodeWriter& w);

		void addInstance(StringRef instanceString, const Path& parentInstance, bool inheritsTransform = true);

		//static std::string getRotate(RotateMode rotateMode, const std::string& rotate);
		static const char* getEulerOrder(RotateMode rotateMode);
		
		static floatQuaternion quaternionEuler(RotateMode rotateMode, float3 angles);

		Type type;

		/// rotate mode
		RotateMode rotateMode;

		// rotate pivots
		float3 rotatePivotTranslation;
		float3 rotatePivotPoint;
			
		// rotate orientation
		floatQuaternion rotateOrientation;
		
		// scale pivots
		float3 scalePivotTranslation;
		float3 scalePivotPoint;
		
		// joint orientation
		floatQuaternion jointOrientation;
};


/// @}

} // namespace digi

#endif
