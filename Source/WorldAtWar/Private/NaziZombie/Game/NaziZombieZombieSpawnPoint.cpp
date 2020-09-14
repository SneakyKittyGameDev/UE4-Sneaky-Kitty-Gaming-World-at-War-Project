//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieZombieSpawnPoint.h"
#include "WorldAtWar/Public/NaziZombie/Useables/Barricade.h"


ANaziZombieZombieSpawnPoint::ANaziZombieZombieSpawnPoint()
{
	Zone = 0;
	bIsActive = false;
}

void ANaziZombieZombieSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

ABarricade* ANaziZombieZombieSpawnPoint::GetLinkedBarricade()
{
	return LinkedBarricade;
}

uint8 ANaziZombieZombieSpawnPoint::GetZone()
{
	return Zone;
}

void ANaziZombieZombieSpawnPoint::SetZone(uint8 ZoneNumber)
{
	Zone = ZoneNumber;
}

bool ANaziZombieZombieSpawnPoint::IsActive()
{
	return bIsActive;
}

void ANaziZombieZombieSpawnPoint::Activate()
{
	bIsActive = true;
}