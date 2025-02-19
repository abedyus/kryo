#include "MotionMatchingAnimInstance.h"

void UMotionMatchingAnimInstance::UpdateMotionMatchingParameters(float InOutput, float InSpeed, bool bInIsJumping)
{
	MotionMatchingOutput = InOutput;
	Speed = InSpeed;
	bIsJumping = bInIsJumping;
}
