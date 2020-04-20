// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameInstanceBase.h"
#include "LobbyBeaconObject.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHostPlayerListDelegate, const TArray<FString>&, HostPlayerList);

UCLASS()
class WORLDATWAR_API ALobbyBeaconObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()
public:
	ALobbyBeaconObject();

protected:
	FMapInfo MapInfo;

	UPROPERTY(BlueprintAssignable)
		FHostPlayerListDelegate HostPlayerList;

	TArray<FString> PlayerNames;

protected:
	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
	virtual AOnlineBeaconClient* SpawnBeaconActor(UNetConnection* ClientConnection) override;

	void UpdateClients();

	virtual void BeginPlay() override;
	void AddHostName();

public:
	UFUNCTION(BlueprintCallable)
		void SetMapInfo(FMapInfo NewMapInfo);

	void SetNewPlayer(const FString& PlayerName);
};
