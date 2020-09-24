// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameModeBase.h"
#include "Components/HordeGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "States/LobbyPlayerState.h"
#include "Controller/LobbyPlayerController.h"
#include "LobbyGameState.h"


ALobbyGameModeBase::ALobbyGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	bDoOnce = false;
}

void ALobbyGameModeBase::CheckPlayersReadyState()
{
	UE_LOG(LogTemp, Warning, TEXT("Checking Ready States."));
	uint32 NumReadyPlayers = 0;
	/**Iterates though Player Controller and gets the LobbyPlayerState then check if the
	ReadyStates is true and if it's true NumReadyPlayer plus 1.*/
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ALobbyPlayerState* ActivePlayer = Cast<ALobbyPlayerController>(It->Get())->GetPlayerState<ALobbyPlayerState>();
			
			if(ActivePlayer)
			{
				if (ActivePlayer->GetReadyState() == true)
				{
					++NumReadyPlayers;
				}
			}
	}
	UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
	if (GI)
	{
		if (int(NumPlayers) >= 2 && int(NumReadyPlayers) >= int(NumPlayers))
		{
			if (!bDoOnce)
			{
				bDoOnce = true;
				UE_LOG(LogTemp, Warning, TEXT("Starting Timer."));
				GetWorldTimerManager().SetTimer(StartGame_TimerHandle, this, &ALobbyGameModeBase::StartGame, 5.0);
			}
		}
		else
		{
			bDoOnce = false;
			GetWorldTimerManager().ClearTimer(StartGame_TimerHandle);
		}
	}
}

void ALobbyGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumPlayers;
}

void ALobbyGameModeBase::Logout(AController * Exiting)
{
	Super::Logout(Exiting);
	--NumPlayers;	
}

void ALobbyGameModeBase::StartGame()
{
	UHordeGameInstance* GI = Cast<UHordeGameInstance>(GetGameInstance());
	if(GI == nullptr)return;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ALobbyPlayerController* ActivePlayer = Cast<ALobbyPlayerController>(It->Get());

		if (ActivePlayer)
		{
			ActivePlayer->ClientTearDownReadyWidget();
		}	
	}
	GI->StartSession();

	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("/Game/Maps/Playground?listen");
}

void ALobbyGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckPlayersReadyState();
}