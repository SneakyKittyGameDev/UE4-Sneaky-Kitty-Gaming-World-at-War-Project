//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"


ANaziZombieGameState::ANaziZombieGameState()
{
	RoundNumber = 1;
	ZombiesOnMap = 0;
	TotalZombiesRemaining = 5;
	ZombieHealth = 150;
}

void ANaziZombieGameState::BeginPlay()
{
	Super::BeginPlay();
	//set initial health based on round number to allow for testing
	/*if (RoundNumber < 10)
		ZombieHealth += 100;
	else
		ZombieHealth *= 1.1f;*/
}

uint16 ANaziZombieGameState::GetRoundNumber()
{
	return RoundNumber;
}

void ANaziZombieGameState::IncrementRoundNumber()
{
	++RoundNumber;
	if (RoundNumber < 10)
	{
		ZombieHealth += 100;
	}
	else
	{
		ZombieHealth *= 1.1f;
	}
}

void ANaziZombieGameState::SetTotalZombiesRemaining(const uint16& ZombieCount)
{
	TotalZombiesRemaining = ZombieCount;
}

uint16 ANaziZombieGameState::GetTotalZombiesRemaining()
{
	return TotalZombiesRemaining;
}

void ANaziZombieGameState::ZombieKilled()
{
	--TotalZombiesRemaining;
	--ZombiesOnMap;
	UE_LOG(LogTemp, Warning, TEXT("ZOMBIES ON MAP: %d"), ZombiesOnMap);
}

uint8 ANaziZombieGameState::GetZombiesOnMap()
{
	return ZombiesOnMap;
}

void ANaziZombieGameState::ZombieSpawned()
{
	++ZombiesOnMap;
	UE_LOG(LogTemp, Warning, TEXT("ZOMBIES ON MAP: %d"), ZombiesOnMap);
}

float ANaziZombieGameState::GetZombieHealth()
{
	return ZombieHealth;
}
