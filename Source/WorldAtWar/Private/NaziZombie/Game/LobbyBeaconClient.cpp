// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconClient.h"


ALobbyBeaconClient::ALobbyBeaconClient()
{

}

void ALobbyBeaconClient::OnFailure()
{
	Super::OnFailure();
	UE_LOG(LogTemp, Warning, TEXT("BEACON FAILED TO CONNECT"));
}

bool ALobbyBeaconClient::StartBeacon(FString Address)
{
	FURL URL = FURL(nullptr, *Address, ETravelType::TRAVEL_Absolute);
	URL.Port = 7787;

	return InitClient(URL);
}

void ALobbyBeaconClient::ClientHasConnected_Implementation(const FString& ServerMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("Server: %s"), *ServerMessage);
}

