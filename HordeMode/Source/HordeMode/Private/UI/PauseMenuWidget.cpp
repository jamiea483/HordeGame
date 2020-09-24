// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenuWidget.h"
#include "Controller/SPlayerController.h"
#include "Controller/LobbyPlayerController.h"
#include "Components/HordeGameInstance.h"
#include "UI/MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"


void UPauseMenuWidget::Controls()
{
	ASPlayerController* MyController = Cast<ASPlayerController>(GetOwningPlayer());
	if (MyController)
	{
		if (GetOwningPlayer()->WasInputKeyJustPressed(FKey::FKey("Gamepad_LeftStick_Up")) || MyController->WasInputKeyJustPressed(FKey::FKey("Gamepad_DPad_Up")) || MyController->WasInputKeyJustPressed(FKey::FKey("W")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Up was pressed."));
			if (GetPauseIndex() > 0)
			{
				SetPauseIndex(-1);
			}	
		}
		else if (GetOwningPlayer()->WasInputKeyJustPressed(FKey::FKey("Gamepad_LeftStick_Down")) || MyController->WasInputKeyJustPressed(FKey::FKey("Gamepad_DPad_Down")) || MyController->WasInputKeyJustPressed(FKey::FKey("S")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Down was pressed."));
			if (GetPauseIndex() < PauseMenuButtons.Num() - 1)
			{
				SetPauseIndex(1);
			}
		}
		else if (GetOwningPlayer()->WasInputKeyJustPressed(FKey::FKey("Gamepad_FaceButton_Bottom")) || MyController->WasInputKeyJustPressed(FKey::FKey("Enter")))
		{
			UE_LOG(LogTemp, Warning, TEXT("confirm was pressed."));
			if (GetPauseIndex() == 0)
			{
				MyController->UnPauseGame();
			}
			else if (GetPauseIndex() == PauseMenuButtons.Num() - 1)
			{
				UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
				if (GI)
				{
					if (GI->Menuclass)
					{						
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
								FInputModeGameOnly InputMode;
								MyController->SetInputMode(InputMode);
						}
					}
				}
			}
		}
	}
	else
	{
		ALobbyPlayerController* MyCon = Cast<ALobbyPlayerController>(GetOwningPlayer());
		if(MyCon)
		if (GetOwningPlayer()->WasInputKeyJustPressed(FKey::FKey("Gamepad_LeftStick_Up")) || MyCon->WasInputKeyJustPressed(FKey::FKey("Gamepad_DPad_Up")) || MyCon->WasInputKeyJustPressed(FKey::FKey("W")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Up was pressed."));
			if (GetPauseIndex() > 0)
			{
				SetPauseIndex(-1);
			}
		}
		else if (GetOwningPlayer()->WasInputKeyJustPressed(FKey::FKey("Gamepad_LeftStick_Down")) || MyCon->WasInputKeyJustPressed(FKey::FKey("Gamepad_DPad_Down")) || MyCon->WasInputKeyJustPressed(FKey::FKey("S")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Down was pressed."));
			if (GetPauseIndex() < PauseMenuButtons.Num() - 1)
			{
				SetPauseIndex(1);
			}
		}
		else if (GetOwningPlayer()->WasInputKeyJustPressed(FKey::FKey("Gamepad_FaceButton_Bottom")) || MyCon->WasInputKeyJustPressed(FKey::FKey("Enter")))
		{
			UE_LOG(LogTemp, Warning, TEXT("confirm was pressed."));
			if (GetPauseIndex() == 0)
			{
				MyCon->UnPauseGame();
			}
			else if (GetPauseIndex() == PauseMenuButtons.Num() - 1)
			{
				UHordeGameInstance* GI = GetGameInstance<UHordeGameInstance>();
				if (GI)
				{
					if (GI->Menuclass)
					{
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
							FInputModeGameOnly InputMode;
							MyCon->SetInputMode(InputMode);
						}
					}
				}
			}
		}
	}
}