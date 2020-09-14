//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/WallWeapon.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AWallWeapon::AWallWeapon()
{
	WallWeaponOutline = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = WallWeaponMesh;

	WallWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	WallWeaponMesh->SetupAttachment(WallWeaponOutline);

	SetReplicates(true);
	
	Cost = 1200;
	AmmoCost = 200;
	bIsUsed = false;
	MeshLerpSpeed = 10.0f;

	AmmoUIMessage = "Press F to buy Ammo";
}

void AWallWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	UIMessage.Append(FString(" [Cost: " + FString::FromInt(Cost) + "]"));
	AmmoUIMessage.Append(FString(" [Cost: " + FString::FromInt(AmmoCost) + "]"));
	
	WallWeaponMesh->SetRelativeLocation(MeshStartLocation);
}

void AWallWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWallWeapon, bIsUsed);
}

void AWallWeapon::LerpWeaponMeshToEnd()
{
	FVector NewLocation = UKismetMathLibrary::VInterpTo(WallWeaponMesh->GetRelativeLocation(), MeshEndLocation, 
		GetWorld()->GetDeltaSeconds(), MeshLerpSpeed);
	WallWeaponMesh->SetRelativeLocation(NewLocation);
	float Distance = FVector::Distance(WallWeaponMesh->GetRelativeLocation(), NewLocation);
	//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
	if (Distance == 0.0f)//double check
	{
		GetWorld()->GetTimerManager().ClearTimer(TLerpMesh);
	}
}

void AWallWeapon::OnRep_WeaponPurchased()
{
	GetWorld()->GetTimerManager().SetTimer(TLerpMesh, this, &AWallWeapon::LerpWeaponMeshToEnd, 0.01f, true);
}

FString AWallWeapon::GetUIMessage(ANaziZombieCharacter* Player)
{
	for (AWeaponBase* Weapon : *Player->GetWeaponArray())
			if (Weapon)
				if (Weapon->IsA(WeaponClass))
					return AmmoUIMessage;
	
	return UIMessage;
}

void AWallWeapon::PurchaseAmmo(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player)
	{
		if (Player->GetCurrentWeapon() && Player->GetCurrentWeapon()->IsA(WeaponClass) && !Player->GetCurrentWeapon()->IsTotalAmmoFull())
		{
			UE_LOG(LogTemp, Warning, TEXT("PURCHASING AMMO FOR: %s"), *Player->GetCurrentWeapon()->GetName());

			if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
				if (!PState->DecrementPoints(Cost))
					return;

			Player->GetCurrentWeapon()->RefillAmmo();
		}
	}
}

void AWallWeapon::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player)
	{		
		for (AWeaponBase* Weapon : *Player->GetWeaponArray())
			if (Weapon)
				if (Weapon->IsA(WeaponClass))
				{
					PurchaseAmmo(Player);
					return;
				}
		
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
			if (!PState->DecrementPoints(Cost))
				return;
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Player;
		if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams))//check WeaponClass valid
		{
			//Weapon->SetActorLocation(Player->GetActorLocation());
			Player->GivePlayerWeapon(Weapon);
			UE_LOG(LogTemp, Warning, TEXT("SPAWNED WEAPON"));
			if (!bIsUsed)
			{
				bIsUsed = true;
				OnRep_WeaponPurchased();
			}
		}
	}
}
