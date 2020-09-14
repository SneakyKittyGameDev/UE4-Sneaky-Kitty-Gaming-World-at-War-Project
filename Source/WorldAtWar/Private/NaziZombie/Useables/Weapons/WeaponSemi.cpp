//Copyright 2020, Cody Dawe, All rights reserved


#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/WeaponSemi.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AWeaponSemi::AWeaponSemi()
{
	WeaponName = "Default Name";
	WeaponMaxAmmo = 98;
	MagazineMaxAmmo = 7;
}

void AWeaponSemi::BeginPlay()
{
	Super::BeginPlay();
	CurrentTotalAmmo = WeaponMaxAmmo;
	CurrentMagazineAmmo = MagazineMaxAmmo;
}