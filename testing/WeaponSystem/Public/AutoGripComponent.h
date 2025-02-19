#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// Forward declare AWeaponBase to avoid circular dependency in this header.
class AWeaponBase;

#include "AutoGripComponent.generated.h"

USTRUCT(BlueprintType)
struct FFingerBoneChain
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Finger")
    TArray<FName> BoneNames;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WEAPONSYSTEM_API UAutoGripComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAutoGripComponent();

protected:
    virtual void BeginPlay() override;

public:
    /** Attaches the specified weapon to this component's owner and updates grip transforms */
    UFUNCTION(BlueprintCallable, Category = "AutoGrip")
    void AttachWeapon(AWeaponBase* Weapon);

    /** (Placeholder) Adjusts the grip. Currently simply disables ticking. */
    UFUNCTION(BlueprintCallable, Category = "AutoGrip")
    void AdjustGrip();

    /** Updates the grip transforms based on the currently attached weapon */
    UFUNCTION(BlueprintCallable, Category = "AutoGrip")
    void UpdateGripTransforms();

    // Optional offsets for additional flexibility
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    FVector LeftHandOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    FRotator LeftHandRotationOffset = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    FVector RightHandOffset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    FRotator RightHandRotationOffset = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    FName LeftHandBone = "hand_l";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    FName RightHandBone = "hand_r";

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    TArray<FFingerBoneChain> LeftHandFingerBoneChains;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    TArray<FFingerBoneChain> RightHandFingerBoneChains;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoGrip|Settings")
    bool bAutoAdjustFingers = true;

private:
    // The currently attached weapon.
    AWeaponBase* CurrentWeapon;

    /** Updates finger IK for the currently attached weapon */
    void AdjustFingerIK();

    /** Moves a specified hand bone toward a target location and rotation (e.g. for IK visualization) */
    void MoveHandToSocket(USkeletalMeshComponent* PlayerMesh, const FName& HandSocket, const FVector& TargetLocation, const FRotator& TargetRotation);

    /** Iterates over the provided finger bone chains and applies IK, drawing debug lines along the bone chain */
    void ApplyFingerIK(USkeletalMeshComponent* PlayerMesh, const TArray<FFingerBoneChain>& FingerBoneChains, const FVector& TargetLocation);
};
