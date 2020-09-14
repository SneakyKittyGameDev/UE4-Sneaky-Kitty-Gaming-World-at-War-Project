//Copyright 2020, Cody Dawe, All rights reserved
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

	UPROPERTY(ReplicatedUsing = OnRep_KnifeAttached)
		class AKnife* Knife;
	UFUNCTION()
		void OnRep_KnifeAttached();

	bool bIsPerformingAction;

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

	UFUNCTION(BlueprintCallable)
		void SetPerformingAction(bool IsPerformingAction) {bIsPerformingAction = IsPerformingAction;}
	UFUNCTION(BlueprintCallable)
		bool IsPerformingAction() {return bIsPerformingAction;}

	void RefreshInteractableObject();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
