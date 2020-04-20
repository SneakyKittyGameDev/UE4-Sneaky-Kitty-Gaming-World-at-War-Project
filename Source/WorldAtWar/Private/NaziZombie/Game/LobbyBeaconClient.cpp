// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconClient.h"
#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconObject.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

ALobbyBeaconClient::ALobbyBeaconClient()
{

}

void ALobbyBeaconClient::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyBeaconClient, MapInfo);
	DOREPLIFETIME(ALobbyBeaconClient, PlayerNames);
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
	if (APlayerController* PController = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerState* PState = PController->PlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("GOT PLAYER STATE"));
			Server_SetClientInfo(PState->GetPlayerName());
		}
	}
}

bool ALobbyBeaconClient::Server_SetClientInfo_Validate(const FString& PlayerName)
{
	return true;
}

void ALobbyBeaconClient::Server_SetClientInfo_Implementation(const FString& PlayerName)
{
	UE_LOG(LogTemp, Warning, TEXT("SERVER: PLAYER NAME: %s"), *PlayerName);
	if (ALobbyBeaconObject* BeaconObject = Cast<ALobbyBeaconObject>(GetBeaconOwner()))
	{
		BeaconObject->SetNewPlayer(PlayerName);
	}
}

void ALobbyBeaconClient::OnRep_LobbyInfoChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("ON REP EVENT FIRED"));
	LobbyInfo.Broadcast(MapInfo);
}

void ALobbyBeaconClient::OnRep_PlayerInfoChanged()
{
	PlayerList.Broadcast(PlayerNames);
}

void ALobbyBeaconClient::SetLobbyInfo(FMapInfo NewMapInfo, TArray<FString> NewPlayerNames)
{
	if (HasAuthority())
	{
		MapInfo = NewMapInfo;
		PlayerNames = NewPlayerNames;
	}
}