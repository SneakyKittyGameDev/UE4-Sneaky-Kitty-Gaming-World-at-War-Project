// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/WallWeapon.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"

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
	bIsUsed = false;
	MeshLerpSpeed = 10.0f;
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
	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
	if (Distance == 0.0f)//double check
	{
		GetWorld()->GetTimerManager().ClearTimer(TLerpMesh);
	}
}

void AWallWeapon::OnRep_WeaponPurchased()
{
	GetWorld()->GetTimerManager().SetTimer(TLerpMesh, this, &AWallWeapon::LerpWeaponMeshToEnd, 0.01f, true);
}

void AWallWeapon::BeginPlay()
{
	Super::BeginPlay();
	WallWeaponMesh->SetRelativeLocation(MeshStartLocation);
}

void AWallWeapon::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player)
	{
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
