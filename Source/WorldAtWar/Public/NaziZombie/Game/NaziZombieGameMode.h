// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NaziZombieGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ANaziZombieGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANaziZombieGameMode();
	
protected:
	class ANaziZombieGameState* ZombieGameState;

	bool bHasLoadedSpawnPoints;
	TArray<class ANaziZombiePlayerSpawnPoint*> PlayerSpawnPoints;

	TArray<class ANaziZombieZombieSpawnPoint*> ActiveZombieSpawnPoints;
	TArray<class ANaziZombieZombieSpawnPoint*> ZombieSpawnPoints;
	
	UPROPERTY(EditAnywhere, Category = "NaziZombieSettings")
		TSubclassOf<class ANaziZombieCharacter> PlayerClass;
	UPROPERTY(EditAnywhere, Category = "NaziZombieSettings")
		TSubclassOf<class AZombieBase> ZombieClass;

	FTimerHandle TZombieSpawnHandle;
	//max zombies on map is 24 + playercount*6

	//zombies per round is multiplier * max zombies on map. multiplier is round number * 0.15.
	//example: round 10 with 3 players is (multiplier = 10*0.15) = 1.5 * 36(24+playercount*6) = 54 TOTAL zombies on round 10 with 3 players.

	uint16 ZombiesRemaining;

protected:
	void CalculateZombieCount();
	void SpawnZombie();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetSpawnPoints();

public:
	void NewZoneActive(uint8 ZoneNumber);
};
