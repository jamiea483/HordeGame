// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/SWeapon.h"
#include "HitScanSWeapon.generated.h"

//Contains infomation of a single hitscan line trace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

		UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;

	UPROPERTY()
		uint8 ReplicationCount;

};

UCLASS()
class HORDEMODE_API AHitScanSWeapon : public ASWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitScanSWeapon();

	virtual void Fire() override;

protected:

	/**Name of the startpoint of the Smoke trail Animation*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* FleshImpactEffect;

	//Smoke effect of bullet trail
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* SmokeBullutEffect;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanLineTrace)
		FHitScanTrace HitScanTrace;

	virtual void PlayFireEffect(const FVector &TraceEndPoint);

	//Called everytime HitScantrace Replicates.
	UFUNCTION()
		void OnRep_HitScanLineTrace();

	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Impactpoint);

	void ApplyDamage(EPhysicalSurface SurfaceType, AActor * HitActor, FVector &ShotDirection, FHitResult &OutHit, AActor * MyOwner);
};
