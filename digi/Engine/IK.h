#ifndef digi_Engine_IK_h
#define digi_Engine_IK_h

#include <digi/Utility/Standard.h>
#include <digi/Math/All.h>


namespace digi {

/// @addtogroup Engine
/// @{
	

namespace IK2D
{
	struct Joint
	{
		float rotate;
		float2 translate;

		float2 worldPosition;
	};

	void runCCD(Joint* joints, int numJoints, float2 target, float epsilon);
}


/// @}

} // namespace digi

#endif
