//Copyright 2020, Cody Dawe, All rights reserved


#include "WorldAtWar/Public/NaziZombie/Useables/MysteryBox.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

AMysteryBox::AMysteryBox()
{
	MysteryBoxMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	RootComponent = MysteryBoxMesh;
	
	TESTWEAPONMESHLOCATION = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponentTEST");
	TESTWEAPONMESHLOCATION->SetupAttachment(MysteryBoxMesh);
	
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	CollisionMesh->SetupAttachment(MysteryBoxMesh);

	Cost = 950;

	bCanUseBox = true;
	WeaponHeight = 115.0f;
	WeaponRaiseSpeed = 1.0f;
	WeaponLowerSpeed = 12.0f;

	CycleWeaponStartingSpeed = 0.07f;
	CycleWeaponInterpSpeed = 2.5f;

	BoxWeaponArrayLength = 0;
	WeaponArrayIndex = 0;
	
	UsingPlayer = nullptr;
	bCanGrabWeapon = false;
}

void AMysteryBox::BeginPlay()
{
	Super::BeginPlay();
	UIMessage.Append(FString(" [Cost: " + FString::FromInt(Cost) + "]"));

	FVector BoxLoc = GetActorLocation();
	BottomLocation = FVector(BoxLoc.X, BoxLoc.Y += -2.1f, BoxLoc.Z += 25.0f);
	CurrentLocation = BottomLocation;
	TopLocation = FVector(BoxLoc.X, BoxLoc.Y += -2.1f, BoxLoc.Z += WeaponHeight);

	CycleWeaponSpeed = CycleWeaponStartingSpeed;
	
	if (HasAuthority())
	{
		for (UClass* Class : WeaponClasses)
		{
			if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(Class))
			{
				Weapon->SetActorLocation(BottomLocation);
				FRotator Rotation = GetActorRotation();
				Rotation.Yaw += 90.0f;
				Weapon->SetActorRotation(Rotation);
				Weapon->SetActorHiddenInGame(true);
				BoxWeaponArray.Add(Weapon);
			}
		}
		BoxWeaponArrayLength = BoxWeaponArray.Num() - 1;
	}
}

void AMysteryBox::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMysteryBox, bCanUseBox);
	DOREPLIFETIME(AMysteryBox, bCanGrabWeapon);
	DOREPLIFETIME(AMysteryBox, UsingPlayer);
	DOREPLIFETIME(AMysteryBox, BoxWeaponArray);
}

void AMysteryBox::AbleToGrabWeapon()
{
	GetWorld()->GetTimerManager().ClearTimer(TRaiseWeapon);
	GetWorld()->GetTimerManager().ClearTimer(TCycleWeapons);
	GetWorld()->GetTimerManager().SetTimer(TLowerWeapon, this, &AMysteryBox::LowerWeapon, 0.01f, true);

	CycleWeaponSpeed = CycleWeaponStartingSpeed;
	
	if (HasAuthority())
		bCanGrabWeapon = true;

	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player->RefreshInteractableObject();
	}
	
	GetWorld()->GetTimerManager().SetTimer(TBoxFinished, this, &AMysteryBox::BoxInteractionFinished, 11.0f, false);
}

void AMysteryBox::BoxInteractionFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("BOX INTERACTION FINISHED"));
	GetWorld()->GetTimerManager().ClearTimer(TLowerWeapon);
	GetWorld()->GetTimerManager().ClearTimer(TRaiseWeapon);
	GetWorld()->GetTimerManager().ClearTimer(TCycleWeapons);
	GetWorld()->GetTimerManager().ClearTimer(TBoxFinished);
	
	if (CloseAnimation)
		MysteryBoxMesh->PlayAnimation(CloseAnimation, false);
	
	if (HasAuthority())
	{
		bCanUseBox = true;
		UsingPlayer = nullptr;
		bCanGrabWeapon = false;
	}
	
	HideAllWeapons();

	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player->RefreshInteractableObject();
	}
	
	CurrentLocation = BottomLocation;
}

void AMysteryBox::HideAllWeapons()
{
	for (AWeaponBase* Weapon : BoxWeaponArray)
	{
		if (Weapon)
		{
			Weapon->SetActorHiddenInGame(true);
		}
	}
}

