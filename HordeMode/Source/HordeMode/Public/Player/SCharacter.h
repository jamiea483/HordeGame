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

	virtual void AddControllerPitchInput(float Val) override;

	virtual void AddControllerYawInput(float Val) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Player Forward Movement
	void MoveForward(float value);

	//Player Side Movement
	void MoveRight(float value);

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

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		FName WeaponSocketName;

	void ReloadWeapon();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/*Pawn died*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsDead;

	/*The pitch of getcontrolRotaion - Getactorrotation normalize */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
		float AimOffSetPitch;

	bool bInPlayArea;

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (ClampMin = 0.0))
		float PlayCountDown;

	float ElaspeTime;

	/*Player Pause the Game*/
	UPROPERTY(BlueprintReadOnly, Category = "Player")
		bool bIsPaused;

	//returns true if line trance hits an enemy
	bool AimAssist(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category = "AimAssist", meta = (ClampMin = 0.0))
		float MagnetismStrenght;

	bool bIsAimAssistOn;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

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
		void SetPlayContDown(float Value) { PlayCountDown = Value; };

	/*Sets the pause state of the character when player pressed start*/
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetPauseState(bool value) { bIsPaused = value; };

	/*Sets the pause state of the character when player pressed start*/
	UFUNCTION(BlueprintCallable, Category = "Player")
		bool GetPauseState() { return bIsPaused; };
};