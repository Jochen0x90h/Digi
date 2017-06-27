#ifndef digi_Nodes_JointNode_h
#define digi_Nodes_JointNode_h

#include <digi/VectorMath/All.h>
#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	A transform node that takes the parent matrix and translate/rotate/scale
	parameters as inputs and calculates an output matrix:
	transform formula is P * T * iS * Jo * R * Ro * S
	where
	P: parent matrix
	T: translate
	iS: inverse parent scale
	Jo: joint orientation
	R: rotate
	Ro: rotate orientation
	S: scale

	inputs:
		parentMatrix (reference, float4x4)
		parentScale (reference, float3x3)
		translate
		rotate
		scale
	outputs:
		scaleMatrix (hidden, float4x4)
		worldMatrix (float4x4)
*/
class JointNode : public Node
{
	public:
		
		enum TranslateMode
		{
			TRANSLATE_NONE,
			TRANSLATE_X,
			TRANSLATE_Y,
			TRANSLATE_Z,
			TRANSLATE_GENERAL,
		};
		
		enum RotateMode
		{
			ROTATE_NONE,
			ROTATE_HINGE_X,
			ROTATE_HINGE_Y,
			ROTATE_HINGE_Z,
			ROTATE_SADDLE_YZ,
			ROTATE_SADDLE_ZX,
			ROTATE_SADDLE_XY,
			ROTATE_BALL_XYZ,
			ROTATE_BALL_YZX,
			ROTATE_BALL_ZXY,
			ROTATE_RQ,
			ROTATE_EULER_XYZ,
			ROTATE_EULER_YZX,
			ROTATE_EULER_ZXY,
			ROTATE_EULER_XZY,
			ROTATE_EULER_YXZ,
			ROTATE_EULER_ZYX,
			ROTATE_GENERAL,
		};

		enum ScaleMode
		{
			SCALE_NONE,
			SCALE_UNIFORM,
			SCALE_AXIS
		};
		
		JointNode() : translateMode(TRANSLATE_NONE), rotateMode(ROTATE_NONE), scaleMode(SCALE_NONE) {}

		JointNode(const Path& parentMatrix, const Path& parentScaleMatrix,
			TranslateMode translateMode, RotateMode rotateMode, ScaleMode scaleMode);

		virtual ~JointNode();

		virtual std::string getNodeType();
		
		TranslateMode getTranslateMode() { return this->translateMode; }
		RotateMode getRotateMode() { return this->rotateMode; }
		ScaleMode getScaleMode() { return this->scaleMode; }
		

		virtual void generateUpdateCodeThis(NodeWriter& w);
		
		std::string getTranslate();
		std::string getRotate();
	
		static int getNumTranslateComponents(TranslateMode translateMode);
		static std::string getTranslateType(TranslateMode translateMode);

		static int getNumRotateComponents(RotateMode rotateMode);
		static std::string getRotateType(RotateMode rotateMode);
		static char getAxis(int axis);
		static const char* getEulerOrder(RotateMode rotateMode);

		static int getNumScaleComponents(ScaleMode scaleMode);
		static std::string getScaleType(ScaleMode scaleMode);
	
		// joint orientation
		floatQuaternion jointOrientation;
		
		// rotate orientation
		floatQuaternion rotateOrientation;

	protected:
			
		/// translate mode
		TranslateMode translateMode;

		/// rotate mode
		RotateMode rotateMode;

		/// scale mode
		ScaleMode scaleMode;
};


/// @}

} // namespace digi

#endif
