#include "Monster/BNHunterCharacter.h"
#include "Monster/Weapons/BNMonsterWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Monster/BNBaseAIController.h"
#include "Animation/AnimInstance.h"
#include "Monster/BNHunterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/BNMonoCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Monster/BNBlackboardKeys.h" // [추가] 블랙보드 키 헤더 파일을 포함합니다.

ABNHunterCharacter::ABNHunterCharacter()
{
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	GetCapsuleComponent()->SetCapsuleHalfHeight(95.0f);
	GetCapsuleComponent()->SetCapsuleRadius(34.0f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

void ABNHunterCharacter::ActivateMonster()
{
	AController* MyController = GetController();
	if (MyController)
	{
		UE_LOG(LogTemp, Log, TEXT("Log: Character '%s' is possessed by Controller '%s'"), *GetName(), *MyController->GetName());
		if (Cast<ABNBaseAIController>(MyController))
		{
			UE_LOG(LogTemp, Log, TEXT("Log: Controller is confirmed to be an ABNBaseAIController."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error: Controller is NOT an ABNBaseAIController!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: Character '%s' has NO controller at the moment of activation!"), *GetName());
	}
	Super::ActivateMonster();
}

void ABNHunterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	if (DefaultWeaponClass)
	{
		if (UWorld* World = GetWorld())
		{
			EquippedWeapon = World->SpawnActor<ABNMonsterWeapon>(DefaultWeaponClass);
			if (EquippedWeapon)
			{
				EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
				EquippedWeapon->SetOwner(this);
			}
		}
	}

	if (DefaultLanternClass)
	{
		if (UWorld* World = GetWorld())
		{
			EquippedLantern = World->SpawnActor<AActor>(DefaultLanternClass);
			if (EquippedLantern)
			{
				EquippedLantern->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LanternAttachSocketName);
			}
		}
	}
}

void ABNHunterCharacter::EnterIdleState()
{
	Super::EnterIdleState();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABNHunterCharacter::EnterChasingState()
{
	Super::EnterChasingState();
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void ABNHunterCharacter::EnterAttackingState()
{
	Super::EnterAttackingState();
	
	if (HasAuthority())
	{
		Multicast_PlayAttackMontage();
	}
}

void ABNHunterCharacter::Multicast_PlayAttackMontage_Implementation()
{
	if (AttackMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			AnimInstance->Montage_Play(AttackMontage);
		}
	}
}

void ABNHunterCharacter::AnimNotify_ExecuteGuaranteedHit()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("--- AnimNotify_ExecuteGuaranteedHit CALLED ON SERVER ---"));

		AController* MyController = GetController();
		if (!MyController)
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: Character has no valid Controller."));
			return;
		}
	
		ABNHunterAIController* AIController = Cast<ABNHunterAIController>(MyController);
		if (!AIController)
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: Controller is not an ABNHunterAIController. Controller is of type: %s"), *MyController->GetName());
			return;
		}

		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (!BlackboardComp)
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: AIController does not have a BlackboardComponent."));
			return;
		}

		// [핵심 수정] 하드코딩된 "Target" 대신 BBKeys::TargetActor 키를 사용합니다.
		UObject* TargetObject = BlackboardComp->GetValueAsObject(BBKeys::TargetActor);
		if (!TargetObject)
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: Blackboard key '%s' is NULL. Check the Behavior Tree."), *BBKeys::TargetActor.ToString());
			return;
		}

		ABNMonoCharacter* TargetPlayer = Cast<ABNMonoCharacter>(TargetObject);
		if (!TargetPlayer)
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: The object in Blackboard key '%s' is NOT a BNMonoCharacter. It is a '%s'."), *BBKeys::TargetActor.ToString(), *TargetObject->GetName());
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[DEBUG] SUCCESS! Triggering synchronized death for player '%s'."), *TargetPlayer->GetName());
		TargetPlayer->Die(this);
	}
}

void ABNHunterCharacter::AnimNotify_ActivateMeleeCollision()
{
	if (HasAuthority() && EquippedWeapon)
	{
		EquippedWeapon->ActivateMeleeCollision();
	}
}

void ABNHunterCharacter::AnimNotify_DeactivateMeleeCollision()
{
    if (HasAuthority() && EquippedWeapon)
	{
		EquippedWeapon->DeactivateMeleeCollision();
	}
}
