// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameInstanceBase.h"

#include "Engine/World.h"

UNaziZombieGameInstanceBase::UNaziZombieGameInstanceBase()
{

}

void UNaziZombieGameInstanceBase::GoToMap(FString MAPURL)
{
	GetWorld()->ServerTravel(MAPURL);
}