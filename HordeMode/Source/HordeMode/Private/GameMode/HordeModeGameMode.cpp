// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/HordeModeGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/SHealthComponent.h"
#include "GameFramework/Pawn.h"
#include "SGameState.h"
#include "States/SPlayerState.h"
#include "GameMode\HordeModeGameMode.h"

AHordeModeGameMode::AHordeModeGameMode()
{
	NextWaveInterval = 2.0f;
	BotSpawnerInterval = 0.5;

	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AHordeModeGameMode::StartWave()
{
	NrOfBotsToSpawn = 2 * WaveCount;

	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetEnemysLeft(NrOfBotsToSpawn);
	}

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AHordeModeGameMode::SpawnBotTimerElapsed, BotSpawnerInterval, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void AHordeModeGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}

void AHordeModeGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

void AHordeModeGameMode::PrepareForNextWave()
{
	WaveCount++;

	//Pass wave count to gamestate.
	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetCurrentWave(WaveCount);
	}

	GetWorldTimerManager().SetTimer(TimerHandle_NextWave, this, &AHordeModeGameMode::StartWave, NextWaveInterval, false);

	SetWaveState(EWaveState::PreparingNextWave);
}

void AHordeModeGameMode::CheckWaveState()
{
	bool bIsTimerActive = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWave);
	if (NrOfBotsToSpawn > 0 || bIsTimerActive)
	{
		return;
	}

	bool bAnyBotAlive = false;
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		//checks if a bot is alive, if there is break for loop cause wave is not done.
		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bAnyBotAlive = true;
			break;
		}
	}
	if (!bAnyBotAlive)
	{

		SetWaveState(EWaveState::WaveCompleted);
		PrepareForNextWave();
	}
}

void AHordeModeGameMode::CheckIfPlayerAlive()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APlayerController* PC = Cast<APlayerController>(It->Get());
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));

				if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
				{
					//a player still alive
					return;
				}
		}

	}
	//if it gets here no player are alive.
	GameOver();
}

void AHordeModeGameMode::GameOver()
{
	//@TODO Finish up match
	SetWaveState(EWaveState::GameOver);
}

//Sets Game States on GameStates
void AHordeModeGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS =  GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}

}

void AHordeModeGameMode::RespawnDeadPlayers()
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		APlayerController* PC = Cast<APlayerController>(It->Get());
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}

	}
}

void AHordeModeGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AHordeModeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckWaveState();
	CheckIfPlayerAlive();
}

void AHordeModeGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetPlayerStates();
	}
}
