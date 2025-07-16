// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

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
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ABNBaseMonster::BeginPlay()
{
    Super::BeginPlay();

    // 데이터 에셋의 DormantStateTag를 초기 상태로 부여합니다.
    if (AbilitySystemComponent && StateDataAsset)
    {
        AbilitySystemComponent->AddReplicatedLooseGameplayTag(StateDataAsset->DormantStateTag);
        UE_LOG(LogTemp, Warning, TEXT("[%s] has entered %s State."), *GetName(), *StateDataAsset->DormantStateTag.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] does not have a valid StateDataAsset!"), *GetName());
    }
}

// ... (ActivateMonster, EnterIdleState 등 나머지 함수는 기존과 동일) ...
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
    AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(StateDataAsset->DormantStateTag);
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

    // 데이터 에셋에 정의된 모든 상태 태그를 제거 리스트에 추가합니다.
    FGameplayTagContainer TagsToRemove;
    TagsToRemove.AddTag(StateDataAsset->DormantStateTag);
    TagsToRemove.AddTag(StateDataAsset->IdleStateTag);
    TagsToRemove.AddTag(StateDataAsset->ChaseStateTag);
    TagsToRemove.AddTag(StateDataAsset->AttackStateTag);
    AbilitySystemComponent->RemoveReplicatedLooseGameplayTags(TagsToRemove);
    
    // 새로운 상태 태그를 추가합니다.
    AbilitySystemComponent->AddReplicatedLooseGameplayTag(NewStateTag);
    UE_LOG(LogTemp, Log, TEXT("[%s] transitioned to state: %s"), *GetName(), *NewStateTag.ToString());

    // 블랙보드의 값을 업데이트합니다.
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsName(TEXT("State"), NewStateTag.GetTagName());
        }
    }
}

bool ABNBaseMonster::HasStateTag(FGameplayTag StateTag) const
{
    if (!AbilitySystemComponent) return false;
    return AbilitySystemComponent->HasMatchingGameplayTag(StateTag);
}