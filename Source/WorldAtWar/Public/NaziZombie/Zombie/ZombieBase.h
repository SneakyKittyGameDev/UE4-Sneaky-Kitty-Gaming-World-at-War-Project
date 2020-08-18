// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieBase.generated.h"

UCLASS()
class WORLDATWAR_API AZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieBase();

protected:
	UPROPERTY(Replicated)
		float Health;

	UPROPERTY(EditDefaultsOnly)
		float CleanupDelay;

	UPROPERTY(ReplicatedUsing = OnRep_Die, EditAnywhere)
		bool bIsDead;
	UFUNCTION()
		void OnRep_Die();
	void OnCleanup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//0 = none, 1 = limb, 2 = chest, 3 = abdomen, 4 = neck, 5 = head
	uint8 GetHitPart(FString BoneName);
	void DecrementHealth(int16 Damage);
	void Die();

	uint8 GetPointsForHit(uint8 HitPart, float Damage);

public:
	void Hit(class ANaziZombieCharacter* Player, class AKnife* Knife);
	void Hit(class ANaziZombieCharacter* Player, FHitResult HitResult);
};
