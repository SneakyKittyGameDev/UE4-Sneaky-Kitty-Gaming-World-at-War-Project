// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieLobbyBeaconClient.h"
#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieLobbyBeaconState.h"

AZombieLobbyBeaconClient::AZombieLobbyBeaconClient()
{
	
}

void AZombieLobbyBeaconClient::BeginPlay()
{
	Super::BeginPlay();
	BeaconState = GetWorld()->SpawnActor<AZombieLobbyBeaconState>(AZombieLobbyBeaconState::StaticClass());
	if (BeaconState)
	{
		UE_LOG(LogTemp, Warning, TEXT("BEACON STATE VALID"));
		SetLobbyState(BeaconState);
	}
}

bool AZombieLobbyBeaconClient::ServerLoginPlayer_Validate(const FString& InSessionId, const FUniqueNetIdRepl& InUniqueId, const FString& UrlString)
{
	return true;
}

void AZombieLobbyBeaconClient::ServerLoginPlayer_Implementation(const FString& InSessionId, const FUniqueNetIdRepl& InUniqueId, const FString& UrlString)
{
	UE_LOG(LogTemp, Warning, TEXT("ID: %s    NetId: %s    URL: %s"), *InSessionId, *InUniqueId->ToString(), *UrlString);
}

bool AZombieLobbyBeaconClient::ConnectToServer(const FString& Address)
{
	FURL URL = FURL(nullptr, *Address, ETravelType::TRAVEL_Absolute);
	URL.Port = 7787;
	return InitClient(URL);
}