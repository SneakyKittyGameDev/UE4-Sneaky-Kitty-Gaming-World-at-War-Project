// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Useables/Weapon1911.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


AWeapon1911::AWeapon1911()
{
	WeaponName = "1911";
	WeaponMaxAmmo = 98;
	MagazineMaxAmmo = 7;
	BaseDamage = 100;

	CurrentTotalAmmo = WeaponMaxAmmo;
	CurrentMagazineAmmo = MagazineMaxAmmo;
}

TArray<FHitResult> AWeapon1911::Fire(ANaziZombieCharacter* ShootingPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("SHOOTING 1911"));

	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}

	TArray<FHitResult> HitResults = PerformLineTrace(ShootingPlayer);
	if (HitResults.Num() > 0)
	{
		for (FHitResult& Result : HitResults)
		{
			if (AActor* HitActor = Result.GetActor())
			{
				if (AZombieBase* Zombie = Cast<AZombieBase>(Result.GetActor()))
				{
					Zombie->Hit(ShootingPlayer);
				}

				UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());
			}
		}
	}

	return HitResults;
}

void AWeapon1911::Reload()
{

}