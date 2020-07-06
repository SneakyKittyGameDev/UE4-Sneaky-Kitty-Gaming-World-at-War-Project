// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CharacterBase.h"
#include "NaziZombieCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractChanged, const FString&, NewInteractMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPointsChanged, int32, NewPoints);

UCLASS()
class WORLDATWAR_API ANaziZombieCharacter : public ACharacterBase
{
	GENERATED_BODY()
public:
	ANaziZombieCharacter();

protected:
	UPROPERTY(BlueprintAssignable)
		FInteractChanged NewInteractMessage;

	UPROPERTY(BlueprintAssignable)
		FPointsChanged NewPoints;

	FTimerHandle TInteractTimerHandle;
	class AInteractableBase* Interactable;

	UPROPERTY(EditDefaultsOnly)
		float InteractionRange;

	UPROPERTY(EditDefaultsOnly)//Set To Replicate MOVE TO PLAYER STATE WHEN CREATED
		int32 Points;

protected:
	void Interact();
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Interact(class AInteractableBase* InteractingObject);
	bool Server_Interact_Validate(class AInteractableBase* InteractingObject);
	void Server_Interact_Implementation(class AInteractableBase* InteractingObject);

	void SetInteractableObject();

	virtual void OnFire() override;

public:
	void IncrementPoints(uint16 Value);
	bool DecrementPoints(uint16 Value);

	UFUNCTION(BlueprintCallable)
		int32 GetPoints();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
