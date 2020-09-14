//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Useables/InteractableBase.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"

// Sets default values
AInteractableBase::AInteractableBase()
{
	ObjectName = "Default";
	UIMessage = "Press F to buy";

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AInteractableBase::BeginPlay()
{
	Super::BeginPlay();

	UIMessage.Append(FString(" " + ObjectName));
}

FString AInteractableBase::GetUIMessage(ANaziZombieCharacter* Player)
{
	return UIMessage;
}

void AInteractableBase::Use(ANaziZombieCharacter* Player)
{

}