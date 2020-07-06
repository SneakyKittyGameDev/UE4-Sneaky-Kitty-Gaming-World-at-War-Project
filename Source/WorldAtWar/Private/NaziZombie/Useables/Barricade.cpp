// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Useables/Barricade.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameMode.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABarricade::ABarricade()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");

	Cost = 500;
	AccessZone = 0;

	UIMessage += "Door [Cost: " + FString::FromInt(Cost) + "]";
	bIsUsed = false;
}

void ABarricade::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
}

void ABarricade::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABarricade, bIsUsed);
}

void ABarricade::OnRep_BarricadeUsed()
{
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (OpenAnimation)
		MeshComp->PlayAnimation(OpenAnimation, false);
	UIMessage = FString();
}

void ABarricade::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && !bIsUsed && Player && Player->DecrementPoints(Cost))
	{
		UE_LOG(LogTemp, Warning, TEXT("IN USE FUNCTION FOR %s"), *GetName());
		bIsUsed = true;
		OnRep_BarricadeUsed();

		if (ANaziZombieGameMode* GM = GetWorld()->GetAuthGameMode<ANaziZombieGameMode>())
		{
			GM->NewZoneActive(AccessZone);
		}
	}
}

uint8 ABarricade::GetAccessZone()
{
	return AccessZone;
}
