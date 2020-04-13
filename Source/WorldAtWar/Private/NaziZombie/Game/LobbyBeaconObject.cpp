// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconObject.h"
#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconClient.h"

ALobbyBeaconObject::ALobbyBeaconObject()
{
	ClientBeaconActorClass = ALobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ALobbyBeaconObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (ALobbyBeaconClient* ClientBeacon = Cast<ALobbyBeaconClient>(NewClientActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("OnClientConnected Cast Successful"));
		ClientBeacon->ClientHasConnected("Hello New Client");
	}
}

AOnlineBeaconClient* ALobbyBeaconObject::SpawnBeaconActor(UNetConnection* ClientConnection)
{
	return Super::SpawnBeaconActor(ClientConnection);
}
