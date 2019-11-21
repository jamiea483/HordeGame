// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HordeModeGameMode.generated.h"


enum class EWaveState : uint8;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, PlayerController);

/**
 * 
 */
UCLASS()
class HORDEMODE_API AHordeModeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHordeModeGameMode();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void SpawnNewBot();

	//start Spawning Bots
	void StartWave();

	//Stop Spawning Bots
	void EndWave();


	void SpawnBotTimerElapsed();

	//Set Timer for Next wave
	void PrepareForNextWave();

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWave;

	UPROPERTY(EditDefaultsOnly, Category = "Gamemode")
		float BotSpawnerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Gamemode")
		float NextWaveInterval;

	int32 NrOfBotsToSpawn;

	int32 WaveCount;
	 
	void CheckWaveState();
	
	void CheckIfPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RespawnDeadPlayers();
public:
	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category ="Gamemode")
	FOnActorKilled OnActorKilled;

protected:
	//override postlogin and calls setplayerstate from the gamestate to up date the playerstate array.
	virtual void PostLogin(APlayerController* NewPlayer)override;
};
