// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NaziZombiePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPointsChanged, int32, NewPoints);

UCLASS()
class WORLDATWAR_API ANaziZombiePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
    ANaziZombiePlayerState();

protected:
	UPROPERTY(BlueprintAssignable)
		FPointsChanged NewPoints;

	UPROPERTY(ReplicatedUsing = OnRep_PointsChanged, EditDefaultsOnly)//Set To Replicate MOVE TO PLAYER STATE WHEN CREATED
		int32 Points;
	UFUNCTION()
		void OnRep_PointsChanged();

public:
	void IncrementPoints(uint16 Value);
	bool DecrementPoints(uint16 Value);

	UFUNCTION(BlueprintCallable)
		int32 GetPoints();
};
