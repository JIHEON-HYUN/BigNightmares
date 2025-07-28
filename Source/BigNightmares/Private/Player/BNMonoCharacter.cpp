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

// [손전등 기능 추가] 시작: 필요한 헤더 파일들을 포함합니다.
#include "Components/SpotLightComponent.h"
#include "Interfaces/LightSensitive.h"
#include "Kismet/KismetSystemLibrary.h"
// [손전등 기능 추가] 끝

#include "Animation/AnimInstance.h" // [추가] AnimInstance 헤더
#include "Kismet/KismetMathLibrary.h"
#include "Interaction/Mission/InteractionInterface.h" // [추가] 인터페이스 헤더를 포함합니다.
#include "InputAction.h" // [추가] UInputAction을 사용하기 위해 필요합니다.

ABNMonoCharacter::ABNMonoCharacter()
{
	// [손전등 기능 추가] Tick 함수를 사용하기 위해 true로 설정합니다.
	PrimaryActorTick.bCanEverTick = true;
	
	SetReplicates(true);
	GetCapsuleComponent()->InitCapsuleSize(10.f, 25.f);

	//bUseControllerRotationPitch = false;
	//bUseControllerRotationRoll = false;
	//bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 300.f;
	//CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = true;
	//부모 컴포넌트의 회전으로부터 독립, 카메라의 고정된 시점 유지
	//CameraBoom->SetUsingAbsoluteRotation(true);
	//CameraBoom->SetRelativeRotation(FRotator(-45.f,-180.f,0.f));
	CameraBoom->SetRelativeRotation(FRotator(-45.f,0.f,0.f));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; //캐릭터가 이동하는 방향으로 자동으로 회전
	GetCharacterMovement()->RotationRate = FRotator(0.f,360.f,0.f); //캐릭터의 회전 속도, 360 => 1초에 한 바퀴
	//TODO(NOTE): 이동속도 변경을 원하면 해당 수치 조절
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	//TODO(NOTE): 값이 낮을 수 록 천천히 멈춤 나중에 게임에 맞춰 조절
	GetCharacterMovement()->BrakingDecelerationWalking  = 2048.f; //감속 속도 조절 , 기본값 2048

	//TODO(NOTE): Character의 SkeletalMesh의 경로가 바뀌면 이 경로도 반드시 수정 필요
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("/Game/Characters/Player/Meshes/Player.Player"));

	if (SkeletalMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletalMeshAsset.Object);
	}

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0,0.0, -27.0), FRotator(0.0,0.0,0.0));

	//TODO(NOTE): Editor의 ABP_BNMonoAnim의 경로가 바뀐다면 해당 경로 반드시 수정 필요
	static ConstructorHelpers::FClassFinder<UBNMonoAnimInstance> ABP_BNMonoAnimInstance(TEXT("/Game/Characters/Player/Animations/ABP_BNMonoAnimInstance"));

	if (ABP_BNMonoAnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ABP_BNMonoAnimInstance.Class);
	}
	
	//InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// 추가 ~
	// [손전등 기능 수정] 캐릭터에 스포트라이트 컴포넌트를 생성하고, 카메라가 아닌 캐릭터의 '메시'에 부착합니다.
	FlashlightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightComponent"));
	FlashlightComponent->SetupAttachment(GetMesh(), TEXT("skinned_l_innerHand_bnSocket")); 

	// [핵심 수정] 게임 시작 시에는 기본적으로 손전등을 꺼둡니다.
	FlashlightComponent->SetVisibility(false);
	// ~ 추가
}

// [추가] 플레이어를 영구적으로 멈추게 하는 함수입니다.
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
        // 헌터를 바라보도록 플레이어의 회전 값을 변경합니다.
        FVector StartLocation = GetActorLocation();
        FVector TargetLocation = LookAtTarget->GetActorLocation();
        
        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
        LookAtRotation.Pitch = 0.f; // 고개는 숙이지 않도록
        LookAtRotation.Roll = 0.f;  // 몸은 기울이지 않도록

        SetActorRotation(LookAtRotation);
    }
}

// [추가] 즉시 래그돌로 전환하고 충격량을 적용하는 새로운 함수입니다.
void ABNMonoCharacter::HandleImmediateDeath(AActor* DamageCauser)
{
    if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	UE_LOG(LogTemp, Warning, TEXT("'%s' has been killed instantly by ragdoll."), *GetName());

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->DisableInput(PC);
	}

	// 캐릭터의 모든 움직임과 애니메이션 제어를 중단합니다.
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
    GetMesh()->SetAnimInstanceClass(nullptr); 

	// 캡슐 콜리전을 완전히 끕니다.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 래그돌을 위한 물리 설정을 적용합니다.
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

    // 공격자로부터의 방향으로 충격량을 계산하고 적용합니다.
    if (DamageCauser)
    {
        FVector ImpulseDirection = GetActorLocation() - DamageCauser->GetActorLocation();
        ImpulseDirection.Z = 0.5f; // 살짝 위로 뜨도록 Z값을 보정합니다.
        ImpulseDirection.Normalize();

        float ImpulseStrength = 1000.0f; // 충격량의 세기 (조절 가능)
        GetMesh()->AddImpulse(ImpulseDirection * ImpulseStrength, NAME_None, true);
    }
}


