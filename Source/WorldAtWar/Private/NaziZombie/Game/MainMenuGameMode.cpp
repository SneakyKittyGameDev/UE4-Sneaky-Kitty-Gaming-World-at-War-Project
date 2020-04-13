// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/MainMenuGameMode.h"
#include "WorldAtWar/Public/NaziZombie/Game/LobbyBeaconObject.h"

#include "OnlineBeaconHost.h"
#include "OnlineJsonSerializer.h"
#include "Engine/World.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	Http = &FHttpModule::Get();
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void AMainMenuGameMode::CreateServer()
{
	FServerData ServerData = FServerData();

	ServerData.ServerName = "MY TEST SERVER NAME";
	ServerData.CurrentPlayers = 3;
	ServerData.MapName = "MY MAP NAME";

	TSharedPtr<FJsonObject> JsonObj = MakeShareable(new FJsonObject);
	JsonObj->SetNumberField("ServerID", ServerData.ServerID);
	JsonObj->SetStringField("IPAddress", ServerData.IPAddress);
	JsonObj->SetStringField("ServerName", ServerData.ServerName);
	JsonObj->SetNumberField("CurrentPlayers", ServerData.CurrentPlayers);
	JsonObj->SetStringField("MapName", ServerData.MapName);

	FString StringToSend;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&StringToSend);

	FJsonSerializer::Serialize(JsonObj.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AMainMenuGameMode::OnProcessRequestComplete);
	Request->SetURL("https://localhost:44363/api/server");
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	Request->SetContentAsString(StringToSend);
	Request->ProcessRequest();
}

void AMainMenuGameMode::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	Request->OnProcessRequestComplete().Unbind();
	UE_LOG(LogTemp, Warning, TEXT("Success: %d"), Success);
	if (Success)
	{
		FString ResponseStr = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("Success: %s"), *ResponseStr);
		SetupBeacon();
	}
}

void AMainMenuGameMode::SetupBeacon()
{
	if (AOnlineBeaconHost* Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass()))
	{
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);
			if (ALobbyBeaconObject* LobbyBeaconObj = GetWorld()->SpawnActor<ALobbyBeaconObject>(ALobbyBeaconObject::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("CREATED AND REGISTERED LOBBY BEACON HOST"));
				Host->RegisterHost(LobbyBeaconObj);
			}
		}
	}
}