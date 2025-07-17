// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Triggers/BNMonsterActivationTrigger.h"
#include "Components/BoxComponent.h"
#include "Character/BNBaseMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h" // [추가] GetGameState를 위해 필요합니다.
#include "TimerManager.h" // [추가] 타이머 사용을 위해 필요합니다.
// #include "YourGame/BNGameState.h" // [참고] 나중에 실제 게임 스테이트 클래스를 포함해야 합니다.

ABNMonsterActivationTrigger::ABNMonsterActivationTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}

void ABNMonsterActivationTrigger::BeginPlay()
{
	Super::BeginPlay();

	// 에디터에서 선택한 활성화 조건에 따라 다른 초기화 로직을 수행합니다.
	switch (ActivationCondition)
	{
	case EActivationCondition::OnPlayerOverlap:
		// 플레이어 진입 조건일 때만 오버랩 이벤트를 바인딩합니다.
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABNMonsterActivationTrigger::OnOverlapBegin);
		break;

	case EActivationCondition::OnTimeElapsed:
		{
			// 타이머 조건일 때, 지정된 시간 후에 OnTimerFinished 함수를 호출하는 타이머를 설정합니다.
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABNMonsterActivationTrigger::OnTimerFinished, TimeToWait, false);
		}
		break;

	case EActivationCondition::OnMissionCount:
		// 미션 클리어 조건은 다른 곳(예: 게임 스테이트)에서 TryActivate()를 호출해주어야 하므로, 여기서는 아무것도 하지 않습니다.
		break;

	case EActivationCondition::OnExternalEvent:
		// 외부 이벤트 조건도 마찬가지로, 다른 곳에서 TryActivate()를 호출해주어야 합니다.
		break;
	}
}

void ABNMonsterActivationTrigger::TryActivate()
{
	// 한 번만 작동하도록 설정되었고 이미 작동했다면, 아무것도 하지 않습니다.
	if (bTriggerOnce && bHasBeenTriggered)
	{
		return;
	}

	bool bCanActivate = false;

	// 현재 활성화 조건에 따라 발동 여부를 결정합니다.
	switch (ActivationCondition)
	{
		case EActivationCondition::OnPlayerOverlap:
		case EActivationCondition::OnExternalEvent:
			// 이 조건들은 외부 호출에 의해 즉시 활성화됩니다.
			bCanActivate = true;
			break;

		case EActivationCondition::OnTimeElapsed:
			// 타이머는 SetTimer에 의해 직접 관리되므로, 이 함수에서는 처리하지 않습니다.
			break;

		case EActivationCondition::OnMissionCount:
			{
				// [중요] 실제 게임 스테이트에서 현재 미션 클리어 갯수를 가져와야 합니다.
				// 아래는 예시 코드이며, 실제 프로젝트에 맞게 수정해야 합니다.
				// ABNGameState* MyGameState = GetWorld()->GetGameState<ABNGameState>();
				// if (MyGameState && MyGameState->GetClearedMissionCount() >= RequiredMissionCount)
				// {
				//	 bCanActivate = true;
				// }
				
				// 임시로 항상 활성화되도록 설정 (테스트용)
				UE_LOG(LogTemp, Warning, TEXT("Checking Mission Count. Implement actual logic in GameState!"));
				bCanActivate = true; 
			}
			break;
	}

	if (bCanActivate)
	{
		ActivateMonsters();
	}
}

void ABNMonsterActivationTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 오버랩된 액터가 플레이어 캐릭터인지 확인합니다.
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
	{
		// 활성화를 시도합니다.
		TryActivate();
	}
}

void ABNMonsterActivationTrigger::OnTimerFinished()
{
	UE_LOG(LogTemp, Log, TEXT("Timer finished. Activating monsters."));
	TryActivate();
}


void ABNMonsterActivationTrigger::ActivateMonsters()
{
	// 이미 작동했다면 중복 실행을 방지합니다.
	if (bTriggerOnce && bHasBeenTriggered)
	{
		return;
	}

	// 지정된 모든 몬스터를 순회하며 활성화시킵니다.
	for (ABNBaseMonster* Monster : MonstersToActivate)
	{
		if (Monster)
		{
			Monster->ActivateMonster();
		}
	}

	// 트리거가 작동했음을 기록합니다.
	bHasBeenTriggered = true;
}
