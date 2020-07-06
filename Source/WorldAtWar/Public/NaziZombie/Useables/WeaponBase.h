// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class WORLDATWAR_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimMontage* FPSArmsFireMontage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* ReloadAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		FString WeaponName;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		int32 BaseDamage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		int32 WeaponMaxAmmo;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		int32 MagazineMaxAmmo;

	int32 CurrentTotalAmmo;
	int32 CurrentMagazineAmmo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FHitResult> PerformLineTrace(class ANaziZombieCharacter* ShootingPlayer);

public:	
	virtual TArray<FHitResult> Fire(class ANaziZombieCharacter* ShootingPlayer);
	
	virtual void Reload();

	//first element is Magazine Ammo, second element is Total Ammo
	TArray<int32> GetCurrentAmmo();

	class UAnimMontage* GetFireAnimMontage();
};
