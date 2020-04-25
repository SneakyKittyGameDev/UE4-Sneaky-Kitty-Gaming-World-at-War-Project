// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieMainMenuGameMode.h"
#include "WorldAtWar/Public/NaziZombie/Game/MainMenu/ZombieLobbyBeaconHost.h"

#include "OnlineBeaconHost.h"
#include "Engine/World.h"

AZombieMainMenuGameMode::AZombieMainMenuGameMode()
{
	HostBeacon = nullptr;
}

void AZombieMainMenuGameMode::BeginPlay()
{

}

bool AZombieMainMenuGameMode::SetupLobby()
{
	if (AOnlineBeaconHost* Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass()))
	{
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);

			HostBeacon = GetWorld()->SpawnActor<AZombieLobbyBeaconHost>(AZombieLobbyBeaconHost::StaticClass());
			if (HostBeacon)
			{
				Host->RegisterHost(HostBeacon);
				HostBeacon->Init(FName("My Session"));
				UE_LOG(LogTemp, Warning, TEXT("REGISTERED LOBBY HOST BEACON"));
				return true;
			}
		}
	}
	return false;
}
