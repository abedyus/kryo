#include "CustomCameraComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShakeBase.h"
#include "Components/PostProcessComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h" // For APawn

UCustomCameraComponent::UCustomCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CameraMode = ECameraMode::ThirdPerson;
    bIsTransitioning = false;
    TransitionElapsedTime = 0.0f;
    TransitionTotalDuration = 0.0f;
    WarpElapsedTime = 0.0f;
    WarpTotalDuration = 0.0f;
    OriginalFOV = 90.0f; // Default value, can be overridden in the editor
    CurrentRotation = FRotator::ZeroRotator;

    // Initialize audio component
    //WarpAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("WarpAudioComponent"));
    //WarpAudioComponent->SetupAttachment(this);
    //WarpAudioComponent->bAutoActivate = false;

    // Initialize default camera positions
    FirstPersonPosition = FVector(0.0f, 0.0f, 0.0f);
    ThirdPersonPosition = FVector(-300.0f, 0.0f, 100.0f);
    AimPositionFirstPerson = FVector(0.0f, 0.0f, 0.0f);
    AimPositionThirdPerson = FVector(-300.0f, 0.0f, 100.0f);

    // Initialize FOV settings
    DefaultFOV = 90.0f;
    AimingFOV = 60.0f;
    TransitionSpeed = 5.0f;
    ZoomedFOV = 80.0f;
    SprintFOV = 70.0f;

    // Initialize rotation and sway settings
    MaxYawRotation = 90.0f;
    MaxPitchRotation = 45.0f;
    RotationSpeed = 2.0f;
    SwayAmount = 5.0f;
    SwaySpeed = 2.0f;

    // Initialize bobbing settings
    RunningBobMagnitude = 10.0f;
    WalkingBobMagnitude = 5.0f;
    BobFrequency = 10.0f;

    // Initialize post-process settings
    DepthOfField = 1000.0f;
    MotionBlurAmount = 1.0f;
    ColorGradingIntensity = 1.0f;
    VignetteIntensity = 0.5f;

    // Initialize feature toggles
    bEnableHeadBobbing = true;
    bEnableCameraSway = true;
    bEnableTerrainTilt = false;
    bEnableCameraLag = true;
    CameraLagSpeed = 5.0f;
    bEnableDynamicFOV = true;
    DynamicZoomThreshold = 500.0f;
    DynamicZoomSpeed = 2.0f;
    bEnableContextualPositioning = false;
    bEnableIntelligentFraming = false;
    bEnableAdaptiveDepthOfField = false;
    FocusDistance = 1000.0f;
    bEnableCollisionPrediction = false;
    bEnableEnvironmentalAwareness = false;
    bEnableFocusBasedFOV = false;
    bEnableAdvancedMotionBlur = false;
    bEnableCameraInertia = false;
    CameraInertiaStrength = 5.0f;

    // Initialize collision defaults
    MinCameraHeight = 50.0f;
    MaxCameraHeight = 300.0f;

    // Initialize dynamic material instances
    FadeMaterialInstance = nullptr;
    OcclusionMaterialInstance = nullptr;

    // Initialize camera state flags
    bIsRunning = false;
    bIsAiming = false;
    bIsFirstPersonMode = false;

    // Initialize warp effect variables
    WarpMaxFOVIncrease = 30.0f;
    WarpVignetteIntensity = 1.0f;
    WarpMotionBlurAmount = 2.0f;
    WarpDuration = 2.0f;
    WarpElapsedTime = 0.0f;
    WarpTotalDuration = WarpDuration;
    WarpStartFOV = DefaultFOV;
    WarpEndFOV = DefaultFOV + WarpMaxFOVIncrease;

    // Initialize transition variables
    bIsTransitioning = false;
    TransitionElapsedTime = 0.0f;
    TransitionTotalDuration = 0.0f;
    TransitionStartPosition = GetRelativeLocation();
    TransitionTargetPosition = ThirdPersonPosition;
    TransitionStartFOV = FieldOfView;
    TransitionTargetFOV = DefaultFOV;

    // Initialize LastObstacleDetectionTime
    LastObstacleDetectionTime = 0.0f;
}

