// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Zombie/ZombiePool.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

// Sets default values
AZombiePool::AZombiePool()
{

}

// Called when the game starts or when spawned
void AZombiePool::BeginPlay()
{
	Super::BeginPlay();

	if (ZombieClass)
	{
		for (uint8 i = 0; i < 50; ++i)
		{
			if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>())
			{
				Zombies.Add(Zombie);
			}
		}
	}
}

void AZombiePool::IncreaseBuffer()
{
	if (ZombieClass)
	{
		for (uint8 i = 0; i < 10; ++i)
		{
			if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>())
			{
				Zombies.Add(Zombie);
			}
		}
	}
}

AZombieBase* AZombiePool::GetZombie()
{
	for(AZombieBase* Zombie : Zombies)
	{
		if (Zombie && !Zombie->IsActive())
		{
			Zombie->SetActive(true);
			return Zombie;
		}
	}

	IncreaseBuffer();
	if (ZombieClass)
	{
		if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>())
		{
			Zombie->SetActive(true);
			Zombies.Add(Zombie);
			return Zombie;
		}
	}
	return nullptr;
}
