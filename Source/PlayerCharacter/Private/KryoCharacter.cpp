#include "KryoCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CustomCamera/Public/CustomCameraComponent.h"
#include "WeaponSystem/Public/WeaponBase.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagsManager.h"
#include "Kismet/GameplayStatics.h"
#include "InputActionData.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"


AKryoCharacter::AKryoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	CustomCamera = CreateDefaultSubobject<UCustomCameraComponent>(TEXT("CustomCamera"));
	CustomCamera->SetupAttachment(GetRootComponent());
	CustomCamera->SetCameraMode(ECameraMode::ThirdPerson);

	bIsAiming = false;
	bIsRunning = false;
	bIsCrouching = false;
	bIsJumping = false;

	WeaponSocketName = "WeaponSocket";
	GameplayState = EGameplayState::Idle;
	CurrentWeaponIndex = 0;
}

void AKryoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CustomCamera)
	{
		CustomCamera->Activate();
	}

	if (HasAuthority())
	{
		for (const TSubclassOf<AWeaponBase>& WeaponClass : DefaultWeapons)
		{
			if (WeaponClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
				if (Weapon)
				{
					Weapons.Add(Weapon);
					Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
					Weapon->SetActorHiddenInGame(true);
				}
			}
		}
		if (Weapons.Num() > 0)
		{
			EquipWeapon(0);
		}
	}

	if (IsLocallyControlled() && InputActionData && InputMappingContext)
	{
		BindInputActions();
	}
}

void AKryoCharacter::BindInputActions()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		auto FindActionByTag = [this](const FGameplayTag& Tag) -> UInputAction*
			{
				for (const FInputActionMapping& Mapping : InputActionData->InputActionMappings)
				{
					if (Mapping.InputTag == Tag)
					{
						return Mapping.InputAction;
					}
				}
				return nullptr;
			};

		if (UInputAction* MoveAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Move")))
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKryoCharacter::Move);
		}
		if (UInputAction* LookAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Look")))
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKryoCharacter::Look);
		}
		if (UInputAction* FireAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Fire")))
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnFire);
		}
		if (UInputAction* ReloadAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Reload")))
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnReload);
		}
		if (UInputAction* AimAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Aim")))
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AKryoCharacter::OnStopAim);
		}
		if (UInputAction* RunAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Run")))
		{
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnRun);
			EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AKryoCharacter::OnStopRun);
		}
		if (UInputAction* SwitchWeaponAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.SwitchWeapon")))
		{
			EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnSwitchWeapon);
		}
		if (UInputAction* MeleeAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Melee")))
		{
			EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnMeleeAttack);
		}
		if (UInputAction* InteractAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Interact")))
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnInteract);
		}
		if (UInputAction* JumpAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Jump")))
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnJump);
		}
		if (UInputAction* CrouchAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Crouch")))
		{
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnCrouch);
		}
		if (UInputAction* ThrowAction = FindActionByTag(FGameplayTag::RequestGameplayTag("Input.Action.Throw")))
		{
			EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AKryoCharacter::OnThrow);
		}
	}
}

void AKryoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AKryoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateGameplayState();
}

void AKryoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AKryoCharacter, GameplayState);
	DOREPLIFETIME(AKryoCharacter, Weapons);
	DOREPLIFETIME(AKryoCharacter, CurrentWeaponIndex);
}

void AKryoCharacter::Move(const FInputActionValue& Value)
{
	// Implement movement logic here.
}

void AKryoCharacter::Look(const FInputActionValue& Value)
{
	// Implement look logic here.
}

void AKryoCharacter::OnFire(const FInputActionValue& Value)
{
	Server_Fire();
}

void AKryoCharacter::Server_Fire_Implementation()
{
	if (Weapons.IsValidIndex(CurrentWeaponIndex) && GameplayState != EGameplayState::Reloading)
	{
		Weapons[CurrentWeaponIndex]->Fire();
	}
}

bool AKryoCharacter::Server_Fire_Validate()
{
	return true;
}

void AKryoCharacter::OnReload(const FInputActionValue& Value)
{
	Server_Reload();
}

void AKryoCharacter::Server_Reload_Implementation()
{
	if (Weapons.IsValidIndex(CurrentWeaponIndex) && GameplayState != EGameplayState::Reloading)
	{
		GameplayState = EGameplayState::Reloading;
		Weapons[CurrentWeaponIndex]->Reload();
		GameplayState = EGameplayState::Idle;
	}
}

bool AKryoCharacter::Server_Reload_Validate()
{
	return true;
}

void AKryoCharacter::OnAim(const FInputActionValue& Value)
{
	Server_StartAiming();
}

