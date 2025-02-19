#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "WeaponSystem/Public/WeaponBase.h"
#include "CustomCamera/Public/CustomCameraComponent.h"
#include "InputActionData.h"
#include "InputMappingContext.h"
#include "KryoCharacter.generated.h"

UENUM(BlueprintType)
enum class EGameplayState : uint8
{
	Idle,
	Aiming,
	Running,
	Reloading,
	Crouching,
	Jumping,
	Swimming,
	Dead,
	InCover
};

UCLASS()
class PLAYERCHARACTER_API AKryoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AKryoCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputActionData* InputActionData;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	void BindInputActions();

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Custom Camera Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCustomCameraComponent* CustomCamera;

	// Weapon System
	// Marked for replication
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Replicated)
	TArray<AWeaponBase*> Weapons;

	// Marked for replication
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Replicated)
	int32 CurrentWeaponIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<TSubclassOf<AWeaponBase>> DefaultWeapons;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(int32 WeaponIndex);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SwitchWeapon();

	// Gameplay State (already marked for replication)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Replicated)
	EGameplayState GameplayState;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Motion Matching Inputs (to be overridden as needed)
	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);

private:
	// Camera/Movement state flags
	bool bIsAiming;
	bool bIsRunning;
	bool bIsCrouching;
	bool bIsJumping;

	// Weapon Socket name
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;

	// --- Server RPC Functions ---
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reload();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartAiming();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopAiming();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartRunning();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopRunning();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SwitchWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MeleeAttack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Jump();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Crouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Throw();

	// Helper Functions
	void UpdateGameplayState();
	void AttachWeaponToSocket(AWeaponBase* Weapon, FName SocketName);

	// Enhanced Input Handlers
	void OnFire(const FInputActionValue& Value);
	void OnReload(const FInputActionValue& Value);
	void OnAim(const FInputActionValue& Value);
	void OnStopAim(const FInputActionValue& Value);
	void OnRun(const FInputActionValue& Value);
	void OnStopRun(const FInputActionValue& Value);
	void OnSwitchWeapon(const FInputActionValue& Value);
	void OnMeleeAttack(const FInputActionValue& Value);
	void OnInteract(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);
	void OnThrow(const FInputActionValue& Value);
};
