// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/InteractableBase.h"
#include "Barricade.generated.h"

UCLASS()
class WORLDATWAR_API ABarricade : public AInteractableBase
{
	GENERATED_BODY()
public:
	ABarricade();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UStaticMeshComponent* CollisionMesh;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* OpenAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		uint16 Cost;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		uint8 AccessZone;

	UPROPERTY(ReplicatedUsing = OnRep_BarricadeUsed)
		bool bIsUsed;

	UFUNCTION()
		void OnRep_BarricadeUsed();

protected:
	virtual void BeginPlay() override;
	virtual void Use(class ANaziZombieCharacter* Player) override;

public:
	uint8 GetAccessZone();
};
