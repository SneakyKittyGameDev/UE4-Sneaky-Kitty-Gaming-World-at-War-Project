// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WorldAtWarGameMode.generated.h"

UCLASS(minimalapi)
class AWorldAtWarGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWorldAtWarGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};