void AMysteryBox::CycleWeapon()
{
	if (WeaponArrayIndex > BoxWeaponArrayLength - 1)
		WeaponArrayIndex = 0;
	else
		++WeaponArrayIndex;
	
	SelectedWeapon = BoxWeaponArray[WeaponArrayIndex];
	if (SelectedWeapon == nullptr) return;
	SelectedWeapon->SetActorHiddenInGame(false);
	
	uint8 PreviousIndex = 0;
	if (WeaponArrayIndex > 0)
		PreviousIndex = WeaponArrayIndex - 1;
	else
		PreviousIndex = BoxWeaponArrayLength;
	
	PreviousWeapon = BoxWeaponArray[PreviousIndex];
	if (PreviousWeapon)
		PreviousWeapon->SetActorHiddenInGame(true);

	CycleWeaponSpeed = UKismetMathLibrary::FInterpTo_Constant(CycleWeaponSpeed, 0.7f, GetWorld()->GetDeltaSeconds(), CycleWeaponInterpSpeed);
	GetWorld()->GetTimerManager().SetTimer(TCycleWeapons, this, &AMysteryBox::CycleWeapon, CycleWeaponSpeed, true);
}

void AMysteryBox::RaiseWeapon()
{
	if (SelectedWeapon == nullptr) return;

	CurrentLocation = UKismetMathLibrary::VInterpTo(CurrentLocation, TopLocation, GetWorld()->GetDeltaSeconds(), WeaponRaiseSpeed);
	SelectedWeapon->SetActorLocation(CurrentLocation);
}

void AMysteryBox::LowerWeapon()
{
	if (SelectedWeapon == nullptr) return;

	CurrentLocation = UKismetMathLibrary::VInterpTo_Constant(CurrentLocation, BottomLocation, GetWorld()->GetDeltaSeconds(), WeaponLowerSpeed);
	SelectedWeapon->SetActorLocation(CurrentLocation);
}

bool AMysteryBox::Multi_BoxUsed_Validate(uint8 RandomIndex)
{
	return true;
}

void AMysteryBox::Multi_BoxUsed_Implementation(uint8 RandomIndex)
{
	WeaponArrayIndex = RandomIndex;
	BoxWeaponArrayLength = BoxWeaponArray.Num() - 1;
	
	if (OpenAnimation)
		MysteryBoxMesh->PlayAnimation(OpenAnimation, false);

	FTimerHandle TGrabWeapon;
	GetWorld()->GetTimerManager().SetTimer(TGrabWeapon, this, &AMysteryBox::AbleToGrabWeapon, 4.0f, false);
	
	GetWorld()->GetTimerManager().SetTimer(TRaiseWeapon, this, &AMysteryBox::RaiseWeapon, 0.01f, true);
	GetWorld()->GetTimerManager().SetTimer(TCycleWeapons, this, &AMysteryBox::CycleWeapon, CycleWeaponSpeed, true);

	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player->RefreshInteractableObject();
	}
}

FString AMysteryBox::GetUIMessage(ANaziZombieCharacter* Player)
{
	if (bCanUseBox)
		return UIMessage;
	if (Player && UsingPlayer && Player == UsingPlayer && bCanGrabWeapon)
		return FString("Press F To Grab Weapon");
	
	return FString();
}

bool AMysteryBox::Multi_BoxFinished_Validate()
{
	return true;
}

void AMysteryBox::Multi_BoxFinished_Implementation()
{
	BoxInteractionFinished();
}

void AMysteryBox::GrabWeapon()
{
	if (UsingPlayer && SelectedWeapon)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = UsingPlayer;
		
		if (AWeaponBase* Weapon = GetWorld()->SpawnActor<AWeaponBase>(SelectedWeapon->GetClass(), SpawnParams))
		{
			UsingPlayer->GivePlayerWeapon(Weapon, false);
			Multi_BoxFinished();
		}
	}
}

void AMysteryBox::Use(ANaziZombieCharacter* Player)
{
	if (Player && bCanUseBox)
	{
		if (ANaziZombiePlayerState* PState = Player->GetPlayerState<ANaziZombiePlayerState>())
			if (!PState->DecrementPoints(Cost))
				return;

		if (HasAuthority())
		{
			bCanUseBox = false;
			UsingPlayer = Player;
			
			uint8 RandomIndex = FMath::RandRange(0, BoxWeaponArrayLength);
			Multi_BoxUsed(RandomIndex);
		}
	}
	else if (HasAuthority() && bCanGrabWeapon && Player && UsingPlayer && Player == UsingPlayer)
	{
		GrabWeapon();
	}
}