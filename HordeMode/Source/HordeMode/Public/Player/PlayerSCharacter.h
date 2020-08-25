// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/SCharacter.h"
#include "PlayerSCharacter.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API APlayerSCharacter : public ASCharacter
{
	GENERATED_BODY()
	
public:
	APlayerSCharacter();

	virtual void AddControllerPitchInput(float Val) override;

	virtual void AddControllerYawInput(float Val) override;

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Player Forward Movement
	void MoveForward(float value);

	//Player Side Movement
	void MoveRight(float value);


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	///Death Out of Bounds

	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (ClampMin = 0.0))
		float PlayCountDown = 10;

	float ElaspeTime;

public:

	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetPlayContDown(float Value) { PlayCountDown = Value; };

	/*Sets the pause state of the character when player pressed start*/
	UFUNCTION(BlueprintCallable, Category = "Player")
		void SetPauseState(bool value) { bIsPaused = value; };

	/*Sets the pause state of the character when player pressed start*/
	UFUNCTION(BlueprintCallable, Category = "Player")
		bool GetPauseState() { return bIsPaused; };

	///Aim Assist

	/**True only if the player is moving else it is false
	@MoveRight
	*/
	bool bIsMoving;


	//returns true if line trance hits an enemy
	bool AimAssist(float DeltaTime);

private:
	bool bIsAimAssistOn;

	UPROPERTY(EditDefaultsOnly, Category = "AimAssist", meta = (ClampMin = 0.0))
		float MagnetismStrenght;
};
