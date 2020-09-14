//Copyright 2020, Cody Dawe, All rights reserved
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

	//zombies per round levels 1-5 is ((round number * 0.2) * max zombies on map) = zombies per round
	//example round 3 with 2 players = (3 * 0.2) * 30 = 18 zombies on round 3 with 2 players
	
	//zombies per round is multiplier * max zombies on map. multiplier is round number * 0.15.
	//example: round 10 with 3 players is (multiplier = 10*0.15) = 1.5 * 36(24+playercount*6) = 54 TOTAL zombies on round 10 with 3 players.

	uint32 ZombiesRemaining;

protected:
	void CalculateZombieCount();
	void SpawnZombie();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void SetSpawnPoints();

public:
	void NewZoneActive(uint8 ZoneNumber);
	void ZombieKilled();
};
