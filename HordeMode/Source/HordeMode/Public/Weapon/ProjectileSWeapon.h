// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeapon.h"
#include "ProjectileSWeapon.generated.h"

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

protected:

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
			TSubclassOf<AActor> Grenades;
};
