// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"

// Sets default values
AZombieBase::AZombieBase()
{

}

// Called when the game starts or when spawned
void AZombieBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZombieBase::Hit(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player)
	{
		Player->IncrementPoints(100);
	}
}