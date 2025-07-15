// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BNBaseMonster.h"
#include "Abilities/BNBaseAbilitySystemComponent.h"
#include "Monster/BNMonsterGameplayTags.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

ABNBaseMonster::ABNBaseMonster()
{
    PrimaryActorTick.bCanEverTick = false;

    // 어빌리티 시스템 컴포넌트를 생성
    AbilitySystemComponent = CreateDefaultSubobject<UBNBaseAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // AI 감지 컴포넌트 생성
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

UAbilitySystemComponent* ABNBaseMonster::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ABNBaseMonster::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // GAS 및 Perception 초기화
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
    if (PerceptionComponent)
    {
        PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABNBaseMonster::OnTargetPerceptionUpdated);
    }
    
    // TODO: 이 곳에서 몬스터의 기본 어빌리티(공격, 스킬 등)를 부여하는 로직을 추가할 수 있습니다.
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
    StartAILogic();
    EnterIdleState();
}

void ABNBaseMonster::EnterIdleState()
{
    // [수정됨] FBNMonsterGameplayTags 싱글톤에서 'Idle' 태그를 가져와 상태를 전환합니다.
    TransitionToState(FBNMonsterGameplayTags::Get().Character_Monster_Active_Idle);
}

void ABNBaseMonster::EnterChasingState()
{
    // [수정됨] FBNMonsterGameplayTags 싱글톤에서 'Chasing' 태그를 가져와 상태를 전환합니다.
    TransitionToState(FBNMonsterGameplayTags::Get().Character_Monster_Active_Chasing);
}

void ABNBaseMonster::EnterAttackingState()
{
    // [수정됨] FBNMonsterGameplayTags 싱글톤에서 'Attacking' 태그를 가져와 상태를 전환합니다.
    TransitionToState(FBNMonsterGameplayTags::Get().Character_Monster_Active_Attacking);
}

void ABNBaseMonster::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (!AIController || !Actor) return;

    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp) return;

    // 감지에 성공했는지 여부를 확인합니다.
    if (Stimulus.WasSuccessfullySensed())
    {
        // 감지한 액터를 블랙보드의 'TargetActor' 키에 저장합니다.
        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        // TODO: 이 곳에서 바로 추격 상태로 전환할 수도 있습니다.
        // EnterChasingState(); 
    }
    else
    {
        // 감지했던 액터를 놓쳤다면, 블랙보드의 'TargetActor'를 비웁니다.
        BlackboardComp->ClearValue(TEXT("TargetActor"));
        // TODO: 이 곳에서 다시 대기 상태로 전환할 수 있습니다.
        // EnterIdleState();
    }
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
            // 블랙보드의 'State' 키(Name 타입)에 현재 상태 태그의 이름을 저장합니다.
            BlackboardComp->SetValueAsName(TEXT("State"), NewStateTag.GetTagName());
        }
    }
}

bool ABNBaseMonster::HasStateTag(FGameplayTag StateTag) const
{
    if (!AbilitySystemComponent || !StateTag.IsValid()) return false;
    return AbilitySystemComponent->HasMatchingGameplayTag(StateTag);
}

void ABNBaseMonster::StartAILogic()
{
    // [수정됨] FBNMonsterGameplayTags 싱글톤을 이용해 태그를 직접 비교합니다.
    if (HasStateTag(FBNMonsterGameplayTags::Get().Character_Monster_Dormant))
    {
        return;
    }
    
    AAIController* AIController = Cast<AAIController>(GetController());
    
    // 컨트롤러와 행동 트리 에셋이 모두 유효할 때, 행동 트리를 실행시킵니다.
    if (AIController && BehaviorTree)
    {
        AIController->RunBehaviorTree(BehaviorTree);
    }
}