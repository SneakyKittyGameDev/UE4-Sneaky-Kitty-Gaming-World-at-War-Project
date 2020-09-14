//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponName = "Default Name";
	DelayBetweenShots = 0.14;
	WeaponMaxAmmo = 255;
	MagazineMaxAmmo = 30;

	CurrentTotalAmmo = WeaponMaxAmmo;
	CurrentMagazineAmmo = MagazineMaxAmmo;

	bCanFire = true;
	bIsFiring = false;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponMesh->HideBoneByName(FName("emptyCase_1"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_2"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_3"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("emptyCase_4"), EPhysBodyOp::PBO_None);
	WeaponMesh->HideBoneByName(FName("Magazine2"), EPhysBodyOp::PBO_None);
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentTotalAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentMagazineAmmo, COND_OwnerOnly);
}

TArray<FHitResult> AWeaponBase::PerformLineTrace(ANaziZombieCharacter* ShootingPlayer)
{
	FVector Start = WeaponMesh->GetSocketLocation(FName("muzzleSocket"));
	FVector Rot = WeaponMesh->GetSocketRotation(FName("muzzleSocket")).Vector();

	FVector End = Start + Rot * 5000.0f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(ShootingPlayer);

	FCollisionResponseParams CollisionResponse;

	GetWorld()->LineTraceMultiByChannel(OUT HitResults, Start, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionResponse);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 3.0f);
	return HitResults;
}

TArray<FHitResult> AWeaponBase::PerformLineTrace(FVector MuzzleLocation, FRotator MuzzleRotation)
{
	FVector End = MuzzleLocation + MuzzleRotation.Vector() * 5000.0f;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if (GetOwner())
		CollisionParams.AddIgnoredActor(GetOwner());

	FCollisionResponseParams CollisionResponse;

	GetWorld()->LineTraceMultiByChannel(OUT HitResults, MuzzleLocation, End, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams, CollisionResponse);
	//DrawDebugLine(GetWorld(), MuzzleLocation, End, FColor::Red, false, 2.0f, 0, 3.0f);
	return HitResults;
}

bool AWeaponBase::Server_Fire_Validate(const TArray<FHitResult>& HitResults)
{
	return true;
}

void AWeaponBase::Server_Fire_Implementation(const TArray<FHitResult>& HitResults)
{
	if (CurrentMagazineAmmo > 0)
	{
		--CurrentMagazineAmmo;
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
		if (HitResults.Num() > 0)
			Multi_Fire(HitResults[0]);
		else
			Multi_Fire(FHitResult());
	}
}

bool AWeaponBase::Multi_Fire_Validate(const FHitResult& HitResult)
{
	return true;
}

void AWeaponBase::Multi_Fire_Implementation(const FHitResult& HitResult)
{
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

void AWeaponBase::ControlFireDelay()
{
	bCanFire = true;
	bIsFiring = false;
	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		Player->SetPerformingAction(false);
		Player->RefreshAmmoWidget();
	}
}

void AWeaponBase::Fire()
{
	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (Player->IsPerformingAction()) return;
		
		if (CurrentMagazineAmmo > 0 && bCanFire)
		{
			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					AnimInstance->Montage_Play(FPSArmsMontage);
					if (Player->GetIsAiming())
						AnimInstance->Montage_JumpToSection(FName("FireADS"), FPSArmsMontage);
					else
						AnimInstance->Montage_JumpToSection(FName("FireHip"), FPSArmsMontage);
				}
			}
			
			--CurrentMagazineAmmo;
			bCanFire = false;
			bIsFiring = true;
			
			if (CurrentMagazineAmmo <= 0 && FireEmptyAnimation)
					WeaponMesh->PlayAnimation(FireEmptyAnimation, false);
				else if (FireAnimation)
					WeaponMesh->PlayAnimation(FireAnimation, false);

			TArray<FHitResult> HitResults = PerformLineTrace(Player);
			if (HitResults.Num() > 0)
			{
				for (FHitResult& Result : HitResults)
				{
					FString HitBone = Result.BoneName.ToString();
					if (AActor* HitActor = Result.GetActor())
					{
						if (AZombieBase* Zombie = Cast<AZombieBase>(HitActor))
						{
							Zombie->Hit(Player, Result);
						}
						UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *HitActor->GetName());			
					}
				}
			}
			if (GetWorld()->IsServer())
			{
				if (HitResults.Num() > 0)
					Multi_Fire(HitResults[0]);
				else
					Multi_Fire(FHitResult());
			}
			else
				Server_Fire(HitResults);

			FTimerHandle FireRateHandle;
			GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &AWeaponBase::ControlFireDelay, DelayBetweenShots, false);
		}
	}
}