void UCustomCameraComponent::BeginPlay()
{
    Super::BeginPlay();
    // Set view target with blend
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            PC->SetViewTargetWithBlend(OwnerPawn, 1.0f, EViewTargetBlendFunction::VTBlend_Cubic);
        }
    }
    InitializeCamera();
    SetupPostProcessMaterial();

    if (bEnableDynamicObstacleDetection)
    {
        GetWorld()->GetTimerManager().SetTimer(ObstacleDetectionTimerHandle, this, &UCustomCameraComponent::PerformDynamicObstacleDetection, ObstacleDetectionInterval, true);
    }
}

void UCustomCameraComponent::InitializeCamera()
{
    SetFieldOfView(DefaultFOV);
    SetRelativeLocation(ThirdPersonPosition);
    CurrentRotation = GetRelativeRotation();
}

void UCustomCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateCamera(DeltaTime);
    UpdateCameraRotation(DeltaTime);
    UpdateDynamicFOV(DeltaTime);
    UpdateTransition(DeltaTime);
    HandleCameraCollision();
    UpdateBasedOnTerrain();

    if (bEnableDynamicZoom)
    {
        PerformDynamicZoom();
    }

    if (bEnableContextualPositioning)
    {
        UpdateContextualPositioning();
    }

    if (bEnableIntelligentFraming)
    {
        UpdateIntelligentFraming();
    }

    if (bEnableAdaptiveDepthOfField)
    {
        UpdateAdaptiveDepthOfField();
    }

    if (bEnableCollisionPrediction)
    {
        PredictAndPreventCollisions();
    }

    if (bEnableEnvironmentalAwareness)
    {
        UpdateEnvironmentalAwareness();
    }

    if (bEnableFocusBasedFOV)
    {
        UpdateFocusBasedFOV();
    }

    if (bEnableAdvancedMotionBlur)
    {
        ApplyAdvancedMotionBlur();
    }

    if (bEnableCameraInertia)
    {
        ApplyCameraInertia(DeltaTime);
    }

    if (bEnableHeadBobbing)
    {
        ApplyHeadBobbing(DeltaTime, bIsRunning);
    }

    if (bEnableCameraSway)
    {
        ApplyCameraSway(DeltaTime);
    }
    if (bEnableDynamicObjectTransparency)
    {
        HandleDynamicObjectTransparency();
    }

    if (bEnableOverShoulderRepositioning)
    {
        FVector TargetPosition = bIsAiming ? OverShoulderOffset : FVector::ZeroVector;
        SetRelativeLocation(FMath::VInterpTo(GetRelativeLocation(), TargetPosition, DeltaTime, RepositioningSpeed));
    }

    if (bEnableRecoil)
    {
        ApplyRecoil(DeltaTime);
    }

    if (bEnableEnvironmentalAwareness)
    {
        UpdateEnvironmentalAwareness();
    }

    if (bEnableFocusBasedFOV)
    {
        UpdateFocusBasedFOV();
    }

    if (bEnableAdvancedMotionBlur)
    {
        ApplyAdvancedMotionBlur();
    }

    if (bEnableCameraInertia)
    {
        ApplyCameraInertia(DeltaTime);
    }
}

void UCustomCameraComponent::SetCameraMode(ECameraMode NewMode)
{
    if (CameraMode != NewMode)
    {
        CameraMode = NewMode;
        switch (CameraMode)
        {
        case ECameraMode::FirstPerson:
            SwitchToFirstPerson();
            break;
        case ECameraMode::ThirdPerson:
            SwitchToThirdPerson();
            break;
        case ECameraMode::FreeCamera:
            SwitchToFreeCamera();
            break;
        case ECameraMode::Cinematic:
            SwitchToCinematic();
            break;
        }
    }
}

ECameraMode UCustomCameraComponent::GetCameraMode() const
{
    return CameraMode;
}

void UCustomCameraComponent::SwitchToFirstPerson()
{
    SmoothTransitionToTarget(FirstPersonPosition, DefaultFOV, TransitionSpeed);
    bIsFirstPersonMode = true;
}

void UCustomCameraComponent::SwitchToThirdPerson()
{
    SmoothTransitionToTarget(ThirdPersonPosition, DefaultFOV, TransitionSpeed);
    bIsFirstPersonMode = false;
}

void UCustomCameraComponent::SwitchToFreeCamera()
{
    UE_LOG(LogTemp, Log, TEXT("Switched to Free Camera Mode"));
}

void UCustomCameraComponent::SwitchToCinematic()
{
    UE_LOG(LogTemp, Log, TEXT("Switched to Cinematic Mode"));
}

