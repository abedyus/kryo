#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CameraShakeAction.h" // Ensure this header defines ECameraShakeAction
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "Engine/PostProcessVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraShakeBase.h"
#include "CustomCameraComponent.generated.h"

// Forward Declarations
class APostProcessVolume;
class UMaterialInstanceDynamic;
class UAudioComponent;
class UCameraShakeBase;

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
    ThirdPerson UMETA(DisplayName = "Third Person"),
    FirstPerson UMETA(DisplayName = "First Person"),
    FreeCamera UMETA(DisplayName = "Free Camera"),
    Cinematic UMETA(DisplayName = "Cinematic")
};

USTRUCT(BlueprintType)
struct FCameraShakeParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake|Parameters")
    float Intensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake|Parameters")
    float Frequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake|Parameters")
    bool bAdditive;

    FCameraShakeParams()
        : Intensity(1.0f), Frequency(10.0f), bAdditive(false) {
    }
};

USTRUCT(BlueprintType)
struct FCameraShakeMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    ECameraShakeAction Action;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    FCameraShakeParams ShakeParams;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CUSTOMCAMERA_API UCustomCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UCustomCameraComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // **Camera Modes**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Modes")
    ECameraMode CameraMode;

    UFUNCTION(BlueprintCallable, Category = "Camera|Modes")
    void SetCameraMode(ECameraMode NewMode);

    UFUNCTION(BlueprintCallable, Category = "Camera|Modes")
    ECameraMode GetCameraMode() const;

    UFUNCTION(BlueprintCallable, Category = "Camera|Modes")
    void SwitchToFirstPerson();

    UFUNCTION(BlueprintCallable, Category = "Camera|Modes")
    void SwitchToThirdPerson();

    UFUNCTION(BlueprintCallable, Category = "Camera|Modes")
    void SwitchToFreeCamera();

    UFUNCTION(BlueprintCallable, Category = "Camera|Modes")
    void SwitchToCinematic();

    // **Aiming and Running**
    UFUNCTION(BlueprintCallable, Category = "Camera|Aiming")
    void StartAiming();

    UFUNCTION(BlueprintCallable, Category = "Camera|Aiming")
    void StopAiming();

    UFUNCTION(BlueprintCallable, Category = "Camera|Running")
    void StartRunning();

    UFUNCTION(BlueprintCallable, Category = "Camera|Running")
    void StopRunning();

    // **Camera Controls**
    UFUNCTION(BlueprintCallable, Category = "Camera|Control")
    void Look(float AxisValueX, float AxisValueY);

    UFUNCTION(BlueprintCallable, Category = "Camera|Control")
    void Move(float AxisValueX, float AxisValueY);

    // **Input Binding Helpers**
    UFUNCTION(BlueprintCallable, Category = "Camera|Control")
    void LookX(float AxisValue);

    UFUNCTION(BlueprintCallable, Category = "Camera|Control")
    void LookY(float AxisValue);

    UFUNCTION(BlueprintCallable, Category = "Camera|Control")
    void MoveX(float AxisValue);

    UFUNCTION(BlueprintCallable, Category = "Camera|Control")
    void MoveY(float AxisValue);

    // **Effects**
    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void ApplyFadeEffect(float Duration);

    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void ApplyOcclusionEffect();

    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void RemoveOcclusionEffect();

    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void ApplyDepthOfField();

    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void ApplyMotionBlur();

    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void ApplyColorGrading();

    UFUNCTION(BlueprintCallable, Category = "Camera|Effects")
    void ApplyVignette();

    // **Camera Shakes**
    UFUNCTION(BlueprintCallable, Category = "Camera|Shake")
    void TriggerCameraShake(TSubclassOf<UCameraShakeBase> ShakeClassParam, float Scale = 1.f);

    UFUNCTION(BlueprintCallable, Category = "Camera|Shake")
    void TriggerCameraShakeAction(ECameraShakeAction Action);

    // **Warp Effect**
    UFUNCTION(BlueprintCallable, Category = "Camera|WarpEffect")
    void BeginWarpEffect();

    // **Transitions**
    UFUNCTION(BlueprintCallable, Category = "Camera|Transition")
    void SmoothTransitionToTarget(FVector TargetPosition, float TargetFOV, float Duration);

    UFUNCTION(BlueprintCallable, Category = "Camera|Transition")
    void InstantTransitionToTarget(FVector TargetPosition, float TargetFOV);

    // **AAA Features**
    // Dynamic Obstacle Detection
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Dynamic Obstacle Detection")
    bool bEnableDynamicObstacleDetection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Dynamic Obstacle Detection", meta = (EditCondition = "bEnableDynamicObstacleDetection"))
    float ObstacleDetectionInterval;

    // Dynamic Zoom
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Dynamic Zoom")
    bool bEnableDynamicZoom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Dynamic Zoom", meta = (EditCondition = "bEnableDynamicZoom"))
    float DynamicZoomThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Dynamic Zoom", meta = (EditCondition = "bEnableDynamicZoom"))
    float DynamicZoomSpeed;

    // Contextual Positioning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Contextual Positioning")
    bool bEnableContextualPositioning;

    // Intelligent Framing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Intelligent Framing")
    bool bEnableIntelligentFraming;

    // Adaptive Depth of Field
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Adaptive Depth of Field")
    bool bEnableAdaptiveDepthOfField;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Adaptive Depth of Field", meta = (EditCondition = "bEnableAdaptiveDepthOfField"))
    float FocusDistance;

    // Collision Prediction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Collision Prediction")
    bool bEnableCollisionPrediction;

    // Environmental Awareness
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Environmental Awareness")
    bool bEnableEnvironmentalAwareness;

    // Focus-based FOV
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Focus-based FOV")
    bool bEnableFocusBasedFOV;

    // Advanced Motion Blur
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Advanced Motion Blur")
    bool bEnableAdvancedMotionBlur;

    // Camera Inertia
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Inertia")
    bool bEnableCameraInertia;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|AAA Features|Inertia", meta = (EditCondition = "bEnableCameraInertia"))
    float CameraInertiaStrength;

    // **Additional Variables**

    // Head Bobbing Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|HeadBobbing")
    bool bEnableHeadBobbing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|HeadBobbing")
    float RunningBobMagnitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|HeadBobbing")
    float WalkingBobMagnitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|HeadBobbing")
    float BobFrequency;

    // Camera Sway Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Sway")
    bool bEnableCameraSway;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Sway")
    float SwayAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Sway")
    float SwaySpeed;

    // Rotation Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Rotation")
    float MaxYawRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Rotation")
    float MaxPitchRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Rotation")
    float RotationSpeed;

    // Terrain Tilt
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Tilt")
    bool bEnableTerrainTilt;

    // Camera Lag
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Lag")
    bool bEnableCameraLag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Lag")
    float CameraLagSpeed;

    // Dynamic FOV
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
    bool bEnableDynamicFOV;

    // Camera Height Constraints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Collision")
    float MinCameraHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Collision")
    float MaxCameraHeight;

    // **Additional Materials**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Effects")
    UMaterialInterface* FadeMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Effects")
    UMaterialInterface* OcclusionMaterial;

    // NEW: Transparent Material for dynamic transparency
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Occlusion")
    UMaterialInterface* TransparentMaterial;

    // **Camera Positions**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Positions")
    FVector FirstPersonPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Positions")
    FVector ThirdPersonPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Positions")
    FVector AimPositionFirstPerson;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Positions")
    FVector AimPositionThirdPerson;

    // **FOV Settings**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
    float DefaultFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
    float AimingFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
    float TransitionSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
    float ZoomedFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FOV")
    float SprintFOV;

    // **PostProcess Settings**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    UMaterialInterface* PostProcessMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    APostProcessVolume* PostProcessVolume;

    // Depth of Field
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    float DepthOfField;

    // Motion Blur Amount
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    float MotionBlurAmount;

    // Color Grading Intensity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    float ColorGradingIntensity;

    // Vignette Intensity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|PostProcess")
    float VignetteIntensity;

    // **Audio Settings for Warp Effect**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    USoundBase* WarpSoundEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    float WarpSoundVolumeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    float WarpSoundPitchMultiplier;

    // **Warp Effect Parameters**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    float WarpMaxFOVIncrease;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    float WarpDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    float WarpVignetteIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|WarpEffect")
    float WarpMotionBlurAmount;

    // **Camera Shakes**
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
    TArray<FCameraShakeMapping> CameraShakeMappings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Shake")
    TSubclassOf<UCameraShakeBase> CameraShakeClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Repositioning", meta = (EditCondition = "bEnableOverShoulderRepositioning"))
    float RepositioningSpeed;

    // **Timer Handles**
    FTimerHandle ObstacleDetectionTimerHandle;
    FTimerHandle FadeTimerHandle;
    FTimerHandle FadeInterpolationHandle;
    FTimerHandle WarpTimerHandle;

