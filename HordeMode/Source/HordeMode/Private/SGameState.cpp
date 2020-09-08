// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"
#include "States/SPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerState.h"

#include "Components/HordeGameInstance.h"
#include "UI/MainMenuWidget.h"
#include "Controller/MenuController.h"
#include "Kismet/GameplayStatics.h"



void ASGameState::SetWaveState(EWaveState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EWaveState OldState = WaveState;

		WaveState = NewState;
		OnRep_WaveState(OldState);

	}
}

void ASGameState::OnRep_WaveState(EWaveState oldState)
{
	WaveStateChanged( oldState, WaveState );
	if (WaveState == EWaveState::GameOver && !bIsGameOver)
	{
		bIsGameOver = true;
		if (GameOverScreen)
		{
			APlayerController* MyController = GetWorld()->GetFirstPlayerController();
			if (MyController)
			{
				UE_LOG(LogTemp, Warning, TEXT("Found Player Controller for Game over."));
				UUserWidget* GameOver = CreateWidget<UUserWidget>(MyController, GameOverScreen);
				GameOver->AddToViewport();

				FTimerHandle TimerHandleGameover;
				GetWorldTimerManager().SetTimer(TimerHandleGameover, this, &ASGameState::GameOver, 4.0);
			}
		}
	}
}

void ASGameState::Sort()
{
	int32 n = PlayerStates.Num();
	QuickSort(PlayerStates, 0, n - 1);
}

int32 ASGameState::Partition(TArray<ASPlayerState*> arr, int32 low, int32 high)
{

	int32 pivot = high;
	float score = arr[high]->Score;

	int32 i = low - 1;

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than the pivot  
		if (arr[j]->Score > score)
		{
			i++; // increment index of smaller element  
			ASPlayerState* temp = arr[i];
			arr[i] = arr[high];
			arr[high] = temp;
		}
	}

	ASPlayerState* temp = arr[i + 1];
	arr[i + 1] = arr[high];
	arr[high] = temp;
	return (i + 1);
}

void ASGameState::QuickSort(TArray<ASPlayerState*> arr, int32 low, int32 high)
{
	if (low < high)
	{
		int32 pi = Partition(arr, low, high);

		// Separately sort elements before  
	 // partition and after partition  
		QuickSort(arr, low, pi - 1);
		QuickSort(arr, pi + 1, high);

	}
}

void ASGameState::SetPlayerStates()
{
	for (int32 i = 0; i < PlayerArray.Num(); i++)
	{
		PlayerStates.AddUnique(Cast<ASPlayerState>(PlayerArray[i]));
	}
}

void ASGameState::SetCurrentWave(int32 value)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		currentWave = value;
	}
}

void ASGameState::SetEnemysLeft(int32 value)
{
	EnemyLeft += value;
}

void ASGameState::GameOver()
{
	UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
	if (GI)
	{
		UMainMenuWidget* Menu = GI->Menuclass;
		if (Menu)
		{
			AMenuController* MyController = Cast<AMenuController>(UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0]));
			if (GI->GetOnlineState())
			{
					GI->SetActiveWidget(EMenuState::MainMenu, 1);
					UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
					GI->LeavingSession = true;
					GI->DestroyExistingSession();				
			}
			else
			{
				GI->SetActiveWidget(EMenuState::MainMenu, 1);
					UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
			}
		}
	}
}

void ASGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
	DOREPLIFETIME(ASGameState, currentWave);
	DOREPLIFETIME(ASGameState, EnemyLeft);
	DOREPLIFETIME(ASGameState, PlayerStates);
	
}