void UCustomCameraComponent::StartAiming()
{
    bIsAiming = true;
    AdjustCameraFOV(GetWorld()->GetDeltaSeconds(), AimingFOV);
}

void UCustomCameraComponent::StopAiming()
{
    bIsAiming = false;
    AdjustCameraFOV(GetWorld()->GetDeltaSeconds(), DefaultFOV);
}

void UCustomCameraComponent::StartRunning()
{
    bIsRunning = true;
}

void UCustomCameraComponent::StopRunning()
{
    bIsRunning = false;
}

void UCustomCameraComponent::Look(float AxisValueX, float AxisValueY)
{
    CurrentRotation.Yaw += AxisValueX * RotationSpeed;
    CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch + AxisValueY * RotationSpeed, -MaxPitchRotation, MaxPitchRotation);
    ClampRotation(CurrentRotation);
    SetRelativeRotation(CurrentRotation);
}

void UCustomCameraComponent::Move(float AxisValueX, float AxisValueY)
{
    if (CameraMode == ECameraMode::FreeCamera)
    {
        FVector Forward = GetForwardVector();
        FVector Right = GetRightVector();
        FVector Movement = (Forward * AxisValueY + Right * AxisValueX) * CameraLagSpeed * GetWorld()->GetDeltaSeconds();
        AddRelativeLocation(Movement, true);
    }
    else
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (OwnerPawn && OwnerPawn->IsLocallyControlled())
        {
            OwnerPawn->AddMovementInput(OwnerPawn->GetActorForwardVector(), AxisValueY);
            OwnerPawn->AddMovementInput(OwnerPawn->GetActorRightVector(), AxisValueX);
        }
    }
}

void UCustomCameraComponent::LookX(float AxisValue)
{
    Look(AxisValue, 0.0f);
}

void UCustomCameraComponent::LookY(float AxisValue)
{
    Look(0.0f, AxisValue);
}

void UCustomCameraComponent::MoveX(float AxisValue)
{
    Move(AxisValue, 0.0f);
}

void UCustomCameraComponent::MoveY(float AxisValue)
{
    Move(0.0f, AxisValue);
}

void UCustomCameraComponent::ApplyFadeEffect(float Duration)
{
    if (PostProcessVolume && FadeMaterial)
    {
        if (FadeMaterialInstance)
        {
            PostProcessVolume->Settings.RemoveBlendable(FadeMaterialInstance);
            FadeMaterialInstance->ConditionalBeginDestroy();
            FadeMaterialInstance = nullptr;
        }

        FadeMaterialInstance = UMaterialInstanceDynamic::Create(FadeMaterial, this);
        FadeMaterialInstance->SetScalarParameterValue(FName("FadeAmount"), 0.0f);
        PostProcessVolume->Settings.AddBlendable(FadeMaterialInstance, 1.0f);

        UPostProcessComponent* PPComponent = Cast<UPostProcessComponent>(PostProcessVolume->GetComponentByClass(UPostProcessComponent::StaticClass()));
        if (PPComponent)
        {
            PPComponent->AddOrUpdateBlendable(FadeMaterialInstance);
        }

        FTimerDelegate FadeDelegate;
        FadeDelegate.BindLambda([DynMaterial = FadeMaterialInstance, Duration, this]()
            {
                if (!DynMaterial) return;
                float DeltaTime = GetWorld()->GetDeltaSeconds();
                float CurrentFade = 0.0f;
                DynMaterial->GetScalarParameterValue(FName("FadeAmount"), CurrentFade);
                float Alpha = FMath::Clamp(CurrentFade + (DeltaTime / Duration), 0.0f, 1.0f);
                DynMaterial->SetScalarParameterValue(FName("FadeAmount"), Alpha);
                if (Alpha >= 1.0f)
                {
                    GetWorld()->GetTimerManager().ClearTimer(FadeInterpolationHandle);
                }
            });

        GetWorld()->GetTimerManager().SetTimer(FadeInterpolationHandle, FadeDelegate, 0.01f, true);
    }
}

