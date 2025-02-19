#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FOnnxInferenceSession.h" // Include our full wrapper header
#include "MotionMatchingCharacter.generated.h"

/** Available animation types for motion matching. */
UENUM(BlueprintType)
enum class EAnimType : uint8
{
    None UMETA(DisplayName = "None"),
    Idle UMETA(DisplayName = "Idle"),
    Run  UMETA(DisplayName = "Run"),
    Jump UMETA(DisplayName = "Jump")
};

/** Mapping from an input action to an animation type. */
USTRUCT(BlueprintType)
struct FInputAnimMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
    FName InputAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
    EAnimType AnimType;
};

UCLASS()
class TESTING_API AMotionMatchingCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMotionMatchingCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    virtual void Tick(float DeltaTime) override;

    /** Path to the ONNX model (settable in the Details panel) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
    FString ONNXModelPath;

    /** Output from the ONNX model (can drive animation blend spaces) */
    UPROPERTY(BlueprintReadOnly, Category = "Motion Matching")
    float MotionMatchingOutput;

    /** Mapping of input actions to animation types */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching")
    TArray<FInputAnimMapping> InputAnimMappings;

    /** The current animation type, based on input and model inference */
    UPROPERTY(BlueprintReadOnly, Category = "Motion Matching")
    EAnimType CurrentAnimType;

private:
    // Our ONNX inference session wrapper.
    TUniquePtr<FOnnxInferenceSession> OnnxSession;

    // Internal state for building the feature vector.
    bool bIsJumping;
    float CurrentSpeed;
    float VerticalVelocity;

    // Inference timing.
    float InferenceInterval;
    float TimeSinceLastInference;

    // Helper functions.
    void BuildFeatureVector(TArray<float>& OutFeatures);
    bool RunModelInference(const TArray<float>& FeatureVector, float& OutPrediction);
    void UpdateMotionMatching(float Prediction);

    // Input handlers.
    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartJump();
    void StopJump();
};
