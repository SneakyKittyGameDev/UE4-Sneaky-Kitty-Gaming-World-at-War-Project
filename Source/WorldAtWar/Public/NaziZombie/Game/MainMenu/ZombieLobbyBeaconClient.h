// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyBeaconClient.h"
#include "ZombieLobbyBeaconClient.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AZombieLobbyBeaconClient : public ALobbyBeaconClient
{
	GENERATED_BODY()
public:
	AZombieLobbyBeaconClient();

protected:
	class AZombieLobbyBeaconState* BeaconState;

	virtual void BeginPlay() override;

	virtual bool ServerLoginPlayer_Validate(const FString& InSessionId,
		const FUniqueNetIdRepl& InUniqueId,
		const FString& UrlString) override;

	virtual void ServerLoginPlayer_Implementation(const FString& InSessionId,
		const FUniqueNetIdRepl& InUniqueId,
		const FString& UrlString) override;

public:
	UFUNCTION(BlueprintCallable)
		bool ConnectToServer(const FString& Address);
};