void AKryoCharacter::Server_StartAiming_Implementation()
{
	bIsAiming = true;
	GameplayState = EGameplayState::Aiming;
}

bool AKryoCharacter::Server_StartAiming_Validate()
{
	return true;
}

void AKryoCharacter::OnStopAim(const FInputActionValue& Value)
{
	Server_StopAiming();
}

void AKryoCharacter::Server_StopAiming_Implementation()
{
	bIsAiming = false;
	GameplayState = EGameplayState::Idle;
}

bool AKryoCharacter::Server_StopAiming_Validate()
{
	return true;
}

void AKryoCharacter::OnRun(const FInputActionValue& Value)
{
	Server_StartRunning();
}

void AKryoCharacter::Server_StartRunning_Implementation()
{
	bIsRunning = true;
	GameplayState = EGameplayState::Running;
}

bool AKryoCharacter::Server_StartRunning_Validate()
{
	return true;
}

void AKryoCharacter::OnStopRun(const FInputActionValue& Value)
{
	Server_StopRunning();
}

void AKryoCharacter::Server_StopRunning_Implementation()
{
	bIsRunning = false;
	GameplayState = EGameplayState::Idle;
}

bool AKryoCharacter::Server_StopRunning_Validate()
{
	return true;
}

void AKryoCharacter::OnSwitchWeapon(const FInputActionValue& Value)
{
	Server_SwitchWeapon();
}

void AKryoCharacter::Server_SwitchWeapon_Implementation()
{
	if (Weapons.Num() > 1)
	{
		int32 NewWeaponIndex = (CurrentWeaponIndex + 1) % Weapons.Num();
		EquipWeapon(NewWeaponIndex);
	}
}

bool AKryoCharacter::Server_SwitchWeapon_Validate()
{
	return true;
}

void AKryoCharacter::OnMeleeAttack(const FInputActionValue& Value)
{
	Server_MeleeAttack();
}

void AKryoCharacter::Server_MeleeAttack_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Melee Attack"));
}

bool AKryoCharacter::Server_MeleeAttack_Validate()
{
	return true;
}

void AKryoCharacter::OnInteract(const FInputActionValue& Value)
{
	Server_Interact();
}

void AKryoCharacter::Server_Interact_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Interact"));
}

bool AKryoCharacter::Server_Interact_Validate()
{
	return true;
}

void AKryoCharacter::OnJump(const FInputActionValue& Value)
{
	Server_Jump();
}

void AKryoCharacter::Server_Jump_Implementation()
{
	Jump();
	bIsJumping = true;
	GameplayState = EGameplayState::Jumping;
}

bool AKryoCharacter::Server_Jump_Validate()
{
	return true;
}

void AKryoCharacter::OnCrouch(const FInputActionValue& Value)
{
	Server_Crouch();
}

void AKryoCharacter::Server_Crouch_Implementation()
{
	Crouch();
	bIsCrouching = true;
	GameplayState = EGameplayState::Crouching;
}

bool AKryoCharacter::Server_Crouch_Validate()
{
	return true;
}

void AKryoCharacter::OnThrow(const FInputActionValue& Value)
{
	Server_Throw();
}

void AKryoCharacter::Server_Throw_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Throw"));
}

bool AKryoCharacter::Server_Throw_Validate()
{
	return true;
}

void AKryoCharacter::EquipWeapon(int32 WeaponIndex)
{
	if (Weapons.IsValidIndex(WeaponIndex))
	{
		if (Weapons.IsValidIndex(CurrentWeaponIndex))
		{
			Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(true);
		}
		CurrentWeaponIndex = WeaponIndex;
		Weapons[CurrentWeaponIndex]->SetActorHiddenInGame(false);
	}
}

void AKryoCharacter::UpdateGameplayState()
{
	if (bIsAiming)
	{
		GameplayState = EGameplayState::Aiming;
	}
	else if (bIsRunning)
	{
		GameplayState = EGameplayState::Running;
	}
	else if (bIsCrouching)
	{
		GameplayState = EGameplayState::Crouching;
	}
	else if (bIsJumping)
	{
		GameplayState = EGameplayState::Jumping;
	}
	else
	{
		GameplayState = EGameplayState::Idle;
	}
}

void AKryoCharacter::AttachWeaponToSocket(AWeaponBase* Weapon, FName SocketName)
{
	if (Weapon && GetMesh())
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

// --- New functions to resolve unresolved external symbols ---
void AKryoCharacter::FireWeapon()
{
	OnFire(FInputActionValue());
}

void AKryoCharacter::ReloadWeapon()
{
	OnReload(FInputActionValue());
}

void AKryoCharacter::SwitchWeapon()
{
	OnSwitchWeapon(FInputActionValue());
}
