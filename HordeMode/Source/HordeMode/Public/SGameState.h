// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	PreparingNextWave,

	WaveInProgress,

	WaitingToComplete,

	WaveCompleted,

	GameOver
};

class ASPlayerState;

/**
 * 
 */
UCLASS()
class HORDEMODE_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
		void SetWaveState(EWaveState NewState);

	//Called in gamemode to set currentwave
		void SetCurrentWave(int32 value);

		UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void SetEnemysLeft(int32 value);

		//called in the crosshair widget to display current wave
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		int32 GetWaveNumber() { return currentWave; };

	//called in the crosshair widget to display how many enemys are left.
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		int32 GetEnemyLeft() { return EnemyLeft; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetPlayerStates();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		TArray<ASPlayerState*> GetPlayerStatesArray() { return PlayerStates; };

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void Sort();

protected:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
		EWaveState WaveState;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState oldState, EWaveState NewState);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
		int32 currentWave;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
		int32 EnemyLeft;

	UFUNCTION()
		void OnRep_WaveState(EWaveState oldState);
	
	UPROPERTY(Replicated ,VisibleAnywhere, Category = "Gameplay")
	TArray<ASPlayerState*> PlayerStates;

	void QuickSort(TArray<ASPlayerState*> arr, int32 low, int32 high);

	int32 Partition(TArray<ASPlayerState*> arr, int32 low, int32 high);

	/*true when the game enters the game over state.*/
	UPROPERTY(BlueprintReadWrite, Category = "GameOver")
		bool bIsGameOver = false;

	void GameOver();

	/*Game over screen*/
	UPROPERTY(EditDefaultsOnly, Category = "GameOver")
		TSubclassOf<class UUserWidget> GameOverScreen;
};
