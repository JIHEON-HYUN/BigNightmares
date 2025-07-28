// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BNMonoCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "Abilities/BNTarotCardAttributeSet.h"
#include "Animation/AnimInstance/BNMonoAnimInstance.h"
#include "BaseGamePlayTags.h"
#include "DataAsset/Input/DataAsset_InputConfig_Player.h"
#include "DataAsset/BNMonoCharacterDataAsset.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Input/BNBaseEnhancedInputComponent.h"
#include "Library/BNAbilitySystemLibrary.h"

#include "Player/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/SpotLightComponent.h"
#include "Interfaces/LightSensitive.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interaction/Mission/InteractionInterface.h"
#include "InputAction.h"

ABNMonoCharacter::ABNMonoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);
	GetCapsuleComponent()->InitCapsuleSize(10.f, 25.f);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeRotation(FRotator(-45.f,0.f,0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f,360.f,0.f); 
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->BrakingDecelerationWalking  = 2048.f; 

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Player/Meshes/Player.Player"));

	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0,0.0, -27.0), FRotator(0.0,0.0,0.0));
	
	static ConstructorHelpers::FClassFinder<UBNMonoAnimInstance> ABP_BNMonoAnimInstance(TEXT("/Game/Characters/Player/Animations/ABP_BNMonoAnimInstance"));

	if (ABP_BNMonoAnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ABP_BNMonoAnimInstance.Class);
	}
	
	FlashlightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightComponent"));
	FlashlightComponent->SetupAttachment(GetMesh(), TEXT("skinned_l_innerHand_bnSocket")); 

	FlashlightComponent->SetVisibility(false);
}

// [추가] 서버에서 호출되어 사망 처리를 시작하는 함수
void ABNMonoCharacter::Die(AActor* DamageCauser)
{
	// 오직 서버(HasAuthority)만이 이 함수를 통해 모든 클라이언트에게 사망 명령을 내릴 수 있습니다.
	if (HasAuthority())
	{
		Multicast_HandleDeath(DamageCauser);
	}
}

// [수정] 이 함수가 이제 모든 클라이언트에서 실행될 Multicast RPC의 구현부입니다.
void ABNMonoCharacter::Multicast_HandleDeath_Implementation(AActor* DamageCauser)
{
    if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	UE_LOG(LogTemp, Warning, TEXT("'%s' has been killed by ragdoll on all clients."), *GetName());

	// 로컬 플레이어인 경우에만 입력을 비활성화 합니다.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if(PC->IsLocalController())
		{
			PC->DisableInput(PC);
		}
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
    GetMesh()->SetAnimInstanceClass(nullptr); 

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

    if (DamageCauser)
    {
        FVector ImpulseDirection = GetActorLocation() - DamageCauser->GetActorLocation();
        ImpulseDirection.Z = 0.5f; 
        ImpulseDirection.Normalize();

        float ImpulseStrength = 1000.0f; 
        GetMesh()->AddImpulse(ImpulseDirection * ImpulseStrength, NAME_None, true);
    }
}


void ABNMonoCharacter::FreezePlayer(AActor* LookAtTarget)
{
    if (bIsImmobilized || bIsDead)
    {
        return;
    }
    bIsImmobilized = true;
    GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
    UE_LOG(LogTemp, Warning, TEXT("Player '%s' has been frozen."), *GetName());

    if (LookAtTarget)
    {
        FVector StartLocation = GetActorLocation();
        FVector TargetLocation = LookAtTarget->GetActorLocation();
        
        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
        LookAtRotation.Pitch = 0.f;
        LookAtRotation.Roll = 0.f; 

        SetActorRotation(LookAtRotation);
    }
}


void ABNMonoCharacter::HandleImmediateDeath(AActor* DamageCauser)
{
    // 이제 이 함수의 주요 로직은 Multicast로 옮겨졌습니다.
    // 다른 코드와의 호환성을 위해 새로운 Die() 함수를 호출하도록 합니다.
    Die(DamageCauser);
}

