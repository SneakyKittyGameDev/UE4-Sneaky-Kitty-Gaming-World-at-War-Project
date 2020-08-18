// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"


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

void ANaziZombiePlayerState::OnRep_PointsChanged()
{
	NewPoints.Broadcast(Points);
}

void ANaziZombiePlayerState::IncrementPoints(uint16 Value)
{
	Points += Value;
	if (HasAuthority())
		OnRep_PointsChanged();
	NewPoints.Broadcast(Points);
	//UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
}

bool ANaziZombiePlayerState::DecrementPoints(uint16 Value)
{
	if (HasAuthority())
	{
		if (Points - Value < 0)
			return false;
		else
			Points -= Value;

		OnRep_PointsChanged();
		UE_LOG(LogTemp, Warning, TEXT("POINTS: %d"), Points);
	}
	return true;
}

int32 ANaziZombiePlayerState::GetPoints()
{
	return Points;
}