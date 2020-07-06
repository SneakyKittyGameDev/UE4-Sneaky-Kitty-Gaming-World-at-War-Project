// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/MainMenu/NaziZombieMainMenuGameMode.h"
#include "WorldAtWar/Public/NaziZombie/MainMenu/NaziZombieBeaconHostObject.h"

#include "OnlineBeaconHost.h"

ANaziZombieMainMenuGameMode::ANaziZombieMainMenuGameMode()
{
	HostObject = nullptr;
	Host = nullptr;
}

bool ANaziZombieMainMenuGameMode::CreateHostBeacon()
{
	Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());
	if (Host)
	{
		UE_LOG(LogTemp, Warning, TEXT("SPANWED AOnlineBeaconHost"));
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);
			UE_LOG(LogTemp, Warning, TEXT("INIT HOST"));
			HostObject = GetWorld()->SpawnActor<ANaziZombieBeaconHostObject>(ANaziZombieBeaconHostObject::StaticClass());
			if (HostObject)
			{
				UE_LOG(LogTemp, Warning, TEXT("SPANWED HOST OBJECT"));
				Host->RegisterHost(HostObject);
				return true;
			}
		}
	}
	return false;
}

ANaziZombieBeaconHostObject* ANaziZombieMainMenuGameMode::GetBeaconHost()
{
	return HostObject;
}