void UCustomCameraComponent::ApplyOcclusionEffect()
{
    if (PostProcessVolume && OcclusionMaterial)
    {
        if (OcclusionMaterialInstance)
        {
            UPostProcessComponent* PPComponent = Cast<UPostProcessComponent>(PostProcessVolume->GetComponentByClass(UPostProcessComponent::StaticClass()));
            if (PPComponent)
            {
                PPComponent->Settings.RemoveBlendable(OcclusionMaterialInstance);
            }
            OcclusionMaterialInstance->ConditionalBeginDestroy();
            OcclusionMaterialInstance = nullptr;
        }

        OcclusionMaterialInstance = UMaterialInstanceDynamic::Create(OcclusionMaterial, this);
        OcclusionMaterialInstance->SetScalarParameterValue(FName("OcclusionIntensity"), 1.0f);

        UPostProcessComponent* PPComponent = Cast<UPostProcessComponent>(PostProcessVolume->GetComponentByClass(UPostProcessComponent::StaticClass()));
        if (PPComponent)
        {
            PPComponent->AddOrUpdateBlendable(OcclusionMaterialInstance);
        }
    }
}

void UCustomCameraComponent::RemoveOcclusionEffect()
{
    if (PostProcessVolume && OcclusionMaterialInstance)
    {
        PostProcessVolume->Settings.RemoveBlendable(OcclusionMaterialInstance);
        OcclusionMaterialInstance->ConditionalBeginDestroy();
        OcclusionMaterialInstance = nullptr;
    }
}

void UCustomCameraComponent::ApplyDepthOfField()
{
    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.DepthOfFieldFocalDistance = DepthOfField;
        PostProcessVolume->Settings.DepthOfFieldFocalRegion = 10.0f;
        PostProcessVolume->Settings.DepthOfFieldFstop = FMath::Clamp(FocusDistance / 1000.0f, 1.0f, 16.0f);
    }
}

void UCustomCameraComponent::ApplyMotionBlur()
{
    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.MotionBlurAmount = MotionBlurAmount;
    }
}

void UCustomCameraComponent::ApplyColorGrading()
{
    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.ColorGradingIntensity = ColorGradingIntensity;
    }
}

void UCustomCameraComponent::ApplyVignette()
{
    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.VignetteIntensity = VignetteIntensity;
    }
}

void UCustomCameraComponent::TriggerCameraShake(TSubclassOf<UCameraShakeBase> ShakeClassParam, float Scale)
{
    if (ShakeClassParam)
    {
        UGameplayStatics::PlayWorldCameraShake(GetWorld(), ShakeClassParam, GetOwner()->GetActorLocation(), 0.0f, 0.0f, Scale);
    }
}

void UCustomCameraComponent::TriggerCameraShakeAction(ECameraShakeAction Action)
{
    for (const FCameraShakeMapping& Mapping : CameraShakeMappings)
    {
        if (Mapping.Action == Action)
        {
            TriggerCameraShake(CameraShakeClass, Mapping.ShakeParams.Intensity);
            break;
        }
    }
}

void UCustomCameraComponent::SmoothTransitionToTarget(FVector TargetPosition, float TargetFOV, float Duration)
{
    bIsTransitioning = true;
    TransitionElapsedTime = 0.0f;
    TransitionTotalDuration = Duration;
    TransitionStartPosition = GetRelativeLocation();
    TransitionTargetPosition = TargetPosition;
    TransitionStartFOV = FieldOfView;
    TransitionTargetFOV = TargetFOV;
}

void UCustomCameraComponent::InstantTransitionToTarget(FVector TargetPosition, float TargetFOV)
{
    SetRelativeLocation(TargetPosition);
    SetFieldOfView(TargetFOV);
    bIsTransitioning = false;
}

void UCustomCameraComponent::BeginWarpEffect()
{
    if (WarpSoundEffect)
    {
        // Play the warp sound at the camera's location without needing an attached audio component.
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), WarpSoundEffect, GetComponentLocation(), WarpSoundVolumeMultiplier, WarpSoundPitchMultiplier);
    }

    OriginalFOV = FieldOfView;
    WarpStartFOV = OriginalFOV;
    WarpEndFOV = OriginalFOV + WarpMaxFOVIncrease;
    WarpElapsedTime = 0.0f;
    WarpTotalDuration = WarpDuration;

    GetWorld()->GetTimerManager().SetTimer(WarpTimerHandle, this, &UCustomCameraComponent::UpdateWarpEffect, 0.01f, true);
}

