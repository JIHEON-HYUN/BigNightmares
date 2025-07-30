// BNCoopMissionManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/BNPlayerRole.h"
#include "BNCoopMissionManager.generated.h"

class ABNMonoCharacter;
class UUserWidget;

UENUM(BlueprintType)
enum class ECoopMissionState : uint8
{
    NotStarted,
    InProgress,
    Completed,
    Failed
};

UCLASS()
class BIGNIGHTMARES_API ABNCoopMissionManager : public AActor
{
    GENERATED_BODY()

public:
    ABNCoopMissionManager();

    void StartMission(ABNMonoCharacter* Player1, ABNMonoCharacter* Player2);
    void MissionComplete();
    void AllPlayersKilledByThinman(AActor* Killer);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // [추가] 스트리밍할 서브레벨의 이름입니다. 에디터에서 설정할 수 있습니다.
    UPROPERTY(EditAnywhere, Category = "Coop Mission|Level Streaming")
    FName CoopLevelName;

    // [추가] 플레이어를 서브레벨의 어느 위치로 보낼지 결정합니다.
    UPROPERTY(EditAnywhere, Category = "Coop Mission|Level Streaming")
    FVector CoopLevelStartPosition;
    
    // [추가] 플레이어를 미션 종료 후 어디로 보낼지 결정합니다.
    UPROPERTY(EditAnywhere, Category = "Coop Mission|Level Streaming")
    FVector ReturnPosition;

private:
    UPROPERTY(Replicated)
    TArray<TObjectPtr<ABNMonoCharacter>> MissionPlayers;

    UPROPERTY(Replicated)
    ECoopMissionState CurrentMissionState;

    void AssignRoles();

    // [추가] 미션 종료 시 공통으로 처리할 함수
    void EndMission();

    UPROPERTY(EditDefaultsOnly, Category = "Coop Mission|UI")
    TSubclassOf<UUserWidget> MissionClearWidgetClass;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ShowClearUI();
};
