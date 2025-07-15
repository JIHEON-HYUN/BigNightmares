// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BNMonoCharacter.h"

#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Abilities/BNBaseAttributeSet.h"
#include "Abilities/BNTarotCardAttributeSet.h"
#include "Animation/AnimInstance/BNMonoAnimInstance.h"
#include "BaseGamePlayTags.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/Input/DataAsset_InputConfig_Player.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "DataAsset/BNMonoCharacterDataAsset.h"
#include "GameFramework/PlayerState/BNPlayerState.h"
#include "Input/BNBaseEnhancedInputComponent.h"
#include "Library/BNAbilitySystemLibrary.h"
#include "Player/InventoryComponent.h"


ABNMonoCharacter::ABNMonoCharacter()
{
	SetReplicates(true);
	GetCapsuleComponent()->InitCapsuleSize(10.f, 25.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(FRotator(-45.f,-180.f,0.f));

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
	BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_UseItem, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_UseItem);
	//TODO(NOTE): Look의 동작이 정해진다면 활성화
	//BaseEnhancedInputComponent->BindNativeInputAction(InputConfigDataAsset, BaseGamePlayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ABNMonoCharacter::Input_Look);
}

void ABNMonoCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f,-180.f, 0.f);

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

		//Debugging : IsValid(MonoCharacterAttributeSet)
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
}

void ABNMonoCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("PossessedBy InitAbilityActorInfo"));
		InitAbilityActorInfo();
	}

	//Debugging Code: GetBNBaseAbilitySystemComponent() && GetAttributeSet<UBNTarotCardAttributeSet>()
	/*if (GetBNBaseAbilitySystemComponent() && GetAttributeSet<UBNTarotCardAttributeSet>())
	{
		const FString AppendString = FString::Printf(TEXT("Owner Actor: %s, AvatarActor: %s"),
			*GetBNBaseAbilitySystemComponent()->GetName(),
			*GetAttributeSet<UBNTarotCardAttributeSet>()->GetName());
		
		UE_LOG(LogTemp, Warning, TEXT("%s"), *AppendString);
	}*/
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
