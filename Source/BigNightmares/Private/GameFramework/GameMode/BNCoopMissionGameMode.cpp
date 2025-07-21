// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/GameMode/BNCoopMissionGameMode.h"
#include "Player/BNMonoCharacter.h"
#include "Player/BNPlayerRole.h"
#include "Kismet/GameplayStatics.h"

ABNCoopMissionGameMode::ABNCoopMissionGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABNCoopMissionGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!NewPlayer)
	{
		return;
	}

	LoggedInPlayers.AddUnique(NewPlayer);
	
	if (LoggedInPlayers.Num() > 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("A third player tried to join. The game is full."));
	}
}

void ABNCoopMissionGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 플레이어가 2명이 아니라면 아무것도 하지 않습니다.
	if (LoggedInPlayers.Num() != 2)
	{
		return;
	}

	APlayerController* Player1Controller = LoggedInPlayers[0];
	APlayerController* Player2Controller = LoggedInPlayers[1];

	if (Player1Controller && Player2Controller)
	{
		ABNMonoCharacter* Player1Character = Cast<ABNMonoCharacter>(Player1Controller->GetPawn());
		ABNMonoCharacter* Player2Character = Cast<ABNMonoCharacter>(Player2Controller->GetPawn());

		// [핵심 해결책] 두 플레이어의 캐릭터가 모두 존재하고,
		// 두 캐릭터의 역할이 모두 'None'일 때만 역할을 할당합니다.
		if (Player1Character && Player2Character && 
			Player1Character->PlayerRole == EPlayerRole::None && 
			Player2Character->PlayerRole == EPlayerRole::None)
		{
			UE_LOG(LogTemp, Error, TEXT("======== Pawns are ready and have no roles! Assigning roles now. This will only happen ONCE. ========"));

			if (FMath::RandBool())
			{
				Player1Character->PlayerRole = EPlayerRole::FlashlightHolder;
				Player2Character->PlayerRole = EPlayerRole::KeyHolder;
				UE_LOG(LogTemp, Warning, TEXT("Assigned FlashlightHolder to %s"), *Player1Character->GetName());
				UE_LOG(LogTemp, Warning, TEXT("Assigned KeyHolder to %s"), *Player2Character->GetName());
			}
			else
			{
				Player1Character->PlayerRole = EPlayerRole::KeyHolder;
				Player2Character->PlayerRole = EPlayerRole::FlashlightHolder;
				UE_LOG(LogTemp, Warning, TEXT("Assigned KeyHolder to %s"), *Player1Character->GetName());
				UE_LOG(LogTemp, Warning, TEXT("Assigned FlashlightHolder to %s"), *Player2Character->GetName());
			}
		}
	}
}
