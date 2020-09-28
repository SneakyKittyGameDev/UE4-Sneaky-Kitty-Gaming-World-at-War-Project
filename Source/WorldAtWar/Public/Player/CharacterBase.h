//Copyright 2020, Cody Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoChanged, int32, MagazineAmmo, int32, TotalAmmo);

UCLASS()
class WORLDATWAR_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	UPROPERTY(BlueprintAssignable)
		FAmmoChanged OnAmmoChanged;
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		TSubclassOf<class AWeaponBase> StartingWeaponClass;

	UPROPERTY(ReplicatedUsing = OnRep_AttachWeapon)
		class AWeaponBase* CurrentWeapon;
	UFUNCTION()
		void OnRep_AttachWeapon();
	//UPROPERTY(Replicated)
	class AWeaponBase* PreviousWeapon;
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SwitchWeapon(class AWeaponBase* NewWeapon, int32 NewWeaponIndex);
	bool Server_SwitchWeapon_Validate(class AWeaponBase* NewWeapon, int32 NewWeaponIndex);
	void Server_SwitchWeapon_Implementation(class AWeaponBase* NewWeapon, int32 NewWeaponIndex);

	UPROPERTY(Replicated)
		int32 WeaponIndex;
	UPROPERTY(Replicated)
		TArray<AWeaponBase*> WeaponArray;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
		TSubclassOf<class AKnife> KnifeClass;

	UPROPERTY(ReplicatedUsing = OnRep_KnifeAttached)
		class AKnife* Knife;
	UFUNCTION()
		void OnRep_KnifeAttached();

		void OnKnifeAttack();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE class AKnife* GetKnife() {return Knife;}

	//set to replicate, skip owner
	UPROPERTY(Replicated)
		bool bIsAiming;

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetAiming(bool WantsToAim);
	bool Server_SetAiming_Validate(bool WantsToAim);
	void Server_SetAiming_Implementation(bool WantsToAim);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:
	void SwitchNextWeapon();
	void SwitchPreviousWeapon();
	
	/** Fires a projectile. */
	virtual void OnFire();
	virtual void OnStopFire();
	virtual void ChangeWeaponFireMode();

	virtual void OnAimingStart();
	virtual void OnAimingEnd();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	//called when turning
	void TurnAtRate(float Rate);

	//called when looking up/down
	void LookUpAtRate(float Rate);

public:
	UFUNCTION(BlueprintCallable)
		bool GetIsAiming();

	UFUNCTION(BlueprintCallable)
		class AWeaponBase* GetCurrentWeapon();

	TArray<AWeaponBase*>* GetWeaponArray() {return &WeaponArray;}

	UFUNCTION(BlueprintCallable)
		void RefreshAmmoWidget();

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintCallable)
		FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};
