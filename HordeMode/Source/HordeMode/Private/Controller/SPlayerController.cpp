// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "PauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "HordeGameInstance.h"
#include "SCharacter.h"

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
	ASCharacter* MyOwner = Cast<ASCharacter>(GetPawn());
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
		ASCharacter* MyOwner = Cast<ASCharacter>(GetPawn());
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