private:
    // Dynamic Object Transparency
    UPROPERTY(EditAnywhere, Category = "Camera|Occlusion")
    bool bEnableDynamicObjectTransparency;

    UPROPERTY(EditAnywhere, Category = "Camera|Occlusion", meta = (EditCondition = "bEnableDynamicObjectTransparency"))
    float OcclusionTransparencyStrength;

    UPROPERTY(EditAnywhere, Category = "Camera|Occlusion", meta = (EditCondition = "bEnableDynamicObjectTransparency"))
    float OcclusionCheckDistance;

    TMap<UPrimitiveComponent*, UMaterialInterface*> OccludedComponents;

    // Over-the-Shoulder Repositioning
    UPROPERTY(EditAnywhere, Category = "Camera|Repositioning")
    bool bEnableOverShoulderRepositioning;

    UPROPERTY(EditAnywhere, Category = "Camera|Repositioning", meta = (EditCondition = "bEnableOverShoulderRepositioning"))
    FVector OverShoulderOffset;

    // Recoil System
    UPROPERTY(EditAnywhere, Category = "Camera|Recoil")
    bool bEnableRecoil;

    UPROPERTY(EditAnywhere, Category = "Camera|Recoil", meta = (EditCondition = "bEnableRecoil"))
    float RecoilIntensity;

    UPROPERTY(EditAnywhere, Category = "Camera|Recoil", meta = (EditCondition = "bEnableRecoil"))
    float RecoilRecoverySpeed;

    FRotator RecoilRotation;

    // Environmental Awareness
    UPROPERTY(EditAnywhere, Category = "Camera|Environment", meta = (EditCondition = "bEnableEnvironmentalAwareness"))
    float EnvironmentCheckDistance;

    // Advanced Motion Blur
    UPROPERTY(EditAnywhere, Category = "Camera|MotionBlur", meta = (EditCondition = "bEnableAdvancedMotionBlur"))
    float MotionBlurIntensityMultiplier;

    // **Dynamic Material Instances**
    UPROPERTY()
    UMaterialInstanceDynamic* FadeMaterialInstance;

    UPROPERTY()
    UMaterialInstanceDynamic* OcclusionMaterialInstance;

    // **Audio Components**
    UPROPERTY()
    UAudioComponent* WarpAudioComponent;

    // **Camera State Flags**
    bool bIsRunning;
    bool bIsAiming;
    bool bIsFirstPersonMode;
    bool bIsTransitioning;

    // **Transition Variables**
    float TransitionElapsedTime;
    float TransitionTotalDuration;
    FVector TransitionStartPosition;
    FVector TransitionTargetPosition;
    float TransitionStartFOV;
    float TransitionTargetFOV;

    // **Current Rotation**
    FRotator CurrentRotation;

    // **Warp Effect Variables**
    float WarpElapsedTime;
    float WarpTotalDuration;
    float OriginalFOV;
    float WarpStartFOV;
    float WarpEndFOV;

    // **Additional Variables**
    float LastObstacleDetectionTime;

    // **AAA Features Functions**
    void PerformDynamicObstacleDetection();
    void PerformDynamicZoom();
    void UpdateContextualPositioning();
    void UpdateIntelligentFraming();
    void UpdateAdaptiveDepthOfField();
    void PredictAndPreventCollisions();
    void UpdateEnvironmentalAwareness();
    void UpdateFocusBasedFOV();
    void ApplyAdvancedMotionBlur();

    void UpdateWarpEffect(); // Warp effect update

    // Helper Functions
    void HandleDynamicObjectTransparency();
    void RestoreOccludedObjects();
    void ApplyRecoil(float DeltaTime);
    void ApplyCameraInertia(float DeltaTime);
    void AdjustCameraFOV(float DeltaTime, float DesiredFOV);
    void InitializeCamera();
    void UpdateCamera(float DeltaTime);
    void UpdateCameraRotation(float DeltaTime);
    void UpdateDynamicFOV(float DeltaTime);
    void UpdateTransition(float DeltaTime);
    void HandleCameraCollision();
    void UpdateBasedOnTerrain();
    void ClampRotation(FRotator& Rotation);
    void ApplyTransitionEffects();
    void ApplyHeadBobbing(float DeltaTime, bool bIsRunningLocal);
    void ApplyCameraSway(float DeltaTime);
    void SetupPostProcessMaterial();
    bool IsInFirstPersonMode() const;
    void SetCustomFOV(float NewFOV);
};
