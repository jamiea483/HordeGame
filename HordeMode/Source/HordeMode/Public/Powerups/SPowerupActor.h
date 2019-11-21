// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class HORDEMODE_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	/* Time Between powerup ticks*/
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float PowerUpInterval;

	/*Total time we apply the powerup effects*/
	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
		int32 TotalNumOfTicks;

		//Total nummber of ticks applyed
	int32 TicksProcesses;

	FTimerHandle TimerHandle_PowerUpTick;

	UFUNCTION()
		void OnTickPowerUp();

	//Keeps state of the power up
	UPROPERTY(ReplicatedUsing = OnRep_PowerUpActive)
		bool bIsPowerUpActive;

	UFUNCTION()
		void OnRep_PowerUpActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
		void OnPowerUpStateChanged(bool NewState);
public:	

	void ActivatePowerUp(AActor * ForActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
		void OnActivated(AActor* ForActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
	void OnPowerUpTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUps")
		void OnExpired();
};
