//Copyright 2020, Cody Dawe, All rights reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBase.generated.h"

UCLASS()
class WORLDATWAR_API AInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableBase();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Nazi Zombie Settings")
		FString UIMessage;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		FString ObjectName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual FString GetUIMessage(class ANaziZombieCharacter* Player);
	virtual void Use(class ANaziZombieCharacter* Player);
};