void AWeaponBase::StopFiring()
{
	
}

void AWeaponBase::ChangeFireMode()
{
}

FWeaponDamage AWeaponBase::GetWeaponDamage()
{
	return WeaponDamage;
}

bool AWeaponBase::Server_Reload_Validate()
{
	return true;
}

void AWeaponBase::Server_Reload_Implementation()
{
	Reload();
}

bool AWeaponBase::Multi_Reload_Validate()
{
	return true;
}

void AWeaponBase::Multi_Reload_Implementation()
{
	if (ANaziZombieCharacter* Character = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (!Character->IsLocallyControlled() && ReloadAnimation)
		{
			if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
			{
				if (ThirdPersonMontage)
				{
					AnimInstance->Montage_Play(ThirdPersonMontage);
					AnimInstance->Montage_JumpToSection(FName("Reload"), ThirdPersonMontage);
				}
			}
			WeaponMesh->PlayAnimation(ReloadAnimation, false);
		}
	}
}

void AWeaponBase::Reload()
{
	if (CurrentTotalAmmo > 0 && CurrentMagazineAmmo != MagazineMaxAmmo)
	{
		if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
		{
			if (Player->IsPerformingAction()) return;
			bool bMagazineIsEmpty = CurrentMagazineAmmo <= 0;

			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				if (FPSArmsMontage)
				{
					bCanFire = false;
					AnimInstance->Montage_Play(FPSArmsMontage);
					float AnimationLength = 1.0f;
					if (bMagazineIsEmpty)
					{
						AnimInstance->Montage_JumpToSection(FName("ReloadEmpty"), FPSArmsMontage);
						AnimationLength = FPSArmsMontage->GetSectionLength(3);
					}
					else
					{
						AnimInstance->Montage_JumpToSection(FName("Reload"), FPSArmsMontage);
						AnimationLength = FPSArmsMontage->GetSectionLength(2);
					}
					UE_LOG(LogTemp, Warning, TEXT("Length of Anim: %f"), AnimationLength);
					AnimationLength -= 0.15f;
					FTimerHandle FireRateHandle;
					GetWorld()->GetTimerManager().SetTimer(FireRateHandle, this, &AWeaponBase::ControlFireDelay, AnimationLength, false);
				}
			}
			
			if (APawn* Pawn = Cast<APawn>(GetOwner()))
			{
				if (Pawn->IsLocallyControlled())
				{
					if (CurrentMagazineAmmo <= 0 && ReloadEmptyAnimation)
						WeaponMesh->PlayAnimation(ReloadEmptyAnimation, false);
					else if (ReloadAnimation)
						WeaponMesh->PlayAnimation(ReloadAnimation, false);
				}
			}

			int Difference = MagazineMaxAmmo - CurrentMagazineAmmo;
			if (CurrentTotalAmmo - Difference >= 0)
			{
				CurrentTotalAmmo -= Difference;
				CurrentMagazineAmmo = MagazineMaxAmmo;	
			}
			else
			{
				CurrentMagazineAmmo += CurrentTotalAmmo;
				CurrentTotalAmmo = 0;
			}

			Player->SetPerformingAction(true);

			if (GetWorld()->IsServer())
				Multi_Reload();
			else
				Server_Reload();
		}
	}
}

TArray<int32> AWeaponBase::GetCurrentAmmo()
{
	return { CurrentMagazineAmmo, CurrentTotalAmmo };
}

int32 AWeaponBase::GetMagazineAmmo()
{
	return CurrentMagazineAmmo;
}

void AWeaponBase::Client_RefillAmmo_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("CLIENT REFILL AMMO"));
	RefillAmmo();
}

void AWeaponBase::RefillAmmo()
{
	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		CurrentTotalAmmo = WeaponMaxAmmo;
		if (HasAuthority() && !Player->IsLocallyControlled())
			Client_RefillAmmo();
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("REFRESHING AMMO WIDGET"));
			Player->RefreshAmmoWidget();
		}
	}
}

UAnimMontage* AWeaponBase::GetFPSAnimMontage()
{
	return FPSArmsMontage;
}

TEnumAsByte<EWeaponID> AWeaponBase::GetWeaponID()
{
	return WeaponID;
}

void AWeaponBase::WeaponIsNowInHand(bool InHand)
{
	WeaponMesh->SetHiddenInGame(!InHand);
}
