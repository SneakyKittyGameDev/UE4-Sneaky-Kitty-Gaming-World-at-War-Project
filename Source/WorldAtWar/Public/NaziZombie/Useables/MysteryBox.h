//Copyright 2020, Cody Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "MysteryBox.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AMysteryBox : public AInteractableBase
{
	GENERATED_BODY()
public:
	AMysteryBox();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class USkeletalMeshComponent* MysteryBoxMesh;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UStaticMeshComponent* CollisionMesh;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class USkeletalMeshComponent* TESTWEAPONMESHLOCATION;
	

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float WeaponHeight;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float WeaponRaiseSpeed;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float WeaponLowerSpeed;
	
	FVector BottomLocation;
	FVector TopLocation;
	
	FVector CurrentLocation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		uint16 Cost;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* OpenAnimation;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* CloseAnimation;

	UPROPERTY(Replicated)
		bool bCanUseBox;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		TArray<TSubclassOf<class AWeaponBase>> WeaponClasses;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Nazi Zombie Settings")
		TArray<class AWeaponBase*> BoxWeaponArray;

	FTimerHandle TRaiseWeapon;
	FTimerHandle TLowerWeapon;
	FTimerHandle TCycleWeapons;
	
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float CycleWeaponStartingSpeed;
	float CycleWeaponSpeed;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float CycleWeaponInterpSpeed;

	uint8 BoxWeaponArrayLength;
	uint8 WeaponArrayIndex;

	class AWeaponBase* SelectedWeapon;
	class AWeaponBase* PreviousWeapon;
	
protected:
	virtual void BeginPlay() override;

	void AbleToGrabWeapon();
	void BoxInteractionFinished();
	void RaiseWeapon();
	void LowerWeapon();
	void CycleWeapon();
	void HideAllWeapons();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_BoxUsed(uint8 RandomIndex);
	bool Multi_BoxUsed_Validate(uint8 RandomIndex);
	void Multi_BoxUsed_Implementation(uint8 RandomIndex);
	
public:
	virtual FString GetUIMessage(class ANaziZombieCharacter* Player) override;
	
	virtual void Use(class ANaziZombieCharacter* Player) override;
};
