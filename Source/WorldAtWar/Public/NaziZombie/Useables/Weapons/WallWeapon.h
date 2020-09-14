//Copyright 2020, Cody Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "WallWeapon.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AWallWeapon : public AInteractableBase
{
	GENERATED_BODY()
public:
	AWallWeapon();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UStaticMeshComponent* WallWeaponOutline;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class USkeletalMeshComponent* WallWeaponMesh;
	
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		FVector MeshStartLocation;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		FVector MeshEndLocation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		uint16 Cost;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		uint16 AmmoCost;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponPurchased)
		bool bIsUsed;

	UFUNCTION()
		void OnRep_WeaponPurchased();

	void LerpWeaponMeshToEnd();

	FTimerHandle TLerpMesh;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float MeshLerpSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Nazi Zombie Settings")
		FString AmmoUIMessage;
	
protected:
	virtual void BeginPlay() override;

	void PurchaseAmmo(class ANaziZombieCharacter* Player);

public:
	virtual FString GetUIMessage(class ANaziZombieCharacter* Player) override;
	virtual void Use(class ANaziZombieCharacter* Player) override;
};
