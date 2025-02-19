#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponData.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Single      UMETA(DisplayName = "Single"),
    Burst       UMETA(DisplayName = "Burst"),
    Automatic   UMETA(DisplayName = "Automatic")
};

USTRUCT(BlueprintType)
struct FWeaponAttachment
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attachment")
    FName SocketName;
};

UCLASS(Blueprintable)
class WEAPONSYSTEM_API UWeaponData : public UDataAsset
{
    GENERATED_BODY()

public:
    UWeaponData();

    // Weapon stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float ReloadTime;

    // Ammo count
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo;

    // Default fire mode (configured per weapon)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    EFireMode FireMode;

    // Weapon mesh and effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSoftObjectPtr<USkeletalMesh> WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
    TSoftObjectPtr<UParticleSystem> MuzzleFlashEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
    TSoftObjectPtr<USoundBase> FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
    TSoftObjectPtr<USoundBase> ReloadSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSoftObjectPtr<UTexture2D> WeaponIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSoftObjectPtr<UTexture2D> Crosshair;

    // Attachments (if you want to display attachment info)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attachments")
    TArray<FWeaponAttachment> Attachments;
};
