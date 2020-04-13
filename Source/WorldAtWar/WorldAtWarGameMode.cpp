// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "WorldAtWarGameMode.h"
#include "WorldAtWarHUD.h"
#include "WorldAtWar/Public/Player/CharacterBase.h"
#include "UObject/ConstructorHelpers.h"

AWorldAtWarGameMode::AWorldAtWarGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BlueprintClasses/Player/BP_CharacterBase.BP_CharacterBase_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AWorldAtWarHUD::StaticClass();
}
