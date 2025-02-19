#include "WeaponBase.h"
#include "Net/UnrealNetwork.h"
#include "WeaponData.h"
#include "Components/SkeletalMeshComponent.h"

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);

    // Default fire mode (for example, Single instead of Idle)
    CurrentFireMode = EFireMode::Single;
    CurrentAmmo = 0;
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    if (WeaponData)  // Updated reference
    {
        CurrentAmmo = WeaponData->MaxAmmo;
    }
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AWeaponBase, CurrentFireMode);
    DOREPLIFETIME(AWeaponBase, CurrentAmmo);
}

void AWeaponBase::Server_Fire_Implementation()
{
    if (CanFire())
    {
        HandleFire();
        Multicast_PlayFireEffect();
    }
}

bool AWeaponBase::Server_Fire_Validate()
{
    return true;
}

void AWeaponBase::Server_Reload_Implementation()
{
    if (CanReload())
    {
        HandleReload();
        Multicast_PlayReloadEffect();
    }
}

bool AWeaponBase::Server_Reload_Validate()
{
    return true;
}

void AWeaponBase::Multicast_PlayFireEffect_Implementation()
{
    PlayFireEffect();
}

void AWeaponBase::Multicast_PlayReloadEffect_Implementation()
{
    PlayReloadEffect();
}

void AWeaponBase::OnRep_WeaponState()
{
    // Handle state changes (e.g., update animations)
}

void AWeaponBase::Fire()
{
    if (GetNetMode() == NM_Client)
    {
        Server_Fire();
    }
}

void AWeaponBase::Reload()
{
    if (GetNetMode() == NM_Client)
    {
        Server_Reload();
    }
}

bool AWeaponBase::CanFire() const
{
    return CurrentAmmo > 0 && CurrentFireMode != EFireMode::Idle;
}

bool AWeaponBase::CanReload() const
{
    return WeaponData && (CurrentAmmo < WeaponData->MaxAmmo);
}

void AWeaponBase::HandleFire()
{
    CurrentAmmo--;

    if (WeaponData && CurrentFireMode == EFireMode::Automatic)
    {
        GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::HandleFire, WeaponData->FireRate, true);
    }
}

void AWeaponBase::HandleReload()
{
    if (WeaponData)
    {
        CurrentAmmo = WeaponData->MaxAmmo;
    }
}

FTransform AWeaponBase::GetGripTransform(FName SocketName) const
{
    if (const USkeletalMeshComponent* SkeletalMesh = FindComponentByClass<USkeletalMeshComponent>())
    {
        return SkeletalMesh->GetSocketTransform(SocketName, RTS_World);
    }
    return FTransform::Identity;
}