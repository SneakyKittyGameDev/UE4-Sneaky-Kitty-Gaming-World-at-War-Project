//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/MainMenu/NaziZombieBeaconHostObject.h"
#include "WorldAtWar/Public/NaziZombie/MainMenu/NaziZombieBeaconClient.h"
#include "WorldAtWar/Public/NaziZombie/MainMenu/NaziZombieMainMenuGameMode.h"

#include "OnlineBeaconHost.h"
#include "TimerManager.h"

ANaziZombieBeaconHostObject::ANaziZombieBeaconHostObject()
{
	ClientBeaconActorClass = ANaziZombieBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	Http = &FHttpModule::Get();
	ServerID = -1;
}

void ANaziZombieBeaconHostObject::BeginPlay()
{
	LobbyInfo.PlayerList.Add(FString("Host"));
	GetWorld()->GetTimerManager().SetTimer(TInitialLobbyHandle, this, &ANaziZombieBeaconHostObject::InitialLobbyHandling, 0.2f, false);
}

void ANaziZombieBeaconHostObject::InitialLobbyHandling()
{
	UpdateLobbyInfo(LobbyInfo);
}

void ANaziZombieBeaconHostObject::SetServerData(FServerData NewServerData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	ServerData = NewServerData;
	ServerData.CurrentPlayers = GetCurrentPlayerCount();

	JsonObject->SetNumberField("ServerID", 0);
	JsonObject->SetStringField("IPAddress", "51651651");
	JsonObject->SetStringField("ServerName", ServerData.ServerName);
	JsonObject->SetStringField("MapName", ServerData.MapName);
	JsonObject->SetNumberField("CurrentPlayers", ServerData.CurrentPlayers);
	JsonObject->SetNumberField("MaxPlayers", ServerData.MaxPlayers);

	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ANaziZombieBeaconHostObject::OnProcessRequestComplete);

	Request->SetURL("https://localhost:44389/api/Host");
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->SetContentAsString(JsonString);

	Request->ProcessRequest();
}

void ANaziZombieBeaconHostObject::UpdateServerData(FServerData NewServerData)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	ServerData = NewServerData;
	ServerData.CurrentPlayers = GetCurrentPlayerCount();

	JsonObject->SetNumberField("ServerID", 0);
	JsonObject->SetStringField("IPAddress", "51651651");
	JsonObject->SetStringField("ServerName", ServerData.ServerName);
	JsonObject->SetStringField("MapName", ServerData.MapName);
	JsonObject->SetNumberField("CurrentPlayers", ServerData.CurrentPlayers);
	JsonObject->SetNumberField("MaxPlayers", ServerData.MaxPlayers);

	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &ANaziZombieBeaconHostObject::OnProcessRequestComplete);

	Request->SetURL("https://localhost:44389/api/Host/1");
	Request->SetVerb("PUT");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->SetContentAsString(JsonString);

	Request->ProcessRequest();
}

int ANaziZombieBeaconHostObject::GetCurrentPlayerCount()
{
	return LobbyInfo.PlayerList.Num();
}

void ANaziZombieBeaconHostObject::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if (Success)
	{
		ServerID = FCString::Atoi(*Response->GetContentAsString());
		UE_LOG(LogTemp, Warning, TEXT("Success, ID: %d"), ServerID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HttpRequest FAILED"));
	}
}

void ANaziZombieBeaconHostObject::UpdateLobbyInfo(FNaziZombieLobbyInfo NewLobbyInfo)
{
	LobbyInfo.MapImage = NewLobbyInfo.MapImage;
	UpdateClientLobbyInfo();
	FOnHostLobbyUpdated.Broadcast(LobbyInfo);
}

void ANaziZombieBeaconHostObject::UpdateClientLobbyInfo()
{
	for (AOnlineBeaconClient* ClientBeacon : ClientActors)
	{
		if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(ClientBeacon))
		{
			Client->Client_OnLobbyUpdated(LobbyInfo);
		}
	}
}

void ANaziZombieBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);

	if (NewClientActor)
	{
		if (GetCurrentPlayerCount() >= ServerData.MaxPlayers)
		{
			if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(NewClientActor))
			{
				Client->SetPlayerIndex(240);
				DisconnectClient(NewClientActor);
				return;
			}
		}
		FString PlayerName = FString("Player ");
		uint8 Index = LobbyInfo.PlayerList.Num();
		PlayerName.Append(FString::FromInt(Index));
		LobbyInfo.PlayerList.Add(PlayerName);

		if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(NewClientActor))
		{
			Client->SetPlayerIndex(Index);
			Client->SetPlayerName(PlayerName);
		}

		UE_LOG(LogTemp, Warning, TEXT("CONNECTED CLIENT VALID"));
		FOnHostLobbyUpdated.Broadcast(LobbyInfo);
		UpdateClientLobbyInfo();

		UpdateServerData(ServerData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CONNECTED CLIENT INVALID"));
	}
}

void ANaziZombieBeaconHostObject::NotifyClientDisconnected(AOnlineBeaconClient* LeavingClientActor)
{
	Super::NotifyClientDisconnected(LeavingClientActor);

	UE_LOG(LogTemp, Warning, TEXT("Client Has DISCONNECTED"));
	if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(LeavingClientActor))
	{
		uint8 Index = Client->GetPlayerIndex();
		if (Index == 240)
		{
			return;
		}
		LobbyInfo.PlayerList.RemoveAt(Index);
	}

	FOnHostLobbyUpdated.Broadcast(LobbyInfo);
	UpdateClientLobbyInfo();
	UpdateServerData(ServerData);
}

void ANaziZombieBeaconHostObject::ShutdownServer()
{
	//Unregister Server From Database via Web API
	DisconnectAllClients();
	if (AOnlineBeaconHost* Host = Cast<AOnlineBeaconHost>(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("DESTROYING HOST BEACON"));
		//Host->UnregisterHost(BeaconTypeName);
		Host->DestroyBeacon();
	}

	if (ServerID != -1)
	{
		//REMOVE SERVER ENTRY FROM DATA TABLE
		TSharedRef<IHttpRequest> Request = Http->CreateRequest();

		Request->SetURL("https://localhost:44389/api/Host/" + FString::FromInt(ServerID));
		Request->SetVerb("DELETE");
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

		Request->ProcessRequest();
	}
}

void ANaziZombieBeaconHostObject::DisconnectAllClients()
{
	UE_LOG(LogTemp, Warning, TEXT("DISCONNECTING ALL CLIENTS"));
	for (AOnlineBeaconClient* Client : ClientActors)
	{
		if (Client)
			DisconnectClient(Client);
	}
}

void ANaziZombieBeaconHostObject::DisconnectClient(AOnlineBeaconClient* ClientActor)
{
	AOnlineBeaconHost* BeaconHost = Cast<AOnlineBeaconHost>(GetOwner());
	if (BeaconHost)
	{
		if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(ClientActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("DISCONNECTING CLIENT %s"), *ClientActor->GetName());
			Client->Client_OnDisconnected();
		}
		BeaconHost->DisconnectClient(ClientActor);
	}
}

void ANaziZombieBeaconHostObject::StartServer(const FString& MapURL)
{
	for (AOnlineBeaconClient* ClientBeacon : ClientActors)
	{
		if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(ClientBeacon))
		{
			Client->Client_FullConnect();
		}
	}

	ShutdownServer();
	GetWorld()->ServerTravel(MapURL + "?listen");
}

void ANaziZombieBeaconHostObject::SendChatToLobby(const FText& ChatMessage)
{
	FOnHostChatReceived.Broadcast(ChatMessage);

	for (AOnlineBeaconClient* ClientBeacon : ClientActors)
	{
		if (ANaziZombieBeaconClient* Client = Cast<ANaziZombieBeaconClient>(ClientBeacon))
		{
			Client->Client_OnChatMessageReceived(ChatMessage);
		}
	}
}
