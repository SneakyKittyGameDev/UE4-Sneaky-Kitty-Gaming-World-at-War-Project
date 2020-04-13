// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "LobbyBeaconClient.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API ALobbyBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()
public:
	ALobbyBeaconClient();

protected:
	virtual void OnFailure() override;

public:
	UFUNCTION(BlueprintCallable)
		bool StartBeacon(FString Address);

	UFUNCTION(Client, Reliable)
		void ClientHasConnected(const FString& ServerMessage);
	void ClientHasConnected_Implementation(const FString& ServerMessage);
};
