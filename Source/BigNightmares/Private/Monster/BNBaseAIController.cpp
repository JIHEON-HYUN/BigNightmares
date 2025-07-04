// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BNBaseAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "GenericTeamAgentInterface.h"

ABNBaseAIController::ABNBaseAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(this))
	{
		TeamAgent->SetGenericTeamId(FGenericTeamId(1));
	}
}

void ABNBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetupPerception();
}

void ABNBaseAIController::SetupPerception()
{
}

FGenericTeamId ABNBaseAIController::GetGenericTeamId() const
{
	FGenericTeamId CurrentTeamID = Super::GetGenericTeamId();
	UE_LOG(LogTemp, Warning, TEXT("[AI] GetGenericTeamId() 호출됨. ID: %d"), CurrentTeamID.GetId());
	return CurrentTeamID;

}
