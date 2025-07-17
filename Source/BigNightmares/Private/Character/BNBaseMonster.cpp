// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"

ABNBaseMonster::ABNBaseMonster()
{
    PrimaryActorTick.bCanEverTick = false;

    // 어빌리티 시스템 컴포넌트를 생성
    AbilitySystemComponent = CreateDefaultSubobject<UBNBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* ABNBaseMonster::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ABNBaseMonster::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    // [복원] 어빌리티 시스템 초기화 코드만 남깁니다.
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ABNBaseMonster::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent && StateDataAsset)
    {
        // 상태 전환 함수를 호출하여 Dormant 상태로 진입합니다.
        TransitionToState(StateDataAsset->DormantStateTag);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] does not have a valid StateDataAsset!"), *GetName());
    }
}

void ABNBaseMonster::ActivateMonster()
{
    if (!AbilitySystemComponent || !StateDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("BNBaseMonster::ActivateMonster - AbilitySystemComponent or StateDataAsset is NULL!"));
        return;
    }
    
    // 이미 활성 상태(Idle, Chase, Attack 등)인지 확인합니다.
    bool bIsAlreadyActive = HasStateTag(StateDataAsset->IdleStateTag) || HasStateTag(StateDataAsset->ChaseStateTag) || HasStateTag(StateDataAsset->AttackStateTag);
    if (bIsAlreadyActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("BNBaseMonster::ActivateMonster - Monster is already active."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[%s] is being activated. Removing Dormant State."), *GetName());

    // Dormant 태그를 제거하고, Idle 상태로 전환합니다.
    EnterIdleState();
}

void ABNBaseMonster::EnterIdleState()
{
    if (!StateDataAsset) return;
    TransitionToState(StateDataAsset->IdleStateTag);
}

void ABNBaseMonster::EnterChasingState()
{
    if (!StateDataAsset) return;
    TransitionToState(StateDataAsset->ChaseStateTag);
}

void ABNBaseMonster::EnterAttackingState()
{
    if (!StateDataAsset) return;
    TransitionToState(StateDataAsset->AttackStateTag);
}

void ABNBaseMonster::TransitionToState(const FGameplayTag& NewStateTag)
{
    if (!AbilitySystemComponent || !StateDataAsset || !NewStateTag.IsValid()) return;

    FGameplayTagContainer TagsToRemove;
    TagsToRemove.AddTag(StateDataAsset->DormantStateTag);
    TagsToRemove.AddTag(StateDataAsset->IdleStateTag);
    TagsToRemove.AddTag(StateDataAsset->ChaseStateTag);
    TagsToRemove.AddTag(StateDataAsset->AttackStateTag);
    AbilitySystemComponent->RemoveReplicatedLooseGameplayTags(TagsToRemove);
	
    AbilitySystemComponent->AddReplicatedLooseGameplayTag(NewStateTag);
    UE_LOG(LogTemp, Log, TEXT("[%s] transitioned to state: %s"), *GetName(), *NewStateTag.ToString());

    ABNBaseAIController* AIController = Cast<ABNBaseAIController>(GetController());
    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            // 블랙보드에 상태를 기록하는 것은 여전히 중요합니다.
            BlackboardComp->SetValueAsName(BBKeys::State, NewStateTag.GetTagName());
        }
    }
}

bool ABNBaseMonster::HasStateTag(FGameplayTag StateTag) const
{
    if (!AbilitySystemComponent) return false;
    return AbilitySystemComponent->HasMatchingGameplayTag(StateTag);
}