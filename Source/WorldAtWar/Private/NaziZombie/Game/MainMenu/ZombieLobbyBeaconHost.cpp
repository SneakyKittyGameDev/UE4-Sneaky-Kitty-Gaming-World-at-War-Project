// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieLobbyBeaconHost.h"
#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieLobbyBeaconClient.h"
#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieLobbyBeaconState.h"

AZombieLobbyBeaconHost::AZombieLobbyBeaconHost()
{
	ClientBeaconActorClass = AZombieLobbyBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	LobbyStateClass = AZombieLobbyBeaconState::StaticClass();
}

void AZombieLobbyBeaconHost::PostLogin(ALobbyBeaconClient* ClientActor)
{
	Super::PostLogin(ClientActor);

	UE_LOG(LogTemp, Warning, TEXT("NEW CLIENT JOINED"));
}

void AZombieLobbyBeaconHost::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);
	
	UE_LOG(LogTemp, Warning, TEXT("Client CONNECTED"));
	if (AZombieLobbyBeaconClient* BeaconClient = Cast<AZombieLobbyBeaconClient>(NewClientActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("CASTED NewClientActor"));
	}
}

ALobbyBeaconPlayerState* AZombieLobbyBeaconHost::HandlePlayerLogin(ALobbyBeaconClient* ClientActor, const FUniqueNetIdRepl& InUniqueId, const FString& Options)
{
	Super::HandlePlayerLogin(ClientActor, InUniqueId, Options);

	UE_LOG(LogTemp, Warning, TEXT("HandlePlayerLogin CONNECTED"));
	if (AZombieLobbyBeaconClient* BeaconClient = Cast<AZombieLobbyBeaconClient>(ClientActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("HandlePlayerLogin NewClientActor"));
		return BeaconClient->PlayerState;
	}
	return nullptr;
}
