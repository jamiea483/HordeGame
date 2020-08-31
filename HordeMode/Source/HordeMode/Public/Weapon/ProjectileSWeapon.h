// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeapon.h"
#include "ProjectileSWeapon.generated.h"

USTRUCT(BlueprintType)
struct FProjectileFired
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TSubclassOf<AActor> Projectile;

	UPROPERTY()
	AActor* MyOwner;

	UPROPERTY()
		uint8 ReplicationCount;
};

/**
 * 
 */
UCLASS()
class HORDEMODE_API AProjectileSWeapon : public ASWeapon
{
	GENERATED_BODY()

public:

	AProjectileSWeapon();

	virtual void Fire() override;

	void SpawnProjectile(AActor * MyOwner, TSubclassOf<AActor> Projectile);

protected:
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
			TSubclassOf<AActor> Grenades;

		UPROPERTY(ReplicatedUsing = OnRep_SpawnProjectile)
		FProjectileFired projectile;

		//Called everytime HitScantrace Replicates.
		UFUNCTION()
			void OnRep_SpawnProjectile();
};
