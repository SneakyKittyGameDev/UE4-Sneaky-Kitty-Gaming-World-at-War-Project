//Copyright 2020, Cody Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "WeaponFull.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AWeaponFull : public AWeaponBase
{
	GENERATED_BODY()
public:
	AWeaponFull();

protected:
	bool bIsInFullAuto;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		bool bIsSelectFire;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		float FireRate;
	
	UPROPERTY(ReplicatedUsing = OnRep_StartFullAutoFire)//replicated using for effects on other clients
		bool bIsFiring;
	UFUNCTION()
	void OnRep_StartFullAutoFire();

	FTimerHandle WeaponFireHandle;
	void PlayWeaponEffects();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_StartFullAutoFire(bool IsFiring);
	bool Server_StartFullAutoFire_Validate(bool IsFiring);
	void Server_StartFullAutoFire_Implementation(bool IsFiring);
	
	virtual void Server_Fire_Implementation(const TArray<FHitResult>& HitResults) override;

	void OnClientFire();
	
public:
	virtual void Fire() override;
	virtual void StopFiring() override;
	virtual void ChangeFireMode() override;
	virtual bool IsFiring() override {return bIsFiring;}
};