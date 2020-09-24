// Fill out your copyright notice in the Description page of Project Settings.


#include "States/LobbyPlayerState.h"
#include "GameMode/LobbyGameModeBase.h"
#include "Controller/LobbyPlayerController.h"
#include "UI/Interface/ReadyUpInterface.h"
#include "Net/UnrealNetwork.h"


void ALobbyPlayerState::ChangePlayerReadyState()
{
	UE_LOG(LogTemp, Warning, TEXT("Ready State change in lobby player state."))
	if (bIsReady)
		bIsReady = false;
	else
		bIsReady = true;

	UpdatePlayerWidget();
}

void ALobbyPlayerState::UpdatePlayerWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("Server Called UpdateReadyUI."));
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		if (PC->ReadyUpWidget)
			PC->ReadyUpWidget->UpdateWidget();
	}
}

void ALobbyPlayerState::OnRep_ReadyState()
{
	UpdatePlayerWidget();
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, bIsReady);
}



