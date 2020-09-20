//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"

#include "Net/UnrealNetwork.h"

ANaziZombiePlayerState::ANaziZombiePlayerState()
{
	Points = 5000;
}

void ANaziZombiePlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANaziZombiePlayerState, Points);
}

void ANaziZombiePlayerState::Client_OnClientsReady_Implementation()
{
	OnClientsReady.Broadcast();
	//FTimerHandle TTempHandle;
	//GetWorld()->GetTimerManager().SetTimer(TTempHandle, this, &ANaziZombiePlayerState::OnNewPlayerJoined, 2.0f, false);
}

void ANaziZombiePlayerState::OnNewPlayerJoined()
{
	//OnClientsReady.Broadcast();
}

void ANaziZombiePlayerState::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TPointsUpdate;
	GetWorld()->GetTimerManager().SetTimer(TPointsUpdate, this, &ANaziZombiePlayerState::UpdatePointsWidget, 0.05f, true);
}

void ANaziZombiePlayerState::UpdatePointsWidget()
{
	OnPointsChanged.Broadcast();
}

void ANaziZombiePlayerState::IncrementPoints(uint16 Value)
{
	Points += Value;
	
	OnPointsChanged.Broadcast();
	//UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
}

bool ANaziZombiePlayerState::DecrementPoints(uint16 Value)
{
	if (Points - Value < 0)
		return false;
	else
		Points -= Value;

	OnPointsChanged.Broadcast();
	//UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
	return true;
}

int32 ANaziZombiePlayerState::GetPoints()
{
	return Points;
}