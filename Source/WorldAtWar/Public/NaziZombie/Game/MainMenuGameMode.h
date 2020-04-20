// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "MainMenuGameMode.generated.h"


USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		int32 ServerID = 0;
	UPROPERTY(BlueprintReadOnly)
		FString IPAddress = "127.0.0.1";
	UPROPERTY(BlueprintReadWrite)
		FString ServerName;
	UPROPERTY(BlueprintReadWrite)
		int32 CurrentPlayers;
	UPROPERTY(BlueprintReadWrite)
		FString MapName;
};

UCLASS()
class WORLDATWAR_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMainMenuGameMode();

protected:
	FHttpModule* Http;

	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);

	UPROPERTY(BlueprintReadOnly)
		class ALobbyBeaconObject* HostBeacon;

protected:
	virtual void BeginPlay() override;

	void SetupBeacon();

public:
	UFUNCTION(BlueprintCallable)
		void CreateServer();

};
