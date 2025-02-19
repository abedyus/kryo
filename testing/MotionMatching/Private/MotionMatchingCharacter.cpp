#include "MotionMatchingCharacter.h"
#include "MotionMatchingAnimInstance.h"
// No need to include FOnnxInferenceSession.h here since it's already included in the header

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"

AMotionMatchingCharacter::AMotionMatchingCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set a default model path (settable in the Details panel)
    ONNXModelPath = FPaths::ProjectContentDir() / TEXT("Models/motion_matching_model.onnx");

    bIsJumping = false;
    CurrentSpeed = 0.f;
    VerticalVelocity = 0.f;
    MotionMatchingOutput = 0.f;
    InferenceInterval = 0.1f;
    TimeSinceLastInference = 0.f;
    CurrentAnimType = EAnimType::Idle;

    // Initialize OnnxSession in the constructor.
    OnnxSession = MakeUnique<FOnnxInferenceSession>();
}

void AMotionMatchingCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize the ONNX session.
    if (!OnnxSession->Initialize(ONNXModelPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize ONNX inference session."));
    }
}

void AMotionMatchingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMotionMatchingCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMotionMatchingCharacter::MoveRight);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMotionMatchingCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMotionMatchingCharacter::StopJump);
}

void AMotionMatchingCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentSpeed = GetVelocity().Size2D();
    VerticalVelocity = GetVelocity().Z;

    TimeSinceLastInference += DeltaTime;
    if (TimeSinceLastInference >= InferenceInterval)
    {
        TimeSinceLastInference = 0.f;
        TArray<float> FeatureVector;
        BuildFeatureVector(FeatureVector);

        float Prediction = 0.f;
        if (RunModelInference(FeatureVector, Prediction))
        {
            MotionMatchingOutput = Prediction;
            UpdateMotionMatching(Prediction);
        }
    }

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UMotionMatchingAnimInstance* AnimInstance = Cast<UMotionMatchingAnimInstance>(MeshComp->GetAnimInstance()))
        {
            AnimInstance->UpdateMotionMatchingParameters(MotionMatchingOutput, CurrentSpeed, bIsJumping);
        }
    }
}

void AMotionMatchingCharacter::BuildFeatureVector(TArray<float>& OutFeatures)
{
    const int32 NumFeatures = 73; // Adjust as needed.
    OutFeatures.Init(0.f, NumFeatures);

    // Example assignments:
    OutFeatures[0] = CurrentSpeed;
    OutFeatures[1] = VerticalVelocity;
    OutFeatures[2] = bIsJumping ? 1.f : 0.f;
    // Populate additional features as required...
}

bool AMotionMatchingCharacter::RunModelInference(const TArray<float>& FeatureVector, float& OutPrediction)
{
    TArray<float> OutputData;
    if (!OnnxSession->RunInference(FeatureVector, OutputData))
    {
        return false;
    }

    if (OutputData.Num() > 0)
    {
        OutPrediction = OutputData[0];
        return true;
    }
    return false;
}

void AMotionMatchingCharacter::UpdateMotionMatching(float Prediction)
{
    UE_LOG(LogTemp, Log, TEXT("Motion Matching Prediction: %f"), Prediction);

    // Example: Use the prediction to drive animation or movement...

    if (bIsJumping)
    {
        CurrentAnimType = EAnimType::Jump;
    }
    else if (CurrentSpeed > 10.f)
    {
        CurrentAnimType = EAnimType::Run;
    }
    else
    {
        CurrentAnimType = EAnimType::Idle;
    }
}

void AMotionMatchingCharacter::MoveForward(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMotionMatchingCharacter::MoveRight(float Value)
{
    if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
    {
        FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AMotionMatchingCharacter::StartJump()
{
    bIsJumping = true;
    Jump();
    CurrentAnimType = EAnimType::Jump;
}

void AMotionMatchingCharacter::StopJump()
{
    bIsJumping = false;
    StopJumping();
    CurrentAnimType = (CurrentSpeed > 10.f) ? EAnimType::Run : EAnimType::Idle;
}
