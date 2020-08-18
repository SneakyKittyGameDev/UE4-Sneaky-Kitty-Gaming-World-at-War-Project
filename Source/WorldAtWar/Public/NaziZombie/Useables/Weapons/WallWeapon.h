// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(ReplicatedUsing = OnRep_WeaponPurchased)
		bool bIsUsed;

	UFUNCTION()
		void OnRep_WeaponPurchased();

	void LerpWeaponMeshToEnd();

	FTimerHandle TLerpMesh;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float MeshLerpSpeed;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Use(class ANaziZombieCharacter* Player) override;
};
