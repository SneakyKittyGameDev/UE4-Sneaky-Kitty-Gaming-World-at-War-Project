// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaziZombie/Useables/WeaponBase.h"
#include "WeaponSemi.generated.h"

/**
 * 
 */
UCLASS()
class WORLDATWAR_API AWeaponSemi : public AWeaponBase
{
	GENERATED_BODY()
public:
	AWeaponSemi();

protected:
	virtual void BeginPlay() override;

};
