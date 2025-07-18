// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Triggers/BNMonsterActivationTrigger.h"
#include "Components/BoxComponent.h"
#include "Character/BNBaseMonster.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
// #include "YourGame/BNGameState.h"

// 생성자
ABNMonsterActivationTrigger::ABNMonsterActivationTrigger()
{
	// 틱 비활성화
	PrimaryActorTick.bCanEverTick = false;
	// 트리거 볼륨 컴포넌트 생성
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	// 루트 컴포넌트로 설정
	SetRootComponent(TriggerVolume);
	// 콜리전 프로필 설정
	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
}

// 액터 생성 시 최초 실행 함수
void ABNMonsterActivationTrigger::BeginPlay()
{
	Super::BeginPlay();

	// 활성화 조건에 따른 초기화
	switch (ActivationCondition)
	{
	case EActivationCondition::OnPlayerOverlap:
		// 플레이어 진입 조건: 오버랩 이벤트 함수 바인딩
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ABNMonsterActivationTrigger::OnOverlapBegin);
		break;

	case EActivationCondition::OnTimeElapsed:
		{
			// 시간 경과 조건: 타이머 설정
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABNMonsterActivationTrigger::OnTimerFinished, TimeToWait, false);
		}
		break;

	case EActivationCondition::OnMissionCount:
		// 미션 카운트 조건: 외부에서 호출되므로 초기화 없음
		break;

	case EActivationCondition::OnExternalEvent:
		// 외부 이벤트 조건: 외부에서 호출되므로 초기화 없음
		break;
	}
}

// 활성화를 시도하는 함수
void ABNMonsterActivationTrigger::TryActivate()
{
	// 중복 발동 방지
	if (bTriggerOnce && bHasBeenTriggered)
	{
		return;
	}

	// 활성화 가능 여부 변수
	bool bCanActivate = false;

	// 조건에 따른 활성화 가능 여부 확인
	switch (ActivationCondition)
	{
		case EActivationCondition::OnPlayerOverlap:
		case EActivationCondition::OnExternalEvent:
			// 플레이어 진입 또는 외부 이벤트는 즉시 활성화
			bCanActivate = true;
			break;

		case EActivationCondition::OnTimeElapsed:
			// 타이머 조건은 OnTimerFinished에서 직접 처리
			break;

		case EActivationCondition::OnMissionCount:
			{
				// TODO: 실제 게임 스테이트에서 미션 카운트 확인 필요
				// ABNGameState* MyGameState = GetWorld()->GetGameState<ABNGameState>();
				// if (MyGameState && MyGameState->GetClearedMissionCount() >= RequiredMissionCount)
				// {
				//	 bCanActivate = true;
				// }
				
				// 임시 테스트용 코드
				UE_LOG(LogTemp, Warning, TEXT("임시 코드: 미션 카운트 조건을 만족한 것으로 처리합니다."));
				bCanActivate = true; 
			}
			break;
	}

	// 활성화 가능 시
	if (bCanActivate)
	{
		// 몬스터 활성화 함수 호출
		ActivateMonsters();
	}
}

// 오버랩(진입) 이벤트 발생 시 호출될 함수
void ABNMonsterActivationTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 진입한 액터를 플레이어 캐릭터로 Cast
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	// 플레이어 컨트롤 확인
	if (PlayerCharacter && PlayerCharacter->IsPlayerControlled())
	{
		// 활성화 시도 함수 호출
		TryActivate();
	}
}

// 타이머 만료 시 호출될 함수
void ABNMonsterActivationTrigger::OnTimerFinished()
{
	UE_LOG(LogTemp, Log, TEXT("타이머 완료. 몬스터를 활성화합니다."));
	// 활성화 시도 함수 호출
	TryActivate();
}

// 몬스터 활성화 로직 함수
void ABNMonsterActivationTrigger::ActivateMonsters()
{
	// 중복 발동 방지
	if (bTriggerOnce && bHasBeenTriggered)
	{
		return;
	}

	// 모든 몬스터 순회
	for (ABNBaseMonster* Monster : MonstersToActivate)
	{
		// 몬스터 유효성 확인
		if (Monster)
		{
			// 몬스터 활성화 함수 호출
			Monster->ActivateMonster();
		}
	}

	// 발동 완료 상태로 변경
	bHasBeenTriggered = true;
}