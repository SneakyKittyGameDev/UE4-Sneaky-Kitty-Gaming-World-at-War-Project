//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Game/NaziZombiePlayerSpawnPoint.h"


ANaziZombiePlayerSpawnPoint::ANaziZombiePlayerSpawnPoint()
{
	bIsUsed = false;
}

bool ANaziZombiePlayerSpawnPoint::IsUsed()
{
	return bIsUsed;
}

void ANaziZombiePlayerSpawnPoint::SetUsed(bool Used)
{
	bIsUsed = Used;
}
