// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NaziZombieMainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ANaziZombieMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANaziZombieMainMenuGameMode();

protected:
	UFUNCTION(BlueprintCallable)
		bool CreateHostBeacon();

	class ANaziZombieBeaconHostObject* HostObject;

	class AOnlineBeaconHost* Host;

	UFUNCTION(BlueprintCallable)
		class ANaziZombieBeaconHostObject* GetBeaconHost();
};
