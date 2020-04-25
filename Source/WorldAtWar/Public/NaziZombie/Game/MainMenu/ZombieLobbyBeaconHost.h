// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyBeaconHost.h"
#include "ZombieLobbyBeaconHost.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AZombieLobbyBeaconHost : public ALobbyBeaconHost
{
	GENERATED_BODY()
public:
	AZombieLobbyBeaconHost();

protected:
	virtual void PostLogin(ALobbyBeaconClient* ClientActor) override;

	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;

	virtual ALobbyBeaconPlayerState* HandlePlayerLogin(ALobbyBeaconClient* ClientActor, const FUniqueNetIdRepl& InUniqueId, const FString& Options) override;
};
