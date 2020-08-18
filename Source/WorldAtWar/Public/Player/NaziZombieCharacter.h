// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CharacterBase.h"
#include "NaziZombieCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractChanged, const FString&, NewInteractMessage);

UCLASS()
class WORLDATWAR_API ANaziZombieCharacter : public ACharacterBase
{
	GENERATED_BODY()
public:
	ANaziZombieCharacter();

protected:
	UPROPERTY(BlueprintAssignable)
		FInteractChanged NewInteractMessage;

	FTimerHandle TInteractTimerHandle;
	class AInteractableBase* Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
		float InteractionRange;

	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
		TSubclassOf<class AKnife> KnifeClass;

	UPROPERTY(Replicated)
		class AKnife* Knife;

protected:
	void Interact();
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Interact(class AInteractableBase* InteractingObject);
	bool Server_Interact_Validate(class AInteractableBase* InteractingObject);
	void Server_Interact_Implementation(class AInteractableBase* InteractingObject);

	void SetInteractableObject();
	
	virtual void OnFire() override;
	virtual void OnStopFire() override;
	void OnReload();

	void OnKnifeAttack();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE class AKnife* GetKnife() {return Knife;}
	
public:
	void GivePlayerWeapon(class AWeaponBase* Weapon);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