void UCustomCameraComponent::PerformDynamicZoom()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    float Speed = OwnerPawn->GetVelocity().Size();
    if (Speed > DynamicZoomThreshold)
    {
        float TargetFOV = FMath::FInterpTo(FieldOfView, ZoomedFOV, GetWorld()->GetDeltaSeconds(), DynamicZoomSpeed);
        SetFieldOfView(TargetFOV);
    }
    else
    {
        float TargetFOV = FMath::FInterpTo(FieldOfView, DefaultFOV, GetWorld()->GetDeltaSeconds(), DynamicZoomSpeed);
        SetFieldOfView(TargetFOV);
    }
}

void UCustomCameraComponent::UpdateContextualPositioning()
{
    // Implementation based on game context
}

void UCustomCameraComponent::UpdateIntelligentFraming()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        FVector PlayerLocation = OwnerPawn->GetActorLocation();
        FVector CameraLocation = GetComponentLocation();
        FVector Direction = (PlayerLocation - CameraLocation).GetSafeNormal();
        FRotator TargetRotation = Direction.Rotation();
        SetRelativeRotation(FMath::RInterpTo(GetRelativeRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed));
    }
}

void UCustomCameraComponent::UpdateAdaptiveDepthOfField()
{
    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.DepthOfFieldFocalDistance = FocusDistance;
        PostProcessVolume->Settings.DepthOfFieldFocalRegion = 10.0f;
        PostProcessVolume->Settings.DepthOfFieldFstop = FMath::Clamp(FocusDistance / 1000.0f, 1.0f, 16.0f);
    }
}

void UCustomCameraComponent::PredictAndPreventCollisions()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    FVector FutureLocation = OwnerPawn->GetActorLocation() + OwnerPawn->GetVelocity() * 0.5f;
    FVector Start = GetComponentLocation();
    FVector End = FutureLocation;
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, Params))
    {
        FVector PushBack = (Start - HitResult.ImpactPoint).GetSafeNormal() * 50.0f;
        SetRelativeLocation(GetRelativeLocation() + PushBack);
    }

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
}

void UCustomCameraComponent::UpdateFocusBasedFOV()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    FVector CameraLocation = GetComponentLocation();
    FVector FocusPoint = CameraLocation + GetForwardVector() * FocusDistance;
    float Distance = FVector::Dist(CameraLocation, FocusPoint);
    float DesiredFOV = FMath::Clamp(Distance / 10.0f, 60.0f, 90.0f);
    AdjustCameraFOV(GetWorld()->GetDeltaSeconds(), DesiredFOV);
}

void UCustomCameraComponent::ApplyAdvancedMotionBlur()
{
    if (PostProcessVolume)
    {
        // Use owner pawn velocity to determine intensity
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        float VelocitySize = OwnerPawn ? OwnerPawn->GetVelocity().Size() : 0.0f;
        float MotionBlurIntensity = FMath::Clamp(VelocitySize / 1000.0f, 0.0f, 1.0f) * MotionBlurIntensityMultiplier;
        PostProcessVolume->Settings.MotionBlurAmount = MotionBlurIntensity;
    }
}

void UCustomCameraComponent::UpdateWarpEffect()
{
    WarpElapsedTime += GetWorld()->GetDeltaSeconds();
    float Alpha = FMath::Clamp(WarpElapsedTime / WarpTotalDuration, 0.0f, 1.0f);
    float NewFOV = FMath::Lerp(WarpStartFOV, WarpEndFOV, Alpha);
    SetFieldOfView(NewFOV);

    if (PostProcessVolume)
    {
        PostProcessVolume->Settings.VignetteIntensity = FMath::Lerp(0.0f, WarpVignetteIntensity, Alpha);
        PostProcessVolume->Settings.MotionBlurAmount = FMath::Lerp(0.0f, WarpMotionBlurAmount, Alpha);
    }

    DrawDebugString(GetWorld(), GetComponentLocation(), FString::Printf(TEXT("Warp Effect: %.2f%%"), Alpha * 100.0f), nullptr, FColor::Yellow, 0.01f);

    if (WarpElapsedTime >= WarpTotalDuration)
    {
        WarpElapsedTime = 0.0f;
        WarpStartFOV = WarpEndFOV;
        WarpEndFOV = OriginalFOV;

        if (PostProcessVolume)
        {
            PostProcessVolume->Settings.VignetteIntensity = 0.0f;
            PostProcessVolume->Settings.MotionBlurAmount = 0.0f;
        }

        if (WarpAudioComponent->IsPlaying())
        {
            WarpAudioComponent->Stop();
        }

        GetWorld()->GetTimerManager().ClearTimer(WarpTimerHandle);
    }
}

