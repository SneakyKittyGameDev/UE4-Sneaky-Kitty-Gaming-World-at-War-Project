//Copyright 2020, Cody Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NaziZombieGameState.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ANaziZombieGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ANaziZombieGameState();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		uint16 RoundNumber;//set to replicate
	uint8 ZombiesOnMap;//set to replicate
	uint16 TotalZombiesRemaining;//set to replicate
	float ZombieHealth;

	UPROPERTY(Replicated)
		bool bIgnoreAmmo;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		TArray<TSubclassOf<class AWeaponBase>> GiveAllWeaponsClasses;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Exec)
		void sf_use_ignoreAmmo(bool IgnoreAmmo);
	UFUNCTION(Exec)
		void GiveAll();
	
public:
	uint16 GetRoundNumber();
	void IncrementRoundNumber();
	void SetTotalZombiesRemaining(const uint16& ZombieCount);
	uint16 GetTotalZombiesRemaining();
	void ZombieKilled();
	uint8 GetZombiesOnMap();
	void ZombieSpawned();
	float GetZombieHealth();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool CheatIgnoreAmmo() {return bIgnoreAmmo;}
};