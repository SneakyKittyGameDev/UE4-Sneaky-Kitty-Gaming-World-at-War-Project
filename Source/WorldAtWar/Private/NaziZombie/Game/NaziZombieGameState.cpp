// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"


ANaziZombieGameState::ANaziZombieGameState()
{
	RoundNumber = 1;
	ZombiesOnMap = 0;
}

uint16 ANaziZombieGameState::GetRoundNumber()
{
	return RoundNumber;
}

void ANaziZombieGameState::IncrementRoundNumber()
{
	++RoundNumber;
}
