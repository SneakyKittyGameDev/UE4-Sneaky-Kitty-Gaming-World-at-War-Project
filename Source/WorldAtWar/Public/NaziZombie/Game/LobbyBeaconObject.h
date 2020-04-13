// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "LobbyBeaconObject.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ALobbyBeaconObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()
public:
	ALobbyBeaconObject();

protected:
	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
	virtual AOnlineBeaconClient* SpawnBeaconActor(UNetConnection* ClientConnection) override;
};
