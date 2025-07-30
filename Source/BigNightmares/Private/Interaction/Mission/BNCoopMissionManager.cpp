// BNCoopMissionManager.cpp

#include "Interaction/Mission/BNCoopMissionManager.h"
#include "Player/BNMonoCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LevelStreaming.h" // 레벨 스트리밍을 위해 필요

ABNCoopMissionManager::ABNCoopMissionManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    CurrentMissionState = ECoopMissionState::NotStarted;

    // 기본값 설정
    CoopLevelName = FName(TEXT("CoopGame")); // 에디터에서 설정한 서브레벨 이름
    CoopLevelStartPosition = FVector(10000.f, 10000.f, 100.f); // 서브레벨의 시작 위치
    ReturnPosition = FVector::ZeroVector; // 돌아올 위치
}

void ABNCoopMissionManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABNCoopMissionManager, MissionPlayers);
    DOREPLIFETIME(ABNCoopMissionManager, CurrentMissionState);
}

void ABNCoopMissionManager::StartMission(ABNMonoCharacter* Player1, ABNMonoCharacter* Player2)
{
    if (HasAuthority() && CurrentMissionState == ECoopMissionState::NotStarted && Player1 && Player2)
    {
        MissionPlayers.Add(Player1);
        MissionPlayers.Add(Player2);
        CurrentMissionState = ECoopMissionState::InProgress;
        
        AssignRoles();

        UE_LOG(LogTemp, Warning, TEXT("Co-op Mission Starting... Loading SubLevel: %s"), *CoopLevelName.ToString());

        // [핵심] 두 명의 플레이어에게만 서브레벨을 로드하고 순간이동시킵니다.
        for (ABNMonoCharacter* Player : MissionPlayers)
        {
            if (APlayerController* PC = Player->GetController<APlayerController>())
            {
                // 1. 해당 플레이어에게 서브레벨을 보이도록 로드합니다.
                FLatentActionInfo LatentInfo;
                UGameplayStatics::LoadStreamLevel(this, CoopLevelName, true, true, LatentInfo);

                // 2. 플레이어를 서브레벨의 시작 위치로 순간이동시킵니다.
                Player->TeleportTo(CoopLevelStartPosition, Player->GetActorRotation());
            }
        }
    }
}

void ABNCoopMissionManager::AssignRoles()
{
    // 역할 분배 로직은 동일
    if (MissionPlayers.Num() == 2)
    {
        if (FMath::RandBool()) 
        {
            MissionPlayers[0]->PlayerRole = EPlayerRole::FlashlightHolder;
            MissionPlayers[1]->PlayerRole = EPlayerRole::KeyHolder;
        } 
        else 
        {
            MissionPlayers[0]->PlayerRole = EPlayerRole::KeyHolder;
            MissionPlayers[1]->PlayerRole = EPlayerRole::FlashlightHolder;
        }
    }
}

void ABNCoopMissionManager::MissionComplete()
{
    if (HasAuthority() && CurrentMissionState == ECoopMissionState::InProgress)
    {
        CurrentMissionState = ECoopMissionState::Completed;
        Multicast_ShowClearUI();
        UE_LOG(LogTemp, Log, TEXT("Co-op Mission Completed!"));
        EndMission();
    }
}

void ABNCoopMissionManager::AllPlayersKilledByThinman(AActor* Killer)
{
     if (HasAuthority() && CurrentMissionState == ECoopMissionState::InProgress)
    {
        CurrentMissionState = ECoopMissionState::Failed;
        UE_LOG(LogTemp, Warning, TEXT("Co-op Mission Failed: All players killed."));
        // 참고: HandleImmediateDeath가 이미 호출될 것이므로 여기서는 사망 처리를 하지 않습니다.
        EndMission();
    }
}

void ABNCoopMissionManager::EndMission()
{
    UE_LOG(LogTemp, Warning, TEXT("Co-op Mission Ending... Unloading SubLevel: %s"), *CoopLevelName.ToString());

    // [핵심] 두 명의 플레이어에게서 서브레벨을 언로드하고 원래 위치로 돌려보냅니다.
    for (ABNMonoCharacter* Player : MissionPlayers)
    {
        if (APlayerController* PC = Player->GetController<APlayerController>())
        {
            // 1. 해당 플레이어에게 서브레벨을 보이지 않도록 언로드합니다.
            FLatentActionInfo LatentInfo;
            UGameplayStatics::UnloadStreamLevel(this, CoopLevelName, LatentInfo, false);

            // 2. 플레이어를 원래 위치로 돌려보냅니다.
            Player->TeleportTo(ReturnPosition, Player->GetActorRotation());

            // 3. 미션 중에 부여했던 역할을 초기화합니다.
            Player->PlayerRole = EPlayerRole::None;
        }
    }
    MissionPlayers.Empty();
}

void ABNCoopMissionManager::Multicast_ShowClearUI_Implementation()
{
    // UI 표시 로직은 동일
    if (MissionClearWidgetClass == nullptr) return;

    for (const auto& Player : MissionPlayers)
    {
        if(Player && Player->IsLocallyControlled())
        {
            if(APlayerController* PC = Player->GetController<APlayerController>())
            {
                 UUserWidget* ClearWidget = CreateWidget<UUserWidget>(PC, MissionClearWidgetClass);
                if (ClearWidget)
                {
                    ClearWidget->AddToViewport();
                }
            }
        }
    }
}