// 몽타주 재생 후 사망 처리 등이 필요할 때를 위해 남겨둡니다.
void ABNMonoCharacter::TriggerGuaranteedDeath()
{
	if (bIsDead)
	{
		return;
	}
	
	// 즉사 공격으로 변경. 공격자 정보가 필요하다면 다른 곳에서 받아와 Die()에 넘겨줘야 합니다.
    Die(nullptr); 
}

void ABNMonoCharacter::HandleLethalHit()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}

void ABNMonoCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    HandleLethalHit();
}

void ABNMonoCharacter::EndImmobilization()
{
	bIsImmobilized = false;
	GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed;
	UE_LOG(LogTemp, Log, TEXT("Player is no longer immobilized."));
	GetWorldTimerManager().ClearTimer(ImmobilizeTimerHandle);
}

void ABNMonoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	if (GetCharacterMovement())
	{
		OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
}

void ABNMonoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!FlashlightComponent) 
	{
		return;
	}

	if (PlayerRole != EPlayerRole::FlashlightHolder)
	{
		if (FlashlightComponent->IsVisible())
		{
			FlashlightComponent->SetVisibility(false);
		}

		if (LastLitActor.IsValid() && LastLitActor->Implements<ULightSensitive>())
		{
			ILightSensitive::Execute_OnHitByLight(LastLitActor.Get(), false);
			LastLitActor = nullptr;
		}
		return;
	}

	if (!FlashlightComponent->IsVisible())
	{
		FlashlightComponent->SetVisibility(true);
	}
	
	const FVector TraceStart = FlashlightComponent->GetComponentLocation();
	const FVector FinalTraceDirection = GetActorForwardVector();
	const float TraceDistance = 5000.0f;
	const FVector TraceEnd = TraceStart + (FinalTraceDirection * TraceDistance);

	const FRotator FlashlightRotation = FinalTraceDirection.Rotation();
	FlashlightComponent->SetWorldRotation(FlashlightRotation);

	FHitResult HitResult;
	TEnumAsByte<ETraceTypeQuery> TraceChannel = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel1);
	
	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		TraceChannel, 
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	AActor* CurrentHitActor = bHit ? HitResult.GetActor() : nullptr;

	if (LastLitActor.Get() != CurrentHitActor)
	{
		if (LastLitActor.IsValid() && LastLitActor->Implements<ULightSensitive>())
		{
			ILightSensitive::Execute_OnHitByLight(LastLitActor.Get(), false);
		}
	}

	if (CurrentHitActor && CurrentHitActor->Implements<ULightSensitive>())
	{
		ILightSensitive::Execute_OnHitByLight(CurrentHitActor, true);
	}
	
	LastLitActor = CurrentHitActor;
}

void ABNMonoCharacter::SetInteractableActor(AActor* Actor)
{
	OverlappedInteractableActor = Actor;
}

void ABNMonoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	checkf(InputConfigDataAsset, TEXT("Forgot to assign a valid data asset as input config"));

	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	checkf(SubSystem , TEXT("EnhancedInputLocalPlayerSubsystem is null"));
	SubSystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UBNBaseEnhancedInputComponent* BaseEnhancedInputComponent = CastChecked<UBNBaseEnhancedInputComponent>(PlayerInputComponent);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Move,ETriggerEvent::Triggered, this,&ABNMonoCharacter::Input_Move);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Look,ETriggerEvent::Triggered, this,&ABNMonoCharacter::Input_Look);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_UseItem, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_UseItem);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_Jump);
	
	if (InteractAction)
	{
		BaseEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_Interact);
	}
}

void ABNMonoCharacter::Input_Interact()
{
	if (OverlappedInteractableActor.IsValid())
	{
		Server_Interact(OverlappedInteractableActor.Get());
	}
}

