//Copyright 2020, Cody Dawe, All rights reserved
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
