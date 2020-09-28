//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameMode.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/Knife.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AZombieBase::AZombieBase()
{
	Health = 150;
	bIsDead = false;
	CleanupDelay = 5.0f;
}

// Called when the game starts or when spawned
void AZombieBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (ANaziZombieGameState* GS = GetWorld()->GetGameState<ANaziZombieGameState>())
		{
			Health = GS->GetZombieHealth();
			UE_LOG(LogTemp, Warning, TEXT("ZombieHealth: %f"), Health);
		}
	}
}

void AZombieBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZombieBase, bIsDead);
	DOREPLIFETIME(AZombieBase, Health);
}

void AZombieBase::DecrementHealth(int16 Damage)
{
	if (HasAuthority())
	{
		Health -= Damage;
		if (Health <= 0)
		{
			Die();
		}
	}
}

void AZombieBase::OnRep_Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);
	if (HasAuthority())
	{
		if (AController* AIController = GetController<AController>())
		{
			AIController->Destroy();
		}
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AZombieBase::OnCleanup, CleanupDelay, false);
	}
}

void AZombieBase::OnCleanup()
{
	Destroy();
}

void AZombieBase::Die()
{
	if (HasAuthority())
	{
		//get game mode and do zombie decremtation
		if (ANaziZombieGameMode* GM = GetWorld()->GetAuthGameMode<ANaziZombieGameMode>())
		{
			GM->ZombieKilled();
		}
		bIsDead = true;
		OnRep_Die();
	}
}

uint8 AZombieBase::GetHitPart(FString BoneName)
{
	if (BoneName.Contains(FString("l")) || BoneName.Contains(FString("r")))
	{//limb hit
		return 1;
	}
	else if (BoneName.Equals(FString("spine_02")))
	{//chest hit
		return 2;
	}
	else if (BoneName.Equals(FString("spine_01")) || BoneName.Equals(FString("pelvis")))//pelvis no hit
	{//abdomen
		return 3;
	}
	else if (BoneName.Equals(FString("neck_01")))
	{//neck hit
		return 4;
	}
	else if (BoneName.Equals(FString("head")))
	{//head hit
		return 5;
	}
	//nothing hit
	return 0;
}

uint8 AZombieBase::GetPointsForHit(uint8 HitPart, float Damage)
{
	if (Health - Damage <= 0)
	{
		switch (HitPart)
		{//fix here only doing limbs/torso/neck/chest/not head
		case 2: {DecrementHealth(Damage); return 60;}//chest
		case 5: {DecrementHealth(Damage); return 100;}//head
		case 3: {DecrementHealth(Damage); return 60;}//abdomen
		case 1: {DecrementHealth(Damage); return 50;}//limbs
		case 4: {DecrementHealth(Damage); return 70;}//neck
		default: {return 0;}
		}
	}
	else
	{
		DecrementHealth(Damage);
		return 10;
	}
}

void AZombieBase::Hit(ANaziZombieCharacter* Player, AKnife* Knife)
{
	if (Player && !bIsDead)
	{
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
		{
			if (Knife)
			{
				DecrementHealth(Knife->GetKnifeDamage());
				UE_LOG(LogTemp, Warning, TEXT("Hit Function On Zombie: %f"), Health);
				if (bIsDead)
					PState->IncrementPoints(100);
				else
					PState->IncrementPoints(10);
			}
		}
	}
}

void AZombieBase::Hit(ANaziZombieCharacter* Player, FHitResult HitResult)
{
	if (Player && !bIsDead)
	{
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
		{
			FString BoneName = HitResult.BoneName.ToString();
			if (BoneName == FString("None"))
				return;
			if (uint8 HitPart = GetHitPart(BoneName))
			{
				if (AWeaponBase* PlayerWeapon = Player->GetCurrentWeapon())
				{
					EHitLocation HitLocation = EHitLocation::None;
					switch (HitPart)
					{
					case 2: HitLocation = EHitLocation::Chest; break;
					case 3: HitLocation = EHitLocation::Abdomen; break;
					case 4: HitLocation = EHitLocation::Head; break;
					case 5: HitLocation = EHitLocation::Head; break;
					}
				
					float WeaponDamage = PlayerWeapon->GetWeaponDamage().GetDamage(HitLocation);
					UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), WeaponDamage);
					if (uint8 PointsForHit = GetPointsForHit(HitPart, WeaponDamage))
					{
						PState->IncrementPoints(PointsForHit);
					}
				}
			}
		}
	}
}