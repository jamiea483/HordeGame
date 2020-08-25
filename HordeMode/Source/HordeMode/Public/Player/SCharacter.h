// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class HORDEMODE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void ChangeWeapon();

	ASWeapon* SpawnWeapon(TSubclassOf<ASWeapon> Weapon, FName SocketName);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//Calls crouch function in character
	void BeginCrouch();

	//Calls uncrouch function in character
	void EndCrouch();

	//Calls jump function in character
	void BeginJumping();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USHealthComponent* HealthComp;

	bool AimDownSight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float ZoomedFOV;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void BeginAim();

	void EndAim();

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<ASWeapon> StartWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<ASWeapon> nextWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
	ASWeapon* CurrentWeapon;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
		ASWeapon* BackUpWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		FName HolsterSocketName;

	void ReloadWeapon();

	virtual void SwitchWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	 void ServerSwitchWeapon();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/*Pawn died*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsDead;

	/*Player Pause the Game*/
	UPROPERTY(BlueprintReadOnly, Category = "Player")
		bool bIsPaused;

	/*The pitch of getcontrolRotaion - Getactorrotation normalize */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		float AimOffSetPitch;

	bool bInPlayArea;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
		bool bWeaponReload;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
		bool bSwitchingWeapon;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapon")
		bool bIsFiring;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Player")
		void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void StopFire();

	USpringArmComponent* GetSpringArm();

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool GetPlayArea() { return bInPlayArea; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetPlayArea(bool Value) { bInPlayArea = Value; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool GetWeaponReloading() { return bWeaponReload; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetWeaponReloading(bool value) { bWeaponReload = value; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool GetWeaponSwitching() { return bSwitchingWeapon; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		bool GetIsFiring() { return bIsFiring; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetIsFiring(bool value) { bIsFiring = value; };

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetWeaponSwitching(bool value) { bSwitchingWeapon = value; };

	///Pick up System
	/**An Array that holds all the weapon in range of the character that he can Pick up*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		TArray<ASWeapon*> WeaponList;

	/**An Array that holds all the weapon in range of the character that he can Pick up*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
		bool bCanPickUp;
};
