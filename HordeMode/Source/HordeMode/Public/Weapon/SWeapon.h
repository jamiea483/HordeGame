// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractBase.h"
#include "SWeapon.generated.h"

class UDamageType;
class UParticleSystem;
class USoundCue;

UENUM(BlueprintType)
enum class EWeaponHolsterSize : uint8
{
	BigWeapon,
	SmallWeapon,
	Grenade

};

UCLASS()
class HORDEMODE_API ASWeapon : public AInteractBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	void StartFire();

	void EndFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	void PlaySFX(USoundCue* SoundToPlay);

	//Create a query that ignore Owning actor and play Area
	void CreateLineTraceCollisionQuery(FCollisionQueryParams &QueryParams, AActor * MyOwner);
	
	UFUNCTION()
		void AddtoMaxAmmo(float value);

	UFUNCTION()
		float GetRefillAmount() { return RefillAmmo; };

	UFUNCTION()
		float GetCurrentMag() { return CurrentMag; };

	UFUNCTION()
		float GetMagSize() { return MagSize; };

protected:

	virtual void BeginPlay()override;

	void PlayFireEffect();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

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

	/*Amount of ammo the character is carring*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		float RefillAmmo;

	bool bClipIsEmpty;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundCue* EmptyClipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		USoundCue* FireSound;

	virtual void Fire();

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

	/* bullet spread in degress*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (Clamp=0.0f))
		float BulletSpread;

	/* Weapon Lift after every fire*/
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (Clamp = 0.0f))
		float WeaponLift;

		void WeaponRecoil();

		/* Size of weapon for holster*/
		UPROPERTY(EditDefaultsOnly, Category = "Weapon")
			EWeaponHolsterSize Holster;
};