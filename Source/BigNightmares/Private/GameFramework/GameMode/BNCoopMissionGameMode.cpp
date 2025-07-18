// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNCoopMissionGameMode.h"
#include "Player/BNMonoCharacter.h"
#include "Player/BNPlayerRole.h"
#include "Kismet/GameplayStatics.h"

void ABNCoopMissionGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer)
	{
		return;
	}

	// 새로운 플레이어를 목록에 추가합니다.
	LoggedInPlayers.Add(NewPlayer);

	// 정확히 2명의 플레이어가 접속했을 때만 역할을 할당합니다.
	if (LoggedInPlayers.Num() == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Two players have joined. Assigning roles randomly."));

		APlayerController* Player1Controller = LoggedInPlayers[0];
		APlayerController* Player2Controller = LoggedInPlayers[1];

		if (Player1Controller && Player2Controller)
		{
			ABNMonoCharacter* Player1Character = Cast<ABNMonoCharacter>(Player1Controller->GetPawn());
			ABNMonoCharacter* Player2Character = Cast<ABNMonoCharacter>(Player2Controller->GetPawn());

			if (Player1Character && Player2Character)
			{
				// 50% 확률로 역할을 랜덤하게 결정합니다.
				if (FMath::RandBool())
				{
					// 경우 1: 플레이어 1이 손전등, 플레이어 2가 열쇠
					Player1Character->PlayerRole = EPlayerRole::FlashlightHolder;
					Player2Character->PlayerRole = EPlayerRole::KeyHolder;
					UE_LOG(LogTemp, Warning, TEXT("Assigned FlashlightHolder to %s"), *Player1Character->GetName());
					UE_LOG(LogTemp, Warning, TEXT("Assigned KeyHolder to %s"), *Player2Character->GetName());
				}
				else
				{
					// 경우 2: 플레이어 1이 열쇠, 플레이어 2가 손전등
					Player1Character->PlayerRole = EPlayerRole::KeyHolder;
					Player2Character->PlayerRole = EPlayerRole::FlashlightHolder;
					UE_LOG(LogTemp, Warning, TEXT("Assigned KeyHolder to %s"), *Player1Character->GetName());
					UE_LOG(LogTemp, Warning, TEXT("Assigned FlashlightHolder to %s"), *Player2Character->GetName());
				}
			}
		}
	}
	else if (LoggedInPlayers.Num() > 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("A third player tried to join. The game is full."));
	}
}