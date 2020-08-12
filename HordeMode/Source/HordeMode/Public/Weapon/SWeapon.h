// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class USoundCue;

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
class HORDEMODE_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	void StartFire();

	void EndFire();

	bool Reload();

	void PlaySFX(USoundCue* SoundToPlay);

	//Create a query that ignore Owning actor and play Area
	void CreateLineTraceCollisionQuery(FCollisionQueryParams &QueryParams, AActor * MyOwner);

protected:

	virtual void BeginPlay()override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComp;

	void PlayFireEffect(const FVector &TraceEndPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* FleshImpactEffect;

	//Smoke effect of bullet trail
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* SmokeBullutEffect;

	//Camera
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShake> ShakingCam;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float MaxAmmoToCarry;

	/*Amount of ammo the character is carring*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float MaxAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float MagSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float CurrentMag;

	bool bClipIsEmpty;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundCue* EmptyClipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundCue* FireSound;

	virtual void Fire();



	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector Impactpoint);

	void ApplyDamage(EPhysicalSurface SurfaceType, AActor * HitActor, FVector &ShotDirection, FHitResult &OutHit, AActor * MyOwner);

	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerReload();

	//Time between shots
	FTimerHandle TimeHandle_TimeBeteenShots;

	float LastFireTime;

	/**Bullet per minute fire*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float RateOfFire;

	float TimeBetweenShots;


	UPROPERTY(ReplicatedUsing = OnRep_HitScanLineTrace)
		FHitScanTrace HitScanTrace;

	//Called everytime HitScantrace Replicates.
	UFUNCTION()
		void OnRep_HitScanLineTrace();

	/* bullet spread in degress*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (Clamp=0.0f))
		float BulletSpread;

	/* Weapon Lift after every fire*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (Clamp = 0.0f))
		float WeaponLift;

		void WeaponRecoil();

};
