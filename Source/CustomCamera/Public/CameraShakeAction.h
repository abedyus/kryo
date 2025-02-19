// CameraShakeAction.h

#pragma once

#include "CoreMinimal.h"
#include "CameraShakeAction.generated.h"

UENUM(BlueprintType)
enum class ECameraShakeAction : uint8
{
    None UMETA(DisplayName = "None"),
    Walk UMETA(DisplayName = "Walk"),
    Run UMETA(DisplayName = "Run"),
    Jump UMETA(DisplayName = "Jump"),
    Land UMETA(DisplayName = "Land"),
    Attack UMETA(DisplayName = "Attack"),
    Damage UMETA(DisplayName = "Damage"),
    Death UMETA(DisplayName = "Death")
};
