// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PowerUpInterval = 0.0f;

	bIsPowerUpActive = false;

	TotalNumOfTicks = 0;
	SetReplicates(true);
}

void ASPowerupActor::OnTickPowerUp()
{
	TicksProcesses++;

	OnPowerUpTicked();

		if (TotalNumOfTicks >= TicksProcesses)
		{
			OnExpired();

			//calls OnRep_PowerupActive on clients
			bIsPowerUpActive = false;

			//Calls OnRep_PowerActive on server.
			OnRep_PowerUpActive();


			GetWorldTimerManager().ClearTimer(TimerHandle_PowerUpTick);
		}
}

void ASPowerupActor::OnRep_PowerUpActive()
{
	OnPowerUpStateChanged(bIsPowerUpActive);
}

void ASPowerupActor::ActivatePowerUp(AActor* ForActor)
{
	OnActivated(ForActor);
	//calls OnRep_PowerupActive on clients
	bIsPowerUpActive = true;

	//Calls OnRep_PowerActive on server.
	OnRep_PowerUpActive();

	if (PowerUpInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerUpTick, this, &ASPowerupActor::OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerUpActive);

}
