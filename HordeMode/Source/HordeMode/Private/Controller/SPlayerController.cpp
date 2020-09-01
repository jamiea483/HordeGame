// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SPlayerController.h"
#include "UI/PauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HordeGameInstance.h"
#include "Player/PlayerSCharacter.h"

void ASPlayerController::LoadPauseMenu()
{
	if (PauseMenuWidget)
	{
		PauseMenu = CreateWidget<UPauseMenuWidget>(this, PauseMenuWidget);
		FInputModeGameAndUI Inputmode;
		
		bShowMouseCursor = false;
		SetInputMode(Inputmode);
		PauseMenu->AddToViewport();
	}
}

void ASPlayerController::TearDown()
{
	if (PauseMenu)
	{
		PauseMenu->RemoveFromParent();
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Start", IE_Pressed, this, &ASPlayerController::PauseGame);
}

void ASPlayerController::PauseGame()
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

void ASPlayerController::UnPauseGame()
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

