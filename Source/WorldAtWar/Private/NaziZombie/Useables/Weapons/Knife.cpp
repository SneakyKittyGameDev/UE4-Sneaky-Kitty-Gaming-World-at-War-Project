//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/Knife.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AKnife::AKnife()
{
	KnifeMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = KnifeMesh;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	CollisionComponent->SetupAttachment(KnifeMesh);
	CollisionComponent->SetRelativeLocation(FVector(0.0f, 21.0f, 0.0f));
	CollisionComponent->InitBoxExtent(FVector(1.2f, 13.0f, 2.7f));
	
	SetReplicates(true);
	
	bCanDamage = false;
	bCanAttack = true;
	bJustHitZombie = false;
	KnifeDamage = 100;
}

// Called when the game starts or when spawned
void AKnife::BeginPlay()
{
	Super::BeginPlay();
	//hide knife by default, unhide when player is knifing
	KnifeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AKnife::OnOverlapBegin);
}

void AKnife::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AKnife, bJustHitZombie, COND_SkipOwner);
}

void AKnife::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult &SweepResult)
{
	if (bCanDamage)
	{
		if (AZombieBase* Zombie = Cast<AZombieBase>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("HITTING ZOMBIE WITH KNIFE"));
			bCanDamage = false;
			if (GetWorld()->IsServer())
				Zombie->Hit(Cast<ANaziZombieCharacter>(GetOwner()), this);
			else
				Server_ZombieHit(Zombie);
			bJustHitZombie = true;
			GetWorld()->GetTimerManager().SetTimer(TZombieHit, this, &AKnife::RevokeJustHitZombie, 0.5f, false);
		}
	}
}

void AKnife::RevokeJustHitZombie()
{
	bJustHitZombie = false;
}

bool AKnife::Server_ZombieHit_Validate(AZombieBase* Zombie)
{
	return true;
}

void AKnife::Server_ZombieHit_Implementation(AZombieBase* Zombie)
{//check if zombie location is within x distance of player
	bJustHitZombie = true;
	GetWorld()->GetTimerManager().SetTimer(TZombieHit, this, &AKnife::RevokeJustHitZombie, 0.5f, false);
	Zombie->Hit(Cast<ANaziZombieCharacter>(GetOwner()), this);
}

void AKnife::OnKnife()
{
	//handle client knifing to keep it responsive
	if (bCanAttack && FPSArmsMontage)
	{
		if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
		{
			if (Player->IsPerformingAction()) return;
			
			if (UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(FPSArmsMontage);
				if (bJustHitZombie)
					AnimInstance->Montage_JumpToSection(FName("KnifeAttack2"), FPSArmsMontage);
				else
					AnimInstance->Montage_JumpToSection(FName("KnifeAttack1"), FPSArmsMontage);
			}

			Player->SetPerformingAction(true);
		}

		if (GetWorld()->IsServer())
			Multi_KnifeAttack();
		else
			Server_KnifeAttack();
	}
}

void AKnife::SetCanDamage(bool CanCauseDamage)
{
	bCanDamage = CanCauseDamage;
}

void AKnife::SetCanAttack(bool CanAttack)
{
	bCanAttack = CanAttack;
}

bool AKnife::Server_KnifeAttack_Validate()
{
	return true;
}

void AKnife::Server_KnifeAttack_Implementation()
{
	Multi_KnifeAttack();
}

bool AKnife::Multi_KnifeAttack_Validate()
{
	return true;
}

void AKnife::Multi_KnifeAttack_Implementation()
{
	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(GetOwner()))
	{
		if (!Player->IsLocallyControlled())
		{
			if (ThirdPersonMontage)
			{
				if (UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance())
				{
					AnimInstance->Montage_Play(ThirdPersonMontage);
					if (bJustHitZombie)
						AnimInstance->Montage_JumpToSection(FName("KnifeAttack2"), ThirdPersonMontage);
					else
						AnimInstance->Montage_JumpToSection(FName("KnifeAttack1"), ThirdPersonMontage);
				}
			}
		}
	}
}