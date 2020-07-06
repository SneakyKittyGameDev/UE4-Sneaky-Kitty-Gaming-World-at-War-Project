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

void AWorldAtWarGameMode::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY GAMEMODE"));
}

void AWorldAtWarGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("PostLogin GAMEMODE"));
}
