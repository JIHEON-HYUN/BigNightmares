#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BNMonsterSpawner.generated.h"

class ATargetPoint;
class ABNBaseMonster;
class ABNMonsterActivationTrigger;

UCLASS()
class BIGNIGHTMARES_API ABNMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABNMonsterSpawner();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<ABNBaseMonster> MonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<ATargetPoint*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (ClampMin = "1"))
	int32 NumberOfMonstersToSpawn = 3;

	// 활성화 트리거 참조 (생성된 몬스터들을 등록하기 위함)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	ABNMonsterActivationTrigger* ActivationTrigger;
};