void UCustomCameraComponent::PerformDynamicObstacleDetection()
{
    FVector Start = GetComponentLocation();
    FVector ForwardVector = GetForwardVector();
    FVector End = Start + ForwardVector * 100.0f;
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, Params))
    {
        UE_LOG(LogTemp, Warning, TEXT("Obstacle Detected: %s"), HitResult.GetActor() ? *HitResult.GetActor()->GetName() : TEXT("None"));

        float PushDistance = (HitResult.ImpactPoint - Start).Size() - 50.0f;
        if (PushDistance < 0.0f)
        {
            PushDistance = 0.0f;
        }
        FVector NewLocation = Start + ForwardVector * PushDistance;
        SetRelativeLocation(NewLocation);
    }

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
}

void UCustomCameraComponent::UpdateCamera(float DeltaTime)
{
    if (bIsTransitioning)
    {
        UpdateTransition(DeltaTime);
    }

    if (bEnableDynamicFOV)
    {
        UpdateDynamicFOV(DeltaTime);
    }

    if (bEnableCollisionPrediction)
    {
        PredictAndPreventCollisions();
    }
}

void UCustomCameraComponent::UpdateCameraRotation(float DeltaTime)
{
    // Rotation is handled in the Look function
}

void UCustomCameraComponent::UpdateDynamicFOV(float DeltaTime)
{
    if (bEnableDynamicFOV)
    {
        float DesiredFOV = DefaultFOV;
        FVector Velocity = FVector::ZeroVector;
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (OwnerPawn)
        {
            Velocity = OwnerPawn->GetVelocity();
        }

        float Speed = Velocity.Size();

        if (bIsAiming)
        {
            DesiredFOV = AimingFOV;
        }
        else if (bIsRunning && Speed > DynamicZoomThreshold)
        {
            DesiredFOV = SprintFOV;
        }
        else if (Speed > DynamicZoomThreshold)
        {
            DesiredFOV = ZoomedFOV;
        }

        AdjustCameraFOV(DeltaTime, DesiredFOV);
    }
}

void UCustomCameraComponent::UpdateTransition(float DeltaTime)
{
    if (bIsTransitioning)
    {
        TransitionElapsedTime += DeltaTime;
        float Alpha = FMath::Clamp(TransitionElapsedTime / TransitionTotalDuration, 0.0f, 1.0f);
        FVector NewLocation = FMath::Lerp(TransitionStartPosition, TransitionTargetPosition, Alpha);
        float NewFOV = FMath::Lerp(TransitionStartFOV, TransitionTargetFOV, Alpha);
        SetRelativeLocation(NewLocation);
        SetFieldOfView(NewFOV);

        if (Alpha >= 1.0f)
        {
            bIsTransitioning = false;
        }
    }
}

void UCustomCameraComponent::HandleCameraCollision()
{
    FVector Start = GetComponentLocation();
    FVector ForwardVector = GetForwardVector();
    FVector End = Start + ForwardVector * 100.0f;
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Camera, Params);
    if (bHit)
    {
        FVector PushBack = (Start - HitResult.ImpactPoint).GetSafeNormal() * 50.0f;
        SetRelativeLocation(GetRelativeLocation() + PushBack);
    }
}

void UCustomCameraComponent::UpdateBasedOnTerrain()
{
    if (bEnableTerrainTilt)
    {
        FVector Start = GetComponentLocation();
        FVector End = Start - FVector(0.0f, 0.0f, 100.0f);
        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(GetOwner());

        if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
        {
            FVector TerrainNormal = HitResult.ImpactNormal;
            FRotator TerrainRotation = TerrainNormal.Rotation();
            FRotator NewRotation = FRotator(0.0f, TerrainRotation.Yaw, TerrainRotation.Roll);
            SetRelativeRotation(FMath::RInterpTo(GetRelativeRotation(), NewRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed));
        }
    }
}

void UCustomCameraComponent::ClampRotation(FRotator& Rotation)
{
    Rotation.Yaw = FMath::Clamp(Rotation.Yaw, -MaxYawRotation, MaxYawRotation);
    Rotation.Pitch = FMath::Clamp(Rotation.Pitch, -MaxPitchRotation, MaxPitchRotation);
}

void UCustomCameraComponent::ApplyTransitionEffects()
{
    // Implementation for transition effects such as screen fade or blur
}

