#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "InputActionData.generated.h"

USTRUCT(BlueprintType)
struct FInputActionMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    FGameplayTag InputTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* InputAction;
};

UCLASS(BlueprintType)
class PLAYERCHARACTER_API UInputActionData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    TArray<FInputActionMapping> InputActionMappings;
};