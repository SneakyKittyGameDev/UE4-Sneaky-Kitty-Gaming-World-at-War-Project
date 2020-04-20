// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameInstanceBase.h"
#include "LobbyBeaconClient.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLobbyInfoDelegate, FMapInfo, LobbyInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerListDelegate, const TArray<FString>&, PlayerList);

UCLASS()
class WORLDATWAR_API ALobbyBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()
public:
	ALobbyBeaconClient();

protected:
	virtual void OnFailure() override;

	UPROPERTY(BlueprintAssignable)
		FLobbyInfoDelegate LobbyInfo;

	UPROPERTY(BlueprintAssignable)
		FPlayerListDelegate PlayerList;

	UPROPERTY(ReplicatedUsing = OnRep_LobbyInfoChanged)
		FMapInfo MapInfo;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerInfoChanged)
		TArray<FString> PlayerNames;

	UFUNCTION()
		void OnRep_LobbyInfoChanged();

	UFUNCTION()
		void OnRep_PlayerInfoChanged();

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetClientInfo(const FString& PlayerName);
	bool Server_SetClientInfo_Validate(const FString& PlayerName);
	void Server_SetClientInfo_Implementation(const FString& PlayerName);

public:
	UFUNCTION(BlueprintCallable)
		bool StartBeacon(FString Address);

	UFUNCTION(Client, Reliable)
		void ClientHasConnected(const FString& ServerMessage);
	void ClientHasConnected_Implementation(const FString& ServerMessage);

	void SetLobbyInfo(FMapInfo NewMapInfo, TArray<FString> NewPlayerNames);
};
