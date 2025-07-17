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
    
    if (AbilitySystemComponent)
    {
        // [복원] 어빌리티 시스템을 초기화하는 코드만 남깁니다.
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ABNBaseMonster::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent && StateDataAsset)
    {
        // [수정] 데이터 에셋에서 '초기 상태' 태그를 가져옵니다. (보통 Dormant)
        const FGameplayTag InitialStateTag = StateDataAsset->DormantStateTag;
        
        // 초기 상태 태그를 부여합니다.
        AbilitySystemComponent->AddReplicatedLooseGameplayTag(InitialStateTag);
		
        // [수정] 현재 부여된 '초기 상태'를 로그로 출력하도록 변경하여 명확성을 높입니다.
        UE_LOG(LogTemp, Warning, TEXT("[%s] has entered initial state: %s"), *GetName(), *InitialStateTag.ToString());

        // AI 컨트롤러에게도 초기 상태를 알려줍니다.
        if (ABNBaseAIController* MyController = Cast<ABNBaseAIController>(GetController()))
        {
            MyController->SetInitialStateOnBlackboard(InitialStateTag.GetTagName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[%s] does not have a valid StateDataAsset!"), *GetName());
    }

    //ActivateMonster();
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
            BlackboardComp->SetValueAsName(BBKeys::State, NewStateTag.GetTagName());
        }
    }
}

bool ABNBaseMonster::HasStateTag(FGameplayTag StateTag) const
{
    if (!AbilitySystemComponent) return false;
    return AbilitySystemComponent->HasMatchingGameplayTag(StateTag);
}