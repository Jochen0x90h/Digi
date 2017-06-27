#include "IK.h"

// http://billbaxter.com/courses/290/html/img34.htm
// http://www.ryanjuckett.com/programming/animation/21-cyclic-coordinate-descent-in-2d
	
namespace digi {

namespace IK2D
{
	void runCCD(Joint* joints, int numJoints, float2 target, float epsilon)
	{
		float2 lastEffector = float2();
		int noMoveCount = 3;
		do
		{
			// forward kinematics
			float2 effector = float2();
			float rotate = 0.0f;
			for (int i = 0; i < numJoints; ++i)
			{
				Joint& joint = joints[i];
				
				joint.worldPosition = effector;
				rotate += joint.rotate;
				float c = cos(rotate);
				float s = sin(rotate);
				effector += vector2(c, s) * joint.translate.x + vector2(-s, c) * joint.translate.y;
			}
		
			// CCD step
			for (int i = numJoints - 1; i >= 0; --i)
			{
				Joint& joint = joints[i];

				float2 jointToEffector = effector - joint.worldPosition;
				float2 jointToTarget = target - joint.worldPosition;
				
				float jointToEffectorLength = length(jointToEffector);
				float jointToTargetLength = length(jointToTarget);
				
				float f = jointToEffectorLength * jointToTargetLength;
				if (f > epsilon)
				{
					float c = dot(jointToEffector, jointToTarget) / f;
					float s = dot(jointToEffector, perp(jointToTarget)) / f;
					
					float rotation = atan2(s, c);
				
					// rotate the end effector position
					effector = joint.worldPosition + vector2(c, s) * jointToEffector.x + vector2(-s, c) * jointToEffector.y;
					
					// rotate the current joint in local space
					joint.rotate += rotation;
				}
				
				// check for termination
			}
			
			// check if the end effector has moved
			if (lengthSquared(effector - lastEffector) < epsilon)
			{
				if (noMoveCount == 0)
					break;
				--noMoveCount;
			}
			lastEffector = effector;
		} while (true);
	}
}

} // namespace digi
