// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "DataAsset/DataAsset_State_Monster.h"
#include "Monster/BNBaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BNBlackboardKeys.h"

// 생성자
ABNBaseMonster::ABNBaseMonster()
{
    // 틱 비활성화
    PrimaryActorTick.bCanEverTick = false;

    // 어빌리티 시스템 컴포넌트 생성
    AbilitySystemComponent = CreateDefaultSubobject<UBNBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    // 리플리케이션 설정
    AbilitySystemComponent->SetIsReplicated(true);
    // 리플리케이션 모드 설정
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

// 어빌리티 시스템 컴포넌트 Getter
UAbilitySystemComponent* ABNBaseMonster::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

// AI 컨트롤러 빙의 시 호출
void ABNBaseMonster::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    // 어빌리티 시스템 컴포넌트 유효성 확인
    if (AbilitySystemComponent)
    {
        // 어빌리티 액터 정보 초기화
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

// 게임 시작 또는 스폰 시 호출
void ABNBaseMonster::BeginPlay()
{
    Super::BeginPlay();

    // 컴포넌트 및 데이터 에셋 유효성 확인
    if (AbilitySystemComponent && StateDataAsset)
    {
        // 최초 상태를 'Dormant'(휴면)로 설정
        TransitionToState(StateDataAsset->DormantStateTag);
    }
    else
    {
        // 데이터 에셋 누락 에러 로그
        UE_LOG(LogTemp, Error, TEXT("[%s] does not have a valid StateDataAsset!"), *GetName());
    }
}

// 몬스터 활성화 함수
void ABNBaseMonster::ActivateMonster()
{
    // 유효성 확인 및 예외 처리
    if (!AbilitySystemComponent || !StateDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("BNBaseMonster::ActivateMonster - AbilitySystemComponent or StateDataAsset is NULL!"));
        return;
    }
    
    // 이미 활성화 상태인지 확인
    bool bIsAlreadyActive = HasStateTag(StateDataAsset->IdleStateTag) || HasStateTag(StateDataAsset->ChaseStateTag) || HasStateTag(StateDataAsset->AttackStateTag);

    // 이미 활성화 시 함수 종료
    if (bIsAlreadyActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("BNBaseMonster::ActivateMonster - Monster is already active."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[%s] is being activated. Removing Dormant State."), *GetName());

    // 대기 상태로 전환
    EnterIdleState();
}

// 대기 상태 진입
void ABNBaseMonster::EnterIdleState()
{
    if (!StateDataAsset) return;
    TransitionToState(StateDataAsset->IdleStateTag);
}

// 추격 상태 진입
void ABNBaseMonster::EnterChasingState()
{
    if (!StateDataAsset) return;
    TransitionToState(StateDataAsset->ChaseStateTag);
}

// 공격 상태 진입
void ABNBaseMonster::EnterAttackingState()
{
    if (!StateDataAsset) return;
    TransitionToState(StateDataAsset->AttackStateTag);
}

// 상태 변경 처리 함수
void ABNBaseMonster::TransitionToState(const FGameplayTag& NewStateTag)
{
    // 유효성 확인
    if (!AbilitySystemComponent || !StateDataAsset || !NewStateTag.IsValid()) return;

    // 공격 진행 중 다른 상태로 변경 방지
    if (AbilitySystemComponent->HasMatchingGameplayTag(StateDataAsset->AttackInProgressTag))
    {
        // 단, 다른 공격 상태로의 전환은 허용 (콤보 등)
        if (NewStateTag != StateDataAsset->AttackStateTag)
        {
            // 상태 변경 중단
            return;
        }
    }

    // 제거할 태그 컨테이너 생성 후 기존 상태 태그 추가
    FGameplayTagContainer TagsToRemove;
    TagsToRemove.AddTag(StateDataAsset->DormantStateTag);
    TagsToRemove.AddTag(StateDataAsset->IdleStateTag);
    TagsToRemove.AddTag(StateDataAsset->ChaseStateTag);
    TagsToRemove.AddTag(StateDataAsset->AttackStateTag);
    // 모든 기존 상태 태그 일괄 제거
    AbilitySystemComponent->RemoveReplicatedLooseGameplayTags(TagsToRemove);

    // 새로운 상태 태그 추가
    AbilitySystemComponent->AddReplicatedLooseGameplayTag(NewStateTag);
    
    // 현재 상태 멤버 변수 업데이트
    this->CurrentStateTag = NewStateTag;

    UE_LOG(LogTemp, Log, TEXT("[%s] transitioned to state: %s"), *GetName(), *NewStateTag.ToString());

    // AI 컨트롤러 유효성 확인
    ABNBaseAIController* AIController = Cast<ABNBaseAIController>(GetController());
    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
        if (BlackboardComp)
        {
            // 블랙보드의 상태 값 업데이트
            BlackboardComp->SetValueAsName(BBKeys::State, NewStateTag.GetTagName());
        }
    }
}

// 특정 상태 태그 보유 여부 확인
bool ABNBaseMonster::HasStateTag(FGameplayTag StateTag) const
{
    // 컴포넌트 유효성 확인
    if (!AbilitySystemComponent) return false;
    // 해당 태그 보유 여부 반환
    return AbilitySystemComponent->HasMatchingGameplayTag(StateTag);
}