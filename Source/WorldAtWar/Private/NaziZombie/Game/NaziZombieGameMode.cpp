// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameMode.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieGameState.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombiePlayerSpawnPoint.h"
#include "WorldAtWar/Public/NaziZombie/Game/NaziZombieZombieSpawnPoint.h"
#include "WorldAtWar/Public/Player/NaziZombieCharacter.h"
#include "WorldAtWar/Public/NaziZombie/Useables/Barricade.h"
#include "WorldAtWar/Public/NaziZombie/Zombie/ZombieBase.h"

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

	UE_LOG(LogTemp, Warning, TEXT("ZombieSpawns: %d"), ZombieSpawnPoints.Num());

	GetWorld()->GetTimerManager().SetTimer(TZombieSpawnHandle, this, &ANaziZombieGameMode::SpawnZombie, 2.0f, true);
}

void ANaziZombieGameMode::CalculateZombieCount()
{
	if (ZombieGameState)
	{
		uint16 RoundNumber = ZombieGameState->GetRoundNumber();
		//do calculation here
		ZombiesRemaining = 5;
	}
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

void ANaziZombieGameMode::SpawnZombie()
{
	if (ZombiesRemaining > 0)
	{
		int RandomIndex = FMath::RandRange(0, ActiveZombieSpawnPoints.Num() - 1);

		if (ANaziZombieZombieSpawnPoint* SpawnPoint = ActiveZombieSpawnPoints[RandomIndex])
		{
			FVector Loc = SpawnPoint->GetActorLocation();
			FRotator Rot = SpawnPoint->GetActorRotation();

			if (AZombieBase* Zombie = GetWorld()->SpawnActor<AZombieBase>(ZombieClass, Loc, Rot))
			{
				//UE_LOG(LogTemp, Warning, TEXT("SPAWNING ZOMBIE"));
				--ZombiesRemaining;
			}
		}
	}
	else
	{
		GetWorld()->GetTimerManager().PauseTimer(TZombieSpawnHandle);
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