// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconObject.h"
#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconClient.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"

ALobbyBeaconObject::ALobbyBeaconObject()
{
	ClientBeaconActorClass = ALobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ALobbyBeaconObject::BeginPlay()
{
	if (APlayerController* PController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerState* PState = PController->PlayerState)
		{
			PlayerNames.Add(PState->GetPlayerName());
			
			FTimerHandle TempTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TempTimerHandle, this, &ALobbyBeaconObject::AddHostName, 0.5f, false);
		}
	}
}

void ALobbyBeaconObject::AddHostName()
{
	HostPlayerList.Broadcast(PlayerNames);
}

void ALobbyBeaconObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (ALobbyBeaconClient* ClientBeacon = Cast<ALobbyBeaconClient>(NewClientActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnClientConnected Cast Successful"));
		ClientBeacon->ClientHasConnected("Hello New Client");
		ClientBeacon->SetLobbyInfo(MapInfo, PlayerNames);
	}
}

AOnlineBeaconClient* ALobbyBeaconObject::SpawnBeaconActor(UNetConnection* ClientConnection)
{
	return Super::SpawnBeaconActor(ClientConnection);
}

void ALobbyBeaconObject::UpdateClients()
{
	for (AOnlineBeaconClient* Beacon : ClientActors)
	{
		if (ALobbyBeaconClient* ClientBeacon = Cast<ALobbyBeaconClient>(Beacon))
		{
			ClientBeacon->SetLobbyInfo(MapInfo, PlayerNames);
		}
	}
}

void ALobbyBeaconObject::SetMapInfo(FMapInfo NewMapInfo)
{
	MapInfo = NewMapInfo;
	UpdateClients();
}

void ALobbyBeaconObject::SetNewPlayer(const FString& PlayerName)
{
	PlayerNames.Add(PlayerName);
	HostPlayerList.Broadcast(PlayerNames);
	UpdateClients();
}