//Copyright 2020, Cody Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NaziZombiePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPointsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClientsReady);

UCLASS()
class WORLDATWAR_API ANaziZombiePlayerState : public APlayerState
{
	GENERATED_BODY()
public:
    ANaziZombiePlayerState();

protected:
	UPROPERTY(BlueprintAssignable)
		FOnPointsChanged OnPointsChanged;
	UPROPERTY(BlueprintAssignable)
		FOnClientsReady OnClientsReady;

	UPROPERTY(Replicated, EditDefaultsOnly)//Set To Replicate MOVE TO PLAYER STATE WHEN CREATED
		int32 Points;

	void OnNewPlayerJoined();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Client, Reliable)
		void Client_OnClientsReady();
	void Client_OnClientsReady_Implementation();
	
	void IncrementPoints(uint16 Value);
	bool DecrementPoints(uint16 Value);
	UFUNCTION(BlueprintCallable)
		void UpdatePointsWidget();

	UFUNCTION(BlueprintCallable)
		int32 GetPoints();
};
