// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZombieMainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AZombieMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AZombieMainMenuGameMode();

protected:
	virtual void BeginPlay() override;

	class AZombieLobbyBeaconHost* HostBeacon;

public:
	UFUNCTION(BlueprintCallable)
		bool SetupLobby();
};
