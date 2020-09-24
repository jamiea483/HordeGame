// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/LobbyPlayerController.h"
#include "States/SPlayerState.h"

#include "UI/PauseMenuWidget.h"
#include "UI/ReadyUpWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HordeGameInstance.h"
#include "Player/PlayerSCharacter.h"
#include "States/LobbyPlayerState.h"

ALobbyPlayerController::ALobbyPlayerController(const FObjectInitializer &ObjectInitializer)
{

}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsLocalController())
		CreateReadyUpUI();

	if (ReadyUpWidget)
		ReadyUpWidget->UpdateWidget();
}

void ALobbyPlayerController::LoadPauseMenu()
{
	if (PauseMenu)
	{
		FInputModeGameAndUI InputMode;
		bShowMouseCursor = false;
		SetInputMode(InputMode);
		PauseMenu->AddToViewport();
	}
	else
	{
		if (PauseMenuWidget)
		{
			PauseMenu = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidget);
			FInputModeGameAndUI InputMode;

			bShowMouseCursor = false;
			SetInputMode(InputMode);
			PauseMenu->AddToViewport();
		}
	}
}

void  ALobbyPlayerController::TearDown()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromParent();
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void ALobbyPlayerController::CreateReadyUpUI()
{
	if (ReadyUpWidget)
	{
		FInputModeGameOnly InputMode;
		bShowMouseCursor = false;
		SetInputMode(InputMode);
		ReadyUpWidget->ToViewport();
		UE_LOG(LogTemp, Warning, TEXT("Ready widget Exist so adding to view port."))

	}
	else
	{
		if (ReadyUpWidgetClass)
		{
			ReadyUpWidget = CreateWidget<UReadyUpWidget>(this, ReadyUpWidgetClass);
			FInputModeGameOnly InputMode;

			bShowMouseCursor = false;
			SetInputMode(InputMode);
			if(ReadyUpWidget)
			ReadyUpWidget->ToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Created Ready widget and Adding to viewport."))
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ERROR: No ReadyUpWidget to create."))
		}
	}
}

void  ALobbyPlayerController::TearDownReadyWidget()
{
	if (ReadyUpWidget)
	{
		ReadyUpWidget->TearDownReadyWidget();
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void  ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Start", IE_Pressed, this, &ALobbyPlayerController::PauseGame);
	InputComponent->BindAction("Back", IE_Pressed, this, &ALobbyPlayerController::ChangeReadyState);
}

void  ALobbyPlayerController::PauseGame()
{
	APlayerSCharacter* MyOwner = Cast<APlayerSCharacter>(GetPawn());
	if (MyOwner)
	{
		if (!MyOwner->GetPauseState())
		{
			UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
			if (GI)
			{
				if (GI->GetOnlineState())
				{
					MyOwner->SetPauseState(true);
					LoadPauseMenu();

				}
				else
				{
					MyOwner->SetPauseState(true);
					LoadPauseMenu();
					UGameplayStatics::SetGamePaused(GetWorld(), true);
				}
			}
		}
	}
}

void ALobbyPlayerController::UnPauseGame()
{
	UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
	if (GI)
	{
		APlayerSCharacter* MyOwner = Cast<APlayerSCharacter>(GetPawn());
		if (MyOwner)
		{
			if (GI->GetOnlineState())
			{
				MyOwner->SetPauseState(false);
				TearDown();
			}
			else
			{
				MyOwner->SetPauseState(false);
				TearDown();
				UGameplayStatics::SetGamePaused(GetWorld(), false);
			}
		}
	}
}

void ALobbyPlayerController::ChangeReadyState()
{
	if (GetLocalRole() < ROLE_Authority)
		ServerChangeReadyState();

	UE_LOG(LogTemp, Warning, TEXT("player controller."))
	ALobbyPlayerState* LobbyState = GetPlayerState<ALobbyPlayerState>();
	if (LobbyState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ready State change in player controller."))
		LobbyState->ChangePlayerReadyState();
	}
}

void ALobbyPlayerController::ServerChangeReadyState_Implementation()
{
	ChangeReadyState();
}

void ALobbyPlayerController::ClientTearDownReadyWidget_Implementation()
{
	TearDownReadyWidget();
}