//Copyright 2020, Cody Dawe, All rights reserved

#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameMode.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombiePlayerSpawnPoint.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieZombieSpawnPoint.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Useables/Barricade.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"
#include "WorldAtWar/Public/Player/NaziZombiePlayerState.h"

#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


ANaziZombieGameMode::ANaziZombieGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/BlueprintClasses/Player/BP_NaziZombieCharacter.BP_NaziZombieCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;


	bHasLoadedSpawnPoints = false;

	ZombiesRemaining = 0;
}

void ANaziZombieGameMode::BeginPlay()
{
	Super::BeginPlay();

	ZombieGameState = GetGameState<ANaziZombieGameState>();
	CalculateZombieCount();

	TArray<AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANaziZombieZombieSpawnPoint::StaticClass(), TempActors);
	for (AActor* Actor : TempActors)
	{
		if (ANaziZombieZombieSpawnPoint* SpawnPoint = Cast<ANaziZombieZombieSpawnPoint>(Actor))
		{
			if (ABarricade* LinkedBarricade = SpawnPoint->GetLinkedBarricade())
			{
				SpawnPoint->SetZone(LinkedBarricade->GetAccessZone());
				ZombieSpawnPoints.Add(SpawnPoint);
				UE_LOG(LogTemp, Warning, TEXT("Zone Number: %d"), LinkedBarricade->GetAccessZone());
			}
			else
			{
				ActiveZombieSpawnPoints.Add(SpawnPoint);
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TZombieSpawnHandle, this, &ANaziZombieGameMode::SpawnZombie, 1.0f, true);
	GetWorld()->GetTimerManager().PauseTimer(TZombieSpawnHandle);
	CalculateZombieCount();

	FTimerHandle TTempClientReady;
	GetWorld()->GetTimerManager().SetTimer(TTempClientReady, this, &ANaziZombieGameMode::TempOnClientsReady, 5.0f, false);
}

void ANaziZombieGameMode::SetSpawnPoints()
{
	TArray<AActor*> TempActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANaziZombiePlayerSpawnPoint::StaticClass(), TempActors);
	for (AActor* Actor : TempActors)
	{
		if (ANaziZombiePlayerSpawnPoint* SpawnPoint = Cast<ANaziZombiePlayerSpawnPoint>(Actor))
		{
			PlayerSpawnPoints.Add(SpawnPoint);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawn Point Count: %d"), PlayerSpawnPoints.Num());
	bHasLoadedSpawnPoints = true;
}

void ANaziZombieGameMode::TempOnClientsReady()
{
	for (APlayerState* PState : GetGameState<ANaziZombieGameState>()->PlayerArray)
	{
		if (ANaziZombiePlayerState* PNState = Cast<ANaziZombiePlayerState>(PState))
		{
			PNState->Client_OnClientsReady();
		}
	}
}

void ANaziZombieGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (bHasLoadedSpawnPoints == false)
		SetSpawnPoints();
	
	for (ANaziZombiePlayerSpawnPoint* SpawnPoint : PlayerSpawnPoints)
	{
		if (!SpawnPoint->IsUsed())
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
			if (APawn* Pawn = GetWorld()->SpawnActor<APawn>(PlayerClass, SpawnLocation, FRotator::ZeroRotator))
			{
				UE_LOG(LogTemp, Warning, TEXT("SPAWEND PAWN TO POSSESS"));
				NewPlayer->Possess(Pawn);
				SpawnPoint->SetUsed(true);
				return;
			}
		}
	}
}

void ANaziZombieGameMode::NewZoneActive(uint8 ZoneNumber)
{
	UE_LOG(LogTemp, Warning, TEXT("SETTING ACTIVE ZONE: %d"), ZoneNumber);

	int Control = 0;

	for (int16 x = ZombieSpawnPoints.Num() - 1; x >= 0; --x)
	{
		ANaziZombieZombieSpawnPoint* SpawnPoint = ZombieSpawnPoints[x];

		UE_LOG(LogTemp, Warning, TEXT("LoopCount: %d"), Control);
		++Control;

		if (SpawnPoint && ZoneNumber == SpawnPoint->GetZone() && !SpawnPoint->IsActive())
		{
			ActiveZombieSpawnPoints.Add(SpawnPoint);
			SpawnPoint->Activate();
			//remove spawn point from the ZombieSpawnPoints array
			ZombieSpawnPoints.RemoveAt(x);
		}
	}
}

void ANaziZombieGameMode::SpawnZombie()
{
	if (ZombieGameState)
	{
		uint8 PlayerCount = ZombieGameState->PlayerArray.Num();
		uint8 MaxZombiesOnMapAtOnce = 24;
		if (PlayerCount > 1)
			MaxZombiesOnMapAtOnce += PlayerCount * 6;
		
		if (ZombiesRemaining > 0 && ZombieGameState->GetZombiesOnMap() <= MaxZombiesOnMapAtOnce - 1)
		{
			if (ActiveZombieSpawnPoints.Num() < 1) return;
			
			int RandomIndex = FMath::RandRange(0, ActiveZombieSpawnPoints.Num() - 1);

			if (ANaziZombieZombieSpawnPoint* SpawnPoint = ActiveZombieSpawnPoints[RandomIndex])
			{
				FVector Loc = SpawnPoint->GetActorLocation();
				FRotator Rot = SpawnPoint->GetActorRotation();

				if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>(ZombieClass, Loc, Rot))
				{
					ZombieGameState->ZombieSpawned();
					--ZombiesRemaining;
				}
			}
		}
		else if (ZombiesRemaining <= 0)
		{
			GetWorld()->GetTimerManager().PauseTimer(TZombieSpawnHandle);
		}
	}
}

void ANaziZombieGameMode::CalculateZombieCount()
{
	if (ZombieGameState)
	{
		uint16 RoundNumber = ZombieGameState->GetRoundNumber();
		uint8 PlayerCount = ZombieGameState->PlayerArray.Num();
		uint8 MaxZombiesOnMapAtOnce = 24;
		if (PlayerCount > 1)
			MaxZombiesOnMapAtOnce += PlayerCount * 6;

		UE_LOG(LogTemp, Warning, TEXT("PlayerCount: %d"), PlayerCount);
		if (RoundNumber > 0 && RoundNumber <= 5)
		{
			UE_LOG(LogTemp, Warning, TEXT("ROUNDS 1-5"));
			ZombiesRemaining = FMath::FloorToInt((RoundNumber * 0.2f) * MaxZombiesOnMapAtOnce);
			UE_LOG(LogTemp, Warning, TEXT("ZombiesRemaining: %d"), ZombiesRemaining);
		}
		else
		{
			ZombiesRemaining = FMath::FloorToInt((RoundNumber * 0.15f) * MaxZombiesOnMapAtOnce);
		}
		//do calculation here
		ZombieGameState->SetTotalZombiesRemaining(ZombiesRemaining);
		GetWorld()->GetTimerManager().UnPauseTimer(TZombieSpawnHandle);
	}
}

void ANaziZombieGameMode::ZombieKilled()
{
	if (ZombieGameState)
	{
		ZombieGameState->ZombieKilled();
		if (ZombieGameState->GetTotalZombiesRemaining() == 0)
		{//start new round
			ZombieGameState->IncrementRoundNumber();
			FTimerHandle TempHandle;
			GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &ANaziZombieGameMode::CalculateZombieCount, 5.0f, false);
		}
		UE_LOG(LogTemp, Warning, TEXT("Zombies Remaining: %d"), ZombieGameState->GetTotalZombiesRemaining());
	}
}