// Fill out your copyright notice in the Description page of Project Settings.

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
	UPROPERTY(EditAnywhere)
		uint16 RoundNumber;//set to replicate
	uint8 ZombiesOnMap;//set to replicate
	uint16 TotalZombiesRemaining;//set to replicate
	float ZombieHealth;

protected:
	virtual void BeginPlay() override;
	
public:
	uint16 GetRoundNumber();
	void IncrementRoundNumber();
	void SetTotalZombiesRemaining(const uint16& ZombieCount);
	uint16 GetTotalZombiesRemaining();
	void ZombieKilled();
	uint8 GetZombiesOnMap();
	void ZombieSpawned();
	float GetZombieHealth();
};