void ABNMonoCharacter::Server_Interact_Implementation(AActor* TargetActor)
{
	if (TargetActor && TargetActor->Implements<UInteractionInterface>())
	{
		IInteractionInterface::Execute_Interact(TargetActor, this);
	}
}

void ABNMonoCharacter::SetOverlappedInteractable(AActor* Interactable)
{
	OverlappedInteractableActor = Interactable;
}

void ABNMonoCharacter::ClearOverlappedInteractable(AActor* Interactable)
{
	if (OverlappedInteractableActor.Get() == Interactable)
	{
		OverlappedInteractableActor = nullptr;
	}
}

void ABNMonoCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	if (bIsImmobilized || bIsDead)
	{
		return;
	}
	
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f,Controller->GetControlRotation().Yaw, 0.f);

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABNMonoCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ABNMonoCharacter::Input_Jump(const FInputActionValue& InputActionValue)
{
	if (InputActionValue.Get<bool>())
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ABNMonoCharacter::Input_UseItem(const FInputActionValue& InputActionValue)
{	
	//
}


UAbilitySystemComponent* ABNMonoCharacter::GetAbilitySystemComponent() const
{
	return Super::GetAbilitySystemComponent();
}

void ABNMonoCharacter::InitAbilityActorInfo()
{
	if (ABNPlayerState* BNPlayerState = GetPlayerState<ABNPlayerState>())
	{
		AbilitySystemComponent = BNPlayerState->GetBNBaseAbilitySystemComponent();
		MonoCharacterAttributeSet = BNPlayerState->GetBNBaseAttributeSet();

		if (!IsValid(MonoCharacterAttributeSet)) return;
		
		if (IsValid(AbilitySystemComponent))
		{
			AbilitySystemComponent->InitAbilityActorInfo(BNPlayerState, this);
			BindCallbacksToDependencies();

			if (HasAuthority())
			{
				InitClassDefaults();
			}
		}
	}
}

void ABNMonoCharacter::InitClassDefaults()
{
	if (!CharacterTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("No Character Tag Select In This Character %s"), *GetNameSafe(this));
	}
	else if (UBNMonoCharacterDataAsset* ClassInfo = UBNAbilitySystemLibrary::GetMonoCharacterDataAsset(this))
	{
		if (FMonoCharacterDefaultInfo* SelectedClassInfo = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			if (IsValid(AbilitySystemComponent))
			{ 
				AbilitySystemComponent->AddCharacterAbilities(SelectedClassInfo->StartingAbility);
				AbilitySystemComponent->AddCharacterPassiveAbilities(SelectedClassInfo->PassiveAbility);
				AbilitySystemComponent->InitializeDefaultAttributes(SelectedClassInfo->DefaultAttributes);
			}
		}
	}
}

void ABNMonoCharacter::BindCallbacksToDependencies()
{
	if (IsValid(AbilitySystemComponent) && IsValid(MonoCharacterAttributeSet) && IsLocallyControlled())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBNTarotCardAttributeSet::GetBaseMoveSpeedAttribute()).AddLambda(
			[this] (const FOnAttributeChangeData& Data)
			{
				Server_SetMaxWalkSpeed(Data.NewValue);
			});
	}
}

void ABNMonoCharacter::BroadcastInitialValues()
{
	if (IsValid(MonoCharacterAttributeSet))
	{
		if (auto* AttrSet = GetAttributeSet<UBNTarotCardAttributeSet>())
		{
			Server_SetMaxWalkSpeed(AttrSet->GetBaseMoveSpeed());
		}
	}
}

void ABNMonoCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void ABNMonoCharacter::Server_SetMaxWalkSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	Client_ApplyMoveSpeed(NewSpeed);
}

void ABNMonoCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		InitAbilityActorInfo();
	}
}

void ABNMonoCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void ABNMonoCharacter::Client_ApplyMoveSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void ABNMonoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABNMonoCharacter, PlayerRole);
}