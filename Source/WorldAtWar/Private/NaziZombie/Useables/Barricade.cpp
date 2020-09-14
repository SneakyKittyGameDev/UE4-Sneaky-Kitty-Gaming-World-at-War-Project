//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Useables/Barricade.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameMode.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABarricade::ABarricade()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MeshComp;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	CollisionMesh->SetupAttachment(MeshComp);
	
	Cost = 500;
	AccessZone = 0;

	bIsUsed = false;
}

void ABarricade::BeginPlay()
{
	Super::BeginPlay();

	UIMessage.Append(FString(" [Cost: " + FString::FromInt(Cost) + "]"));
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
	if (HasAuthority() && !bIsUsed && Player)
	{
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
		{
			if (!PState->DecrementPoints(Cost))
				return;

			UE_LOG(LogTemp, Warning, TEXT("IN USE FUNCTION FOR %s"), *GetName());
			bIsUsed = true;
			OnRep_BarricadeUsed();

			if (ANaziZombieGameMode* GM = GetWorld()->GetAuthGameMode<ANaziZombieGameMode>())
			{
				GM->NewZoneActive(AccessZone);
			}
		}
	}
}

uint8 ABarricade::GetAccessZone()
{
	return AccessZone;
}
