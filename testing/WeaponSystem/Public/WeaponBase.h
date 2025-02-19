#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponData.h"  // Updated include for the renamed data asset
#include "WeaponBase.generated.h"

UCLASS()
class WEAPONSYSTEM_API AWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWeaponBase();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // Replicated weapon state
    UPROPERTY(ReplicatedUsing = OnRep_WeaponState, BlueprintReadOnly, Category = "Weapon")
    EFireMode CurrentFireMode;

    // Replicated ammo count
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo;

    // Weapon data asset (holds properties such as MaxAmmo, FireRate, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    UWeaponData* WeaponData;

    // Server RPCs
    UFUNCTION(Server, Reliable, WithValidation, Category = "Weapon")
    void Server_Fire();

    UFUNCTION(Server, Reliable, WithValidation, Category = "Weapon")
    void Server_Reload();

    // Client RPCs for playing effects
    UFUNCTION(NetMulticast, Reliable, Category = "Weapon")
    void Multicast_PlayFireEffect();

    UFUNCTION(NetMulticast, Reliable, Category = "Weapon")
    void Multicast_PlayReloadEffect();

    // Replication notification
    UFUNCTION()
    void OnRep_WeaponState();

    // Fire and Reload functions
    void Fire();
    void Reload();

    // Check functions
    bool CanFire() const;
    bool CanReload() const;

    // Returns the world transform for the specified socket name.
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual FTransform GetGripTransform(FName SocketName) const;

    // Effect functions – implement in Blueprint
    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayFireEffect();

    UFUNCTION(BlueprintImplementableEvent, Category = "Weapon")
    void PlayReloadEffect();

    // --- New accessor functions ---
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    int32 GetCurrentAmmo() const { return CurrentAmmo; }

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    int32 GetMaxAmmo() const { return WeaponData ? WeaponData->MaxAmmo : 0; }

private:
    // Timer handle for automatic fire
    FTimerHandle FireTimerHandle;

    // Internal functions
    void HandleFire();
    void HandleReload();
};
