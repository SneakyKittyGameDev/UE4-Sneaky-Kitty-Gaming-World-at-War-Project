//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Useables/InteractableBase.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

ANaziZombieCharacter::ANaziZombieCharacter()
{
	Interactable = nullptr;
	InteractionRange = 150.0f;
	bIsPerformingAction = false;
	bHasAllWeapons = false;
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
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANaziZombieCharacter::OnReload);
}

void ANaziZombieCharacter::RefreshInteractableObject()
{
	Interactable = nullptr;
}

void ANaziZombieCharacter::Interact()
{
	if (Interactable)
	{
		Interactable->Use(this);
		
		if (!HasAuthority())
			Server_Interact(Interactable);

		Interactable = nullptr;
	}
}

bool ANaziZombieCharacter::Server_Interact_Validate(AInteractableBase* InteractingObject)
{
	return true;
}

void ANaziZombieCharacter::Server_Interact_Implementation(AInteractableBase* InteractingObject)
{
	float Distance = GetDistanceTo(InteractingObject);

	if (Distance < InteractionRange + 80.0f)
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
		Interactable = Temp;
		NewInteractMessage.Broadcast(Interactable->GetUIMessage(this));
	}
	else if (Interactable && Temp == nullptr)
	{
		Interactable = nullptr;
		NewInteractMessage.Broadcast(FString());
	}
}

void ANaziZombieCharacter::GivePlayerWeapon(AWeaponBase* Weapon, bool DontReplace)
{
	if (HasAuthority() && Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADDING WEAPON TO WEAPON ARRAY"));
		if (WeaponArray.Num() >= 2 && !DontReplace)
		{
			WeaponArray[WeaponIndex]->Destroy();
			WeaponArray[WeaponIndex] = Weapon;
		}
		else
		{
			WeaponArray.Add(Weapon);
			WeaponIndex = WeaponArray.Num() - 1;
		}
		CurrentWeapon = Weapon;
		OnRep_AttachWeapon();
	}
}

void ANaziZombieCharacter::OnFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();//if fire valid
		RefreshAmmoWidget();
	}
}

void ANaziZombieCharacter::OnStopFire()
{
	if (CurrentWeapon)
		CurrentWeapon->StopFiring();
}

void ANaziZombieCharacter::OnReload()
{
	if (CurrentWeapon)
		CurrentWeapon->Reload();
}
