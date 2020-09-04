// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Knife.generated.h"

UCLASS()
class WORLDATWAR_API AKnife : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnife();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
		class UStaticMeshComponent* KnifeMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Nazi Zombie Settings")
		class UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimMontage* FPSArmsMontage;
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		class UAnimMontage* ThirdPersonMontage;

	bool bCanDamage;
	bool bCanAttack;
	UPROPERTY(Replicated)
		bool bJustHitZombie;

	FTimerHandle TZombieHit;
	void RevokeJustHitZombie();
	
	UPROPERTY(EditAnywhere, Category = "Nazi Zombie Settings")
		int16 KnifeDamage;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_ZombieHit(class AZombieBase* Zombie);
	bool Server_ZombieHit_Validate(class AZombieBase* Zombie);
	void Server_ZombieHit_Implementation(class AZombieBase* Zombie);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_KnifeAttack();
	bool Server_KnifeAttack_Validate();
	void Server_KnifeAttack_Implementation();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void Multi_KnifeAttack();
	bool Multi_KnifeAttack_Validate();
	void Multi_KnifeAttack_Implementation();

public:
	void OnKnife();
	UFUNCTION(BlueprintCallable)
		FORCEINLINE class UStaticMeshComponent* GetKnifeMesh() {return KnifeMesh;}

	UFUNCTION(BlueprintCallable)
		void SetCanDamage(bool CanCauseDamage);
	UFUNCTION(BlueprintCallable)
		void SetCanAttack(bool CanAttack);

	FORCEINLINE int16 GetKnifeDamage() {return KnifeDamage;}
};
