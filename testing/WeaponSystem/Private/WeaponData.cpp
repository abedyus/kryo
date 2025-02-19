#include "WeaponData.h"

UWeaponData::UWeaponData()
    : FireRate(0.f)
    , ReloadTime(0.f)
    , MaxAmmo(0)
    , FireMode(EFireMode::Idle)
{
    // Optionally initialize additional properties here.
}
