// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "NaziZombieZombieSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ANaziZombieZombieSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	ANaziZombieZombieSpawnPoint();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class ABarricade* LinkedBarricade;

	uint8 Zone;
	bool bIsActive;

	virtual void BeginPlay() override;

public:
	class ABarricade* GetLinkedBarricade();
	uint8 GetZone();
	void SetZone(uint8 ZoneNumber);
	bool IsActive();
	void Activate();
};
