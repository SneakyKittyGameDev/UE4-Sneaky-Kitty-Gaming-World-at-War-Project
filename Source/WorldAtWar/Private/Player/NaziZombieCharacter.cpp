// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Useables/InteractableBase.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"

ANaziZombieCharacter::ANaziZombieCharacter()
{
	Interactable = nullptr;
	InteractionRange = 150.0f;
	Points = 500;
}

void ANaziZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(TInteractTimerHandle, this, &ANaziZombieCharacter::SetInteractableObject, 0.1f, true);
}

void ANaziZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANaziZombieCharacter::Interact);
}

void ANaziZombieCharacter::Interact()
{
	if (Interactable)
	{
		if (HasAuthority())
			Interactable->Use(this);
		else
			Server_Interact(Interactable);
	}
}

bool ANaziZombieCharacter::Server_Interact_Validate(AInteractableBase* InteractingObject)
{
	return true;
}

void ANaziZombieCharacter::Server_Interact_Implementation(AInteractableBase* InteractingObject)
{
	float Distance = GetDistanceTo(InteractingObject);

	if (Distance < InteractionRange + 30.0f)
		InteractingObject->Use(this);
}

void ANaziZombieCharacter::SetInteractableObject()
{
	FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector Rot = GetFirstPersonCameraComponent()->GetComponentRotation().Vector();
	FVector End = Start + Rot * InteractionRange;

	FHitResult HitResult;
	FCollisionObjectQueryParams CollisionQuery;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByObjectType(OUT HitResult, Start, End, CollisionQuery, CollisionParams);

	AInteractableBase* Temp = Cast<AInteractableBase>(HitResult.GetActor());
	if (Interactable == nullptr && Temp)
	{
		UE_LOG(LogTemp, Warning, TEXT("IS NOW A VALID POINTER"));
		Interactable = Temp;
		NewInteractMessage.Broadcast(Interactable->GetUIMessage());
	}
	else if (Interactable && Temp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IS NOW A NULL PTR"));
		Interactable = nullptr;
		NewInteractMessage.Broadcast(FString());
	}
}

void ANaziZombieCharacter::IncrementPoints(uint16 Value)
{
	Points += Value;
	NewPoints.Broadcast(Points);
	UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
}

bool ANaziZombieCharacter::DecrementPoints(uint16 Value)
{
	if (Points - Value < 0)
		return false;
	else
		Points -= Value;

	NewPoints.Broadcast(Points);
	UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
	return true;
}

int32 ANaziZombieCharacter::GetPoints()
{
	return Points;
}

void ANaziZombieCharacter::OnFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire(this);
		if (UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance())
		{
			if (UAnimMontage* FireMontage = CurrentWeapon->GetFireAnimMontage())
			{
				UE_LOG(LogTemp, Warning, TEXT("PLAYING FIRE ANIM MONTAGE"));
				AnimInstance->Montage_Play(FireMontage);
			}
		}
	}
}
