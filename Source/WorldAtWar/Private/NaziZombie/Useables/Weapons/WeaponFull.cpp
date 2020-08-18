// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/WeaponFull.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AWeaponFull::AWeaponFull()
{
	WeaponMaxAmmo = 240;
	MagazineMaxAmmo = 30;
	bIsInFullAuto = true;
	bIsSelectFire = true;
	FireRate = 500;
}

void AWeaponFull::BeginPlay()
{
	Super::BeginPlay();
	CurrentTotalAmmo = WeaponMaxAmmo;
	CurrentMagazineAmmo = MagazineMaxAmmo;

	FireRate = 60 / FireRate;
}

void AWeaponFull::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponFull, bIsFiring, COND_SkipOwner);
}

bool AWeaponFull::Server_StartFullAutoFire_Validate(bool IsFiring)
{
	return true;
}

void AWeaponFull::Server_StartFullAutoFire_Implementation(bool IsFiring)
{
	UE_LOG(LogTemp, Warning, TEXT("Server_STARTFULLAUTOFIRE"));
	bIsFiring = IsFiring;
	OnRep_StartFullAutoFire();
}

void AWeaponFull::OnRep_StartFullAutoFire()
{
	if (bIsFiring)
	{
		GetWorld()->GetTimerManager().SetTimer(WeaponFireHandle, this, &AWeaponFull::PlayWeaponEffects, FireRate, true);
		PlayWeaponEffects();
	}
	else
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireHandle);
}

void AWeaponFull::PlayWeaponEffects()
{
	//UE_LOG(LogTemp, Warning, TEXT("ONREP PLAYING WEAPON EFFECTS FOR FULL AUTO CLASS"));
	if (ANaziZombieCharacter* Character = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (!Character->IsLocallyControlled() && FireAnimation)
		{
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				if (ThirdPersonMontage)
				{
					AnimInstance->Montage_Play(ThirdPersonMontage);
					if (Character->GetIsAiming())
						AnimInstance->Montage_JumpToSection(FName("FireADS"), ThirdPersonMontage);
					else
						AnimInstance->Montage_JumpToSection(FName("FireHip"), ThirdPersonMontage);
				}
			}
			WeaponMesh->PlayAnimation(FireAnimation, false);
		}
	}
}

void AWeaponFull::Server_Fire_Implementation(const TArray<FHitResult>& HitResults)
{
	if (CurrentMagazineAmmo > 0)
	{
		Super::Server_Fire_Implementation(HitResults);

		if (HitResults.Num() > 0)
		{
			for (FHitResult Result : HitResults)
			{//do checking to prevent cheating
				if (AActor* HitActor = Result.GetActor())
				{
					if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
					{
						if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
							Zombie->Hit(Player, Result);
					}
				}
			}
		}
	}
}

void AWeaponFull::OnClientFire()
{
	if (CurrentMagazineAmmo > 0)
	{
		if (ANaziZombieCharacter* ShootingPlayer = Cast<ANaziZombieCharacter>(GetOwner()))
		{
			--CurrentMagazineAmmo;

			if (UAnimInstance* AnimInstance = ShootingPlayer->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					AnimInstance->Montage_Play(FPSArmsMontage);
					if (ShootingPlayer->GetIsAiming())
						AnimInstance->Montage_JumpToSection(FName("FireADS"), FPSArmsMontage);
					else
						AnimInstance->Montage_JumpToSection(FName("FireHip"), FPSArmsMontage);
				}
			}
			
			UE_LOG(LogTemp, Warning, TEXT("On Client Fire"));
			if (CurrentMagazineAmmo <= 0 && FireEmptyAnimation)
				WeaponMesh->PlayAnimation(FireEmptyAnimation, false);
			else if (FireAnimation)
				WeaponMesh->PlayAnimation(FireAnimation, false);

			TArray<FHitResult> HitResults = PerformLineTrace(ShootingPlayer);
			if (HitResults.Num() > 0)
			{
				for (FHitResult& Result : HitResults)
				{
					FString HitBone = Result.BoneName.ToString();
					if (AActor* HitActor = Result.GetActor())
					{
						if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
						{
							Zombie->Hit(ShootingPlayer, Result);
						}
						UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());			
					}
				}
			}
			if (!GetWorld()->IsServer())
				Server_Fire(HitResults);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireHandle);
		bIsFiring = false;
		if (!GetWorld()->IsServer())
			Server_StartFullAutoFire(bIsFiring);
	}
}

void AWeaponFull::Fire()
{
	if (!bIsFiring && bIsInFullAuto && bCanFire)//and is in full auto
	{
		bIsFiring = true;
		
		bCanFire = false;
		FTimerHandle FireRateHandle;
		GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &AWeaponFull::ControlFireDelay, DelayBetweenShots, false);
		
		OnClientFire();
		GetWorld()->GetTimerManager().SetTimer(WeaponFireHandle, this, &AWeaponFull::OnClientFire, FireRate, true);

		if (!GetWorld()->IsServer())
			Server_StartFullAutoFire(bIsFiring);
	}
	else//is in semi auto
	{
		Super::Fire();
	}
}

void AWeaponFull::StopFiring()
{
	UE_LOG(LogTemp, Warning, TEXT("StopFiring"));
	GetWorld()->GetTimerManager().ClearTimer(WeaponFireHandle);

	bIsFiring = false;
	if (!GetWorld()->IsServer())
		Server_StartFullAutoFire(bIsFiring);
}

void AWeaponFull::ChangeFireMode()
{
	if (bIsSelectFire)
		bIsInFullAuto = !bIsInFullAuto;
}