// --- 아래 함수들은 이제 이 공격 시퀀스에서 직접 사용되지 않습니다. ---

void ABNMonoCharacter::TriggerGuaranteedDeath()
{
	// 이 함수는 이제 HandleImmediateDeath로 대체되었습니다.
	// 만약의 경우를 대비해 남겨두거나, 다른 종류의 죽음에 사용할 수 있습니다.
	if (bIsDead)
	{
		return;
	}
	
	if (DeathMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(DeathMontage);
			FOnMontageEnded MontageEndedDelegate;
			MontageEndedDelegate.BindUObject(this, &ABNMonoCharacter::OnDeathMontageEnded);
			AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
		}
		else
		{
			HandleLethalHit();
		}
	}
	else
	{
		HandleLethalHit();
	}
}

void ABNMonoCharacter::HandleLethalHit()
{
	// 이 함수는 이제 HandleImmediateDeath로 대체되었습니다.
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

	if (AbilitySystemComponent)
	{
		for (UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
		{
			UE_LOG(LogTemp, Warning, TEXT("ASC has AttributeSet: %s (%p)"), *Set->GetName(), Set);
		}
	}

	// 게임 시작 시, 원래의 최대 걷기 속도를 저장합니다.
	if (GetCharacterMovement())
	{
		OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	}
}

// [디버깅 추가 시작] 매 프레임마다 손전등 로직을 처리하는 Tick 함수 전체를 추가했습니다.
void ABNMonoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 손전등 컴포넌트가 없으면 아무것도 하지 않습니다.
	if (!FlashlightComponent) 
	{
		return;
	}

	// 자신의 역할이 'FlashlightHolder'가 아닐 경우
	if (PlayerRole != EPlayerRole::FlashlightHolder)
	{
		// [핵심 수정] 손전등이 켜져 있다면 확실하게 끕니다.
		if (FlashlightComponent->IsVisible())
		{
			FlashlightComponent->SetVisibility(false);
		}

		// 이전에 비추던 대상이 있었다면 빛이 꺼졌다고 알려줍니다.
		if (LastLitActor.IsValid() && LastLitActor->Implements<ULightSensitive>())
		{
			ILightSensitive::Execute_OnHitByLight(LastLitActor.Get(), false);
			LastLitActor = nullptr;
		}
		return;
	}

	// --- 아래부터는 FlashlightHolder일 때만 실행되는 로직 ---

	// [핵심 수정] 손전등이 꺼져 있다면 켭니다.
	if (!FlashlightComponent->IsVisible())
	{
		FlashlightComponent->SetVisibility(true);
	}
	
	// 캐릭터가 바라보는 방향으로 조준하는 로직
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
		EDrawDebugTrace::None, // 디버깅용 선 그리기
		HitResult,
		true
	);

	AActor* CurrentHitActor = bHit ? HitResult.GetActor() : nullptr;

	if(bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace HIT! Actor: %s"), *CurrentHitActor->GetName());
	}

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

// [추가] SetInteractableActor 함수의 내용을 구현합니다.
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
	checkf(SubSystem , TEXT("EnhancedInputLocalPlayerSubsystem is null — check if LocalPlayer is valid and Subsystem is initialized properly."));
	SubSystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UBNBaseEnhancedInputComponent* BaseEnhancedInputComponent = CastChecked<UBNBaseEnhancedInputComponent>(PlayerInputComponent);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Move,ETriggerEvent::Triggered, this,&ABNMonoCharacter::Input_Move);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Look,ETriggerEvent::Triggered, this,&ABNMonoCharacter::Input_Look);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_UseItem, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_UseItem);
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_Jump);
	//TODO(NOTE): Look의 동작이 정해진다면 활성화
	//BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_Look);

	// InteractAction 변수가 유효할 때, Input_Interact 함수를 연결합니다.
	if (InteractAction)
	{
		BaseEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_Interact);
	}
}

// [수정] 상호작용 키를 눌렀을 때의 로직
void ABNMonoCharacter::Input_Interact()
{
	// [수정] 바라보는 대상(FocusedActor)이 아닌, 겹쳐있는 대상(OverlappedInteractableActor)과 상호작용하도록 변경합니다.
	if (OverlappedInteractableActor.IsValid())
	{
		Server_Interact(OverlappedInteractableActor.Get());
	}
}

