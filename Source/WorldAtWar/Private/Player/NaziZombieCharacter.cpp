// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Useables/InteractableBase.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/NaziZombie/Useables/Weapons/Knife.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

ANaziZombieCharacter::ANaziZombieCharacter()
{
	Interactable = nullptr;
	InteractionRange = 150.0f;
}

void ANaziZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		
		Knife = GetWorld()->SpawnActor<AKnife>(KnifeClass, SpawnParams);
		if (Knife)
		{
			OnRep_KnifeAttached();
		}
	}
	GetWorld()->GetTimerManager().SetTimer(TInteractTimerHandle, this, &ANaziZombieCharacter::SetInteractableObject, 0.1f, true);
}

void ANaziZombieCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANaziZombieCharacter, Knife);
}

void ANaziZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ANaziZombieCharacter::Interact);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ANaziZombieCharacter::OnReload);

	PlayerInputComponent->BindAction("KnifeAttack", IE_Pressed, this, &ANaziZombieCharacter::OnKnifeAttack);
}


void ANaziZombieCharacter::OnRep_KnifeAttached()
{
	if (Knife)
	{
		if (IsLocallyControlled())
			Knife->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
		else
			Knife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("s_knifeHolster"));
	}
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

void ANaziZombieCharacter::GivePlayerWeapon(AWeaponBase* Weapon)
{
	if (HasAuthority() && Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADDING WEAPON TO WEAPON ARRAY"));
		if (WeaponArray.Num() >= 2)
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
		CurrentWeapon->Fire();
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

void ANaziZombieCharacter::OnKnifeAttack()
{
	if (Knife)
	{
		Knife->OnKnife();
	}
}
