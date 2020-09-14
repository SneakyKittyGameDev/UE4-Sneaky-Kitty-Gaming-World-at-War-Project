//Copyright 2020, Cody Dawe, All rights reserved


#include "WorldAtWar/Public/NaziZombie/Useables/MysteryBox.h"
#include "WorldAtWar/Public/NaziZombie/Useables/WeaponBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"

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
	DOREPLIFETIME(AMysteryBox, BoxWeaponArray);
}

void AMysteryBox::AbleToGrabWeapon()
{
	GetWorld()->GetTimerManager().ClearTimer(TRaiseWeapon);
	GetWorld()->GetTimerManager().ClearTimer(TCycleWeapons);
	GetWorld()->GetTimerManager().SetTimer(TLowerWeapon, this, &AMysteryBox::LowerWeapon, 0.01f, true);

	CycleWeaponSpeed = CycleWeaponStartingSpeed;

	FTimerHandle TBoxFinished;
	GetWorld()->GetTimerManager().SetTimer(TBoxFinished, this, &AMysteryBox::BoxInteractionFinished, 11.0f, false);
}

void AMysteryBox::BoxInteractionFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("BOX INTERACTION FINISHED"));
	GetWorld()->GetTimerManager().ClearTimer(TLowerWeapon);
	if (CloseAnimation)
		MysteryBoxMesh->PlayAnimation(CloseAnimation, false);
	
	if (HasAuthority())
		bCanUseBox = true;
	
	HideAllWeapons();

	if (ANaziZombieCharacter* Player = Cast<ANaziZombieCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		Player->RefreshInteractableObject();
	}
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
	else
		return FString();
}

void AMysteryBox::Use(ANaziZombieCharacter* Player)
{
	if (HasAuthority() && Player && bCanUseBox)
	{
		bCanUseBox = false;
		
		uint8 RandomIndex = FMath::RandRange(0, BoxWeaponArrayLength);
		Multi_BoxUsed(RandomIndex);
	}
}