// [추가] 서버에서 실제 상호작용을 실행하는 함수
void ABNMonoCharacter::Server_Interact_Implementation(AActor* TargetActor)
{
	// 서버에서도 다시 한번 유효성과 인터페이스를 확인하는 것이 안전합니다.
	if (TargetActor && TargetActor->Implements<UInteractionInterface>())
	{
		// 인터페이스를 통해 대상 액터의 상호작용 함수를 안전하게 호출합니다.
		IInteractionInterface::Execute_Interact(TargetActor, this);
	}
}

// [추가] 상호작용 대상을 설정하는 함수
void ABNMonoCharacter::SetOverlappedInteractable(AActor* Interactable)
{
	OverlappedInteractableActor = Interactable;
}

// [추가] 상호작용 대상을 비우는 함수
void ABNMonoCharacter::ClearOverlappedInteractable(AActor* Interactable)
{
	if (OverlappedInteractableActor.Get() == Interactable)
	{
		OverlappedInteractableActor = nullptr;
	}
}

void ABNMonoCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	// 이동 불가 상태일 때는 이동 입력을 무시합니다.
	if (bIsImmobilized || bIsDead) // [수정] 사망 시에도 이동 불가
	{
		return;
	}
	
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	//const FRotator MovementRotation(0.f,-180.f, 0.f);
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

//TODO: Look이 발생할 일이 생기면 그때 다시 정의
void ABNMonoCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
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
	
	if (AbilitySystemComponent)
	{
		for (UAttributeSet* Set : AbilitySystemComponent->GetSpawnedAttributes())
		{
			UE_LOG(LogTemp, Warning, TEXT("ASC has AttributeSet: %s (%p)"), *Set->GetName(), Set);
		}
	}
	
	float Temp = GetCharacterMovement()->GetMaxSpeed();
	UE_LOG(LogTemp,Warning, TEXT("MAX SPEED %.f"), Temp);

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

#pragma region Debugging : IsValid(MonoCharacterAttributeSet)
		/*if (IsValid(MonoCharacterAttributeSet))
		{
			UE_LOG(LogTemp, Warning, TEXT("MonoCharacterAttributeSet IsValid"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MonoCharacterAttributeSet Not IsValid"));
			UE_LOG(LogTemp, Error, TEXT("%s"), *MonoCharacterAttributeSet.GetName());			
		}
		*/
#pragma endregion

		if (!IsValid(MonoCharacterAttributeSet)) return;
		
		//UE_LOG(LogTemp, Warning, TEXT("PlayerState Successes"));
		if (IsValid(AbilitySystemComponent))
		{
			//UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent IsValid"));
			AbilitySystemComponent->InitAbilityActorInfo(BNPlayerState, this);
			BindCallbacksToDependencies();

			if (HasAuthority())
			{
				//UE_LOG(LogTemp, Warning, TEXT("MonoCharacter HasAuthority"));
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
		//UE_LOG(LogTemp, Error, TEXT("ClassInfo->GetName() : %s"), *ClassInfo->GetName());
		if (FMonoCharacterDefaultInfo* SelectedClassInfo = ClassInfo->ClassDefaultInfoMap.Find(CharacterTag))
		{
			//UE_LOG(LogTemp, Error, TEXT("%s"), *CharacterTag.ToString());
			//UE_LOG(LogTemp, Error, TEXT("SelectedClassInfo->DefaultAttributes->GetName() : %s"), *SelectedClassInfo->DefaultAttributes->GetName());
			if (IsValid(AbilitySystemComponent))
			{ 
				//UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent Setting"));
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
	//CurrentMoveSpeed = NewSpeed;
}

void ABNMonoCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("PossessedBy InitAbilityActorInfo"));
		InitAbilityActorInfo();
	}

#pragma region Debugging Code: GetBNBaseAbilitySystemComponent() && GetAttributeSet<UBNTarotCardAttributeSet>()
	/*if (GetBNBaseAbilitySystemComponent() && GetAttributeSet<UBNTarotCardAttributeSet>())
	{
		const FString AppendString = FString::Printf(TEXT("Owner Actor: %s, AvatarActor: %s"),
			*GetBNBaseAbilitySystemComponent()->GetName(),
			*GetAttributeSet<UBNTarotCardAttributeSet>()->GetName());
		
		UE_LOG(LogTemp, Warning, TEXT("%s"), *AppendString);
	}*/
#pragma endregion
}

void ABNMonoCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState InitAbilityActorInfo"));
	InitAbilityActorInfo();
}

void ABNMonoCharacter::Client_ApplyMoveSpeed_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	//UE_LOG(LogTemp, Warning, TEXT("클라이언트에 속도 적용: %f"), NewSpeed);
}

// PlayerRole 변수를 네트워크를 통해 복제하도록 설정
void ABNMonoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// PlayerRole 변수가 복제 대상임을 엔진에 알림
	DOREPLIFETIME(ABNMonoCharacter, PlayerRole);
}