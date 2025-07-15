// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Monster/BNMonsterGameplayTags.h"
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
    // Super::PossessedBy를 호출하지 않는 것은 그대로 유지합니다.
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ABNBaseMonster::BeginPlay()
{
    Super::BeginPlay();

    // 게임이 시작되면, 몬스터의 초기 상태를 'Dormant'(비활성)으로 설정합니다.
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddReplicatedLooseGameplayTag(FBNMonsterGameplayTags::Get().Character_Monster_Dormant);
    }

    // [로그 추가] 현재 상태를 출력합니다.
    UE_LOG(LogTemp, Warning, TEXT("[%s] has entered Dormant State."), *GetName());
}

// ... (ActivateMonster, EnterIdleState 등 나머지 함수는 기존과 동일) ...
void ABNBaseMonster::ActivateMonster()
{
    UE_LOG(LogTemp, Warning, TEXT("BNBaseMonster::ActivateMonster - Base function called.")); // [로그 추가]

    // [수정됨] 필수 컴포넌트 유무만 확인합니다. 태그 유효성 검사는 더 이상 필요 없습니다.
    if (!AbilitySystemComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("BNBaseMonster::ActivateMonster - AbilitySystemComponent is NULL!")); // [로그 추가]
        return;
    }

    // [수정됨] 변수 이름을 'Tags'에서 'GameplayTags'로 변경하여 명확성을 높입니다.
    const FBNMonsterGameplayTags& GameplayTags = FBNMonsterGameplayTags::Get();
    if (AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Character_Monster_Active))
    {
        UE_LOG(LogTemp, Warning, TEXT("BNBaseMonster::ActivateMonster - Monster is already active.")); // [로그 추가]
        return;
    }

    // [로그 추가] 상태 변경 전 로그를 출력합니다.
    UE_LOG(LogTemp, Warning, TEXT("[%s] is being activated. Removing Dormant State and adding Active State."), *GetName());

    // [수정됨] 더 이상 존재하지 않는 DormantStateTag, ActiveStateTag 변수 대신,
    // FBNMonsterGameplayTags 싱글톤에서 직접 태그를 가져와 사용합니다.
    AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(GameplayTags.Character_Monster_Dormant);
    AbilitySystemComponent->AddReplicatedLooseGameplayTag(GameplayTags.Character_Monster_Active);

    // AI 로직을 시작하고, 초기 행동 상태를 'Idle'(대기)로 설정합니다.
    EnterIdleState();
}

void ABNBaseMonster::EnterIdleState()
{
    TransitionToState(FBNMonsterGameplayTags::Get().Character_Monster_Active_Idle);
}

void ABNBaseMonster::EnterChasingState()
{
    TransitionToState(FBNMonsterGameplayTags::Get().Character_Monster_Active_Chasing);
}

void ABNBaseMonster::EnterAttackingState()
{
    TransitionToState(FBNMonsterGameplayTags::Get().Character_Monster_Active_Attacking);
}

void ABNBaseMonster::TransitionToState(const FGameplayTag& NewStateTag)
{
    if (!AbilitySystemComponent || !NewStateTag.IsValid()) return;

    // [수정됨] 변수 이름을 'Tags'에서 'MonsterTags'로 변경하여 부모 클래스와의 충돌을 피합니다.
    const FBNMonsterGameplayTags& MonsterTags = FBNMonsterGameplayTags::Get();
    FGameplayTagContainer TagsToRemove;
    TagsToRemove.AddTag(MonsterTags.Character_Monster_Active_Idle);
    TagsToRemove.AddTag(MonsterTags.Character_Monster_Active_Chasing);
    TagsToRemove.AddTag(MonsterTags.Character_Monster_Active_Attacking);
    AbilitySystemComponent->RemoveReplicatedLooseGameplayTags(TagsToRemove);
    
    // 새로운 행동 상태 태그를 추가합니다.
    AbilitySystemComponent->AddReplicatedLooseGameplayTag(NewStateTag);

    // AI가 현재 상태를 알 수 있도록 블랙보드(Blackboard)의 값을 업데이트합니다.
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