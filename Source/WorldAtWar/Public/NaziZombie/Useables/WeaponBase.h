// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum EHitLocation
{
	None UMETA(Displayname, "None"),
	Head UMETA(DisplayName, "Head"),
	Neck UMETA(DisplayName, "Neck"),
	Chest UMETA(DisplayName, "Chest"),
	Abdomen UMETA(DisplayName, "Abdomen")
};

USTRUCT(BlueprintType)
struct FWeaponDamage
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
		float BaseDamage = 20.0f;
	UPROPERTY(EditDefaultsOnly)
		float HeadMultiplier = 3.5f;
	UPROPERTY(EditDefaultsOnly)
		float ChestMultiplier = 1.25f;
	UPROPERTY(EditDefaultsOnly)
		float AbdomenMultiplier = 1.1f;
	float GetDamage(EHitLocation HitLocation)
	{
		switch (HitLocation)
		{
			case Head: return BaseDamage * HeadMultiplier;
			case Chest: return BaseDamage * ChestMultiplier;
			case Abdomen: return BaseDamage * AbdomenMultiplier;
			default: return BaseDamage;
		}
	}
};

UENUM(BlueprintType)
enum EWeaponID
{
	Colt1911 UMETA(Displayname, "1911"),
	M1Carbine UMETA(Displayname, "M1Carbine"),
	STG44 UMETA(Displayname, "STG44")
};

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
		TEnumAsByte<EWeaponID> WeaponID;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* FireEmptyAnimation;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* ReloadAnimation;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimationAsset* ReloadEmptyAnimation;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimMontage* FPSArmsMontage;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimMontage* ThirdPersonMontage;
	
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		FString WeaponName;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float DelayBetweenShots;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		FWeaponDamage WeaponDamage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		int32 WeaponMaxAmmo;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		int32 MagazineMaxAmmo;

	UPROPERTY(Replicated)
		int32 CurrentTotalAmmo;
	UPROPERTY(Replicated)
		int32 CurrentMagazineAmmo;

	bool bCanFire;
	void ControlFireDelay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FHitResult> PerformLineTrace(class ANaziZombieCharacter* ShootingPlayer);
	TArray<FHitResult> PerformLineTrace(FVector MuzzleLocation, FRotator MuzzleRotation);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Fire(const TArray<FHitResult>& HitResults);
	bool Server_Fire_Validate(const TArray<FHitResult>& HitResults);
	virtual void Server_Fire_Implementation(const TArray<FHitResult>& HitResults);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_Fire(const FHitResult& HitResult);
	bool Multi_Fire_Validate(const FHitResult& HitResult);
	virtual void Multi_Fire_Implementation(const FHitResult& HitResult);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Reload();
	bool Server_Reload_Validate();
	virtual void Server_Reload_Implementation();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_Reload();
	bool Multi_Reload_Validate();
	virtual void Multi_Reload_Implementation();

public:	
	virtual void Fire();
	virtual void StopFiring();
	virtual void ChangeFireMode();
	FWeaponDamage GetWeaponDamage();

	//0 = cant reload, 1  = reload with ammo in mag, 2 = reload with empty magazine
	virtual void Reload();

	//first element is Magazine Ammo, second element is Total Ammo
	TArray<int32> GetCurrentAmmo();
	int32 GetMagazineAmmo();

	class UAnimMontage* GetFPSAnimMontage();

	UFUNCTION(BlueprintCallable)
	TEnumAsByte<EWeaponID> GetWeaponID();
	void WeaponIsNowInHand(bool InHand);
	class USkeletalMeshComponent* GetWeaponMesh() {return WeaponMesh;}
};
