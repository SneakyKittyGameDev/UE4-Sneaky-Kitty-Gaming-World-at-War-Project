//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

void ANaziZombieGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANaziZombieGameState, bIgnoreAmmo);
}

void ANaziZombieGameState::sf_use_ignoreAmmo(bool IgnoreAmmo)
{
	if (HasAuthority())
	{
		bIgnoreAmmo = IgnoreAmmo;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("You Are Not The Server!"));
	}
}

void ANaziZombieGameState::GiveAll()
{
	if (HasAuthority())
	{
		if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			if (Player->HasAllWeapons())
				return;
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = Player;
			for (UClass* WeaponClass : GiveAllWeaponsClasses)
			{
				if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams))
				{
					Weapon->WeaponIsNowInHand(false);
					Player->GivePlayerWeapon(Weapon, true);
				}
			}
			Player->SetHasAllWeapons();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("You Are Not The Server!"));
	}
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
