// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameModeBase.h"
#include  "HordeGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HordeGameInstance.h"

ALobbyGameModeBase::ALobbyGameModeBase()
{
}

void ALobbyGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++NumPlayers;
	UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
	if (GI)
	{
		int SetPlayers = GI->GetServerSettings().PublicSlots + GI->GetServerSettings().PrivateSlots;
		if (int(NumPlayers) >= 2)
		{
			GetWorldTimerManager().SetTimer(StartGame_TimerHandle, this, &ALobbyGameModeBase::StartGame, 10.0);
		}
	}
}

void ALobbyGameModeBase::Logout(AController * Exiting)
{
	Super::Logout(Exiting);

	--NumPlayers;
	if (NumPlayers < 2)
	{
		GetWorldTimerManager().ClearTimer(StartGame_TimerHandle);
	}
}

void ALobbyGameModeBase::StartGame()
{
	UHordeGameInstance* GI = Cast<UHordeGameInstance>(GetGameInstance());
	if(GI == nullptr)return;
	GI->StartSession();
	
	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("/Game/Maps/Playground?listen");
}