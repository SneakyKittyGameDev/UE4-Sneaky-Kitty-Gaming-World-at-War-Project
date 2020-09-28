// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombiePool.generated.h"

class AZombieBase;

UCLASS()
class WORLDATWAR_API AZombiePool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZombiePool();

protected:
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
	TSubclassOf<AZombieBase> ZombieClass;
	
	TArray<AZombieBase*> Zombies;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void IncreaseBuffer();

public:
	AZombieBase* GetZombie();
};
