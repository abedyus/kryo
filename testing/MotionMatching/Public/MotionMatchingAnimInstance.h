#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MotionMatchingAnimInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class TESTING_API UMotionMatchingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Output from the ONNX model (e.g., driving blend spaces)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Motion Matching")
	float MotionMatchingOutput;

	// Character movement speed (for blending locomotion)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Motion Matching")
	float Speed;

	// Whether the character is currently jumping
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Motion Matching")
	bool bIsJumping;

	// Function to update the parameters from C++
	UFUNCTION(BlueprintCallable, Category = "Motion Matching")
	void UpdateMotionMatchingParameters(float InOutput, float InSpeed, bool bInIsJumping);
};