void UCustomCameraComponent::ApplyHeadBobbing(float DeltaTime, bool bIsRunningLocal)
{
    float BobMagnitude = bIsRunningLocal ? RunningBobMagnitude : WalkingBobMagnitude;
    float BobSpeed = BobFrequency * (bIsRunningLocal ? 1.5f : 1.0f);
    float BobOffset = FMath::Sin(GetWorld()->GetTimeSeconds() * BobSpeed) * BobMagnitude;
    AddRelativeLocation(FVector(0.0f, 0.0f, BobOffset * DeltaTime));
}

void UCustomCameraComponent::ApplyCameraSway(float DeltaTime)
{
    FVector SwayOffset = FVector(
        FMath::Sin(GetWorld()->GetTimeSeconds() * SwaySpeed) * SwayAmount,
        FMath::Cos(GetWorld()->GetTimeSeconds() * SwaySpeed) * SwayAmount,
        0.0f
    );
    AddRelativeLocation(SwayOffset * DeltaTime);
}

void UCustomCameraComponent::SetupPostProcessMaterial()
{
    if (PostProcessVolume && PostProcessMaterial)
    {
        UPostProcessComponent* PPComponent = Cast<UPostProcessComponent>(PostProcessVolume->GetComponentByClass(UPostProcessComponent::StaticClass()));
        if (PPComponent)
        {
            UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(PostProcessMaterial, this);
            PPComponent->AddOrUpdateBlendable(DynMaterial);
        }
    }
}

bool UCustomCameraComponent::IsInFirstPersonMode() const
{
    return bIsFirstPersonMode;
}

void UCustomCameraComponent::SetCustomFOV(float NewFOV)
{
    SetFieldOfView(NewFOV);
}

void UCustomCameraComponent::AdjustCameraFOV(float DeltaTime, float DesiredFOV)
{
    float CurrentFOV = FieldOfView;
    float NewFOV = FMath::FInterpTo(CurrentFOV, DesiredFOV, DeltaTime, 5.0f);
    SetFieldOfView(NewFOV);
}

void UCustomCameraComponent::HandleDynamicObjectTransparency()
{
    FVector Start = GetComponentLocation();
    FVector End = Start + GetForwardVector() * OcclusionCheckDistance;
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        UPrimitiveComponent* HitComponent = HitResult.GetComponent();
        if (HitComponent && !OccludedComponents.Contains(HitComponent))
        {
            UMaterialInterface* OriginalMaterial = HitComponent->GetMaterial(0);
            OccludedComponents.Add(HitComponent, OriginalMaterial);

            UMaterialInstanceDynamic* TransparentMaterialInstance = UMaterialInstanceDynamic::Create(TransparentMaterial, this);
            TransparentMaterialInstance->SetScalarParameterValue("Transparency", OcclusionTransparencyStrength);
            HitComponent->SetMaterial(0, TransparentMaterialInstance);
        }
    }
    else
    {
        RestoreOccludedObjects();
    }
}

void UCustomCameraComponent::RestoreOccludedObjects()
{
    for (auto& Elem : OccludedComponents)
    {
        if (Elem.Key && Elem.Value)
        {
            Elem.Key->SetMaterial(0, Elem.Value);
        }
    }
    OccludedComponents.Empty();
}

void UCustomCameraComponent::ApplyRecoil(float DeltaTime)
{
    if (RecoilRotation != FRotator::ZeroRotator)
    {
        RecoilRotation = FMath::RInterpTo(RecoilRotation, FRotator::ZeroRotator, DeltaTime, RecoilRecoverySpeed);
        AddLocalRotation(RecoilRotation);
    }
}

void UCustomCameraComponent::UpdateEnvironmentalAwareness()
{
    FVector Start = GetComponentLocation();
    FVector End = Start + GetForwardVector() * EnvironmentCheckDistance;
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        FVector NewLocation = HitResult.Location - GetForwardVector() * 50.0f;
        SetRelativeLocation(FMath::VInterpTo(GetRelativeLocation(), NewLocation, GetWorld()->GetDeltaSeconds(), 5.0f));
    }
}

void UCustomCameraComponent::ApplyCameraInertia(float DeltaTime)
{
    // Smoothly interpolate the current rotation to the stored target rotation (CurrentRotation)
    FRotator CurrentRelRotation = GetRelativeRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRelRotation, CurrentRotation, DeltaTime, CameraInertiaStrength);
    SetRelativeRotation(NewRotation);
}
