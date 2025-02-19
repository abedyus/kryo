#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WeaponHUD.generated.h"

UCLASS()
class WEAPONSYSTEM_API AWeaponHUD : public AHUD
{
    GENERATED_BODY()

public:
    AWeaponHUD();

protected:
    virtual void BeginPlay() override;
    virtual void DrawHUD() override;

public:
    UFUNCTION(BlueprintCallable, Category = "Weapon HUD")
    void UpdateWeaponHUD(class AWeaponBase* CurrentWeapon);

private:
    UPROPERTY()
    class AWeaponBase* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    UTexture2D* Crosshair;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    UTexture2D* WeaponIcon;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    FLinearColor AmmoTextColor;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    FLinearColor AttachmentTextColor;

    void DrawCrosshair();
    void DrawWeaponIcon();
    void DrawAmmoCount();
    void DrawAttachmentStatus();
};