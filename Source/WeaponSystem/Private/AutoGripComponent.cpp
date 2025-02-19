#include "AutoGripComponent.h"
#include "WeaponBase.h" // Included here to access AWeaponBase and its GetGripTransform() function.
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UAutoGripComponent::UAutoGripComponent()
{
    // (Optionally, you might want ticking enabled if you plan to update IK each frame.)
    PrimaryComponentTick.bCanEverTick = true;
}

void UAutoGripComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAutoGripComponent::AdjustGrip()
{
    // Minimal implementation – here we disable ticking.
    // (You can later add code to adjust the grip dynamically.)
    PrimaryComponentTick.bCanEverTick = false;
}

void UAutoGripComponent::AttachWeapon(AWeaponBase* Weapon)
{
    if (!Weapon)
    {
        UE_LOG(LogTemp, Error, TEXT("AttachWeapon: Weapon is null or not a valid WeaponBase!"));
        return;
    }

    CurrentWeapon = Weapon;
    UpdateGripTransforms();
}

void UAutoGripComponent::UpdateGripTransforms()
{
    if (!CurrentWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateGripTransforms: No weapon attached!"));
        return;
    }

    USkeletalMeshComponent* PlayerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!PlayerMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateGripTransforms: Could not find SkeletalMeshComponent on owner!"));
        return;
    }

    // Fetch weapon grip transforms from the WeaponBase.
    FTransform LeftGripTransform = CurrentWeapon->GetGripTransform(FName("LeftHandGripSocket"));
    FTransform RightGripTransform = CurrentWeapon->GetGripTransform(FName("RightHandGripSocket"));

    // Apply the hand offsets defined in AutoGripComponent.
    LeftGripTransform = LeftGripTransform * FTransform(LeftHandRotationOffset, LeftHandOffset);
    RightGripTransform = RightGripTransform * FTransform(RightHandRotationOffset, RightHandOffset);

    FVector LeftGripLocation = LeftGripTransform.GetLocation();
    FVector RightGripLocation = RightGripTransform.GetLocation();
    FRotator LeftGripRotation = LeftGripTransform.GetRotation().Rotator();
    FRotator RightGripRotation = RightGripTransform.GetRotation().Rotator();

    // Move hands to the calculated positions.
    MoveHandToSocket(PlayerMesh, LeftHandBone, LeftGripLocation, LeftGripRotation);
    MoveHandToSocket(PlayerMesh, RightHandBone, RightGripLocation, RightGripRotation);

    if (bAutoAdjustFingers)
    {
        AdjustFingerIK();
    }
}

void UAutoGripComponent::MoveHandToSocket(USkeletalMeshComponent* PlayerMesh, const FName& HandBone, const FVector& TargetLocation, const FRotator& TargetRotation)
{
    if (!PlayerMesh || PlayerMesh->GetBoneIndex(HandBone) == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveHandToSocket: Invalid hand bone '%s'!"), *HandBone.ToString());
        return;
    }

    FVector BoneWorldLocation = PlayerMesh->GetBoneLocation(HandBone);

    // Debugging visualization: draw a line from the bone's world location to the target.
    DrawDebugLine(GetWorld(), BoneWorldLocation, TargetLocation, FColor::Green, false, 2.0f, 0, 1.0f);
}

void UAutoGripComponent::AdjustFingerIK()
{
    if (!CurrentWeapon) return;

    USkeletalMeshComponent* PlayerMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (!PlayerMesh) return;

    FTransform LeftGripTransform = CurrentWeapon->GetGripTransform(FName("LeftHandGripSocket"));
    FTransform RightGripTransform = CurrentWeapon->GetGripTransform(FName("RightHandGripSocket"));

    ApplyFingerIK(PlayerMesh, LeftHandFingerBoneChains, LeftGripTransform.GetLocation());
    ApplyFingerIK(PlayerMesh, RightHandFingerBoneChains, RightGripTransform.GetLocation());
}

void UAutoGripComponent::ApplyFingerIK(USkeletalMeshComponent* PlayerMesh, const TArray<FFingerBoneChain>& FingerBoneChains, const FVector& TargetLocation)
{
    if (!PlayerMesh) return;

    for (const FFingerBoneChain& BoneChain : FingerBoneChains)
    {
        FVector CurrentTarget = TargetLocation;

        // Process the chain from tip to base.
        for (int32 i = BoneChain.BoneNames.Num() - 1; i >= 0; --i)
        {
            const FName& BoneName = BoneChain.BoneNames[i];
            int32 BoneIndex = PlayerMesh->GetBoneIndex(BoneName);
            if (BoneIndex == INDEX_NONE) continue;

            // Retrieve the bone matrix and create a transform.
            FTransform BoneTransform = FTransform(PlayerMesh->GetBoneMatrix(BoneIndex));
            FVector BoneLocation = BoneTransform.GetLocation();
            // Calculate the rotation needed to look at the current target.
            FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(BoneLocation, CurrentTarget);

            // Draw a debug line from the bone to the current target.
            DrawDebugLine(GetWorld(), BoneLocation, CurrentTarget, FColor::Red, false, 1.0f, 0, 1.0f);

            // Update target for the next bone in the chain.
            CurrentTarget = BoneLocation;
        }
    }
}
