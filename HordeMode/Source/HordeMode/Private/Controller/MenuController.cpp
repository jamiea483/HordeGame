// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuController.h"
#include "HordeGameInstance.h"
#include "MainMenuWidget.h"
#include "Components/EditableTextBox.h"
#include "HordeGameUserSettings.h"
#include "GameplayWidget.h"
#include "AudioWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/WidgetSwitcher.h"

void AMenuController::BeginPlay()
{
	Super::BeginPlay();

	 GI = GetGameInstance<UHordeGameInstance>();
	if (GI)
	{
		MenuWidget = GI->Menuclass;
	}
}

void AMenuController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MenuUpDown", this, &AMenuController::MenuYAxis);
	InputComponent->BindAxis("MenuLeftRight", this, &AMenuController::MenuXAxis);

	InputComponent->BindAction("StartMenuButton", IE_Pressed, this, &AMenuController::Start);
	InputComponent->BindAction("Confirm", IE_Pressed, this, &AMenuController::Confirm);
	InputComponent->BindAction("Back", IE_Pressed, this, &AMenuController::Back);
}

void AMenuController::MenuYAxis(float value)
{	
	if (GI)
	{
		if (GI->Player1.Num() <= 0)return;
		if (UGameplayStatics::GetPlayerControllerFromID(GetWorld(),GI->Player1[0]) == this)
		{		
			if (MenuWidget)
			{		
				
				if (value > 0.5 && !bDoOnce)
				{
				
					bDoOnce = true;
					if (MenuWidget->GetMainMenuIndex(MenuWidget->GetMenuState()) > 0)
					{
						MenuWidget->SetMainMenuIndex(MenuWidget->GetMenuState(), -1);
						UE_LOG(LogTemp, Warning, TEXT("Index = %i"), MenuWidget->GetMainMenuIndex(MenuWidget->GetMenuState()));
					}
				}
				else if (value < -0.5 && !bDoOnce)
				{
					bDoOnce = true;
					if (MenuWidget->GetMainMenuIndex(MenuWidget->GetMenuState()) < MenuWidget->GetIndexCap(MenuWidget->GetMenuState()))
					{
						MenuWidget->SetMainMenuIndex(MenuWidget->GetMenuState(), 1);
						UE_LOG(LogTemp, Warning, TEXT("Index = %i"), MenuWidget->GetMainMenuIndex(MenuWidget->GetMenuState()));
					}
				}
				if(value < 0.5 && value > -0.5)
				{
					bDoOnce = false;
				}
			}
		}
	}
}

void AMenuController::MenuXAxis(float value)
{	
	if (GI)
	{
		if (GI->Player1.Num() <= 0)return;
		if (UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0]) == this)
		{
			if (MenuWidget)
			{
				if (value > 0.5 && !bDoOnceX)
				{
					bDoOnceX = true;
					if (MenuWidget->GetMainMenuXIndex(MenuWidget->GetMenuState()) < MenuWidget->GetIndexCapXAxis(MenuWidget->GetMenuState()))
					{
						MenuWidget->SetMainMenuXIndex(MenuWidget->GetMenuState(), 1);
					}
				}
				else if (value < -0.5 && !bDoOnceX)
				{
					bDoOnceX = true;
					if (MenuWidget->GetMainMenuXIndex(MenuWidget->GetMenuState()) > 0)
					{
						MenuWidget->SetMainMenuXIndex(MenuWidget->GetMenuState(), -1);
					}
				}
				if (value <= 0.5 && value >= -0.5)
				{
					bDoOnceX = false;
				}
			}
		}
	}
}

/*Sets the Controlling player at the start screen*/
void AMenuController::Start()
{
	if (GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("GI Found."));
		if (MenuWidget->GetMenuState() == EMenuState::StartMenu)
		{
			UE_LOG(LogTemp, Warning, TEXT("Menu Found."));
			if (GI->Player1.Num() <= 0 && MenuWidget)
			{
			
				if ( !MenuWidget->GetIntroIsPlaying())
				{
					GI->Player1.Add(GetInputIndex());
					UE_LOG(LogTemp, Warning, TEXT("Player index is %i"), GI->Player1[0]);
					GI->SetActiveWidget(EMenuState::MainMenu, 1);
				}
			}
			else
			{
				GI->SetActiveWidget(EMenuState::MainMenu, 1);
			}
		}		
	}
}

void AMenuController::Confirm()
{
	if (GI)
	{
		if (GI->Player1.Num() <= 0)return;
		if (UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0]) == this)
		{
			if (MenuWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("Confirm was pressed."));
			 if (MenuWidget->GetMenuState() == EMenuState::MainMenu)
			 {
					if (MenuWidget->GetMainMenuIndex(EMenuState::MainMenu) == 0)
					{
						UGameplayStatics::OpenLevel(GetWorld(), "Playground");

						GI->SetOnlineState(false);
					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::MainMenu) == 1)
					{
						GI->SetActiveWidget(EMenuState::Multiplayer, 4);
					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::MainMenu) == 2)
					{
						GI->SetActiveWidget(EMenuState::Options, 2);
					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::MainMenu) == 3)
					{
						UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
					}
				}
				else if (MenuWidget->GetMenuState() == EMenuState::Options)
				{
					if (MenuWidget->GetMainMenuIndex(EMenuState::Options) == 0)
					{

					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::Options) == 1)
					{
						GI->SetActiveWidget(EMenuState::Audio, 3);
						MenuWidget->SetMainMenuIndex(EMenuState::Audio,-MenuWidget->GetMainMenuIndex(EMenuState::Audio));
						MenuWidget->WBP_Audio->SetAudioForWidget();
					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::Options) == 2)
					{
						GI->SetActiveWidget(EMenuState::Gameplay, 7);
						MenuWidget->SetMainMenuIndex(EMenuState::Gameplay, -MenuWidget->GetMainMenuIndex(EMenuState::Gameplay));
						MenuWidget->WBP_Gameplay->SetGameplayWidget();
					}

				}
				else if (MenuWidget->GetMenuState() == EMenuState::Multiplayer)
				{
					if (MenuWidget->GetMainMenuIndex(EMenuState::Multiplayer) == 0)
					{
						GI->SetActiveWidget(EMenuState::Lobbys, 6);
						GI->FindGameSessions();
					}
					else
					{
						GI->SetActiveWidget(EMenuState::MultiplayerSettings, 5);
					}
				}
				else if (MenuWidget->GetMenuState() == EMenuState::Audio)
				{
					if (MenuWidget->GetMainMenuIndex(EMenuState::Audio) == 3)
					{
						GI->SetActiveWidget(EMenuState::Options, 2);
						UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
						if (userSettings)
						{
							MenuWidget->WBP_Audio->SaveAudioSettings();
						}
					}
				}
				else if (MenuWidget->GetMenuState() == EMenuState::MultiplayerSettings)
				{
					if (MenuWidget->GetMainMenuIndex(EMenuState::MultiplayerSettings) == MenuWidget->MultplayerSettingTextArray.Num())
					{
						FString ServerName = MenuWidget->ServerNameText->GetText().ToString();					
						FString Password = MenuWidget->PasswordText->GetText().ToString();
						FInputModeGameOnly Inputmode;
						UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0])->SetInputMode(Inputmode);
						GI->Host(ServerName, Password, MenuWidget->GetMaxNumberPlayersIndex(), MenuWidget->GetPrivateSlotsIndex());
						GI->SetOnlineState(true);
						
					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::MultiplayerSettings) == 0)
					{
						FInputModeGameAndUI Inputmode;
						Inputmode.SetWidgetToFocus(MenuWidget->ServerNameText->TakeWidget());
						UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0])->SetInputMode(Inputmode);
					}
					else if (MenuWidget->GetMainMenuIndex(EMenuState::MultiplayerSettings) == 3)
					{
						FInputModeGameAndUI Inputmode;
						Inputmode.SetWidgetToFocus(MenuWidget->PasswordText->TakeWidget());
						UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0])->SetInputMode(Inputmode);
					}
				}
				else if (MenuWidget->GetMenuState() == EMenuState::Lobbys)
				{
					if (MenuWidget->ServerList->GetAllChildren().Num() >= 1)
					{
						GI->Join(MenuWidget->GetMainMenuIndex(EMenuState::Lobbys));
					}
				}
				else if (MenuWidget->GetMenuState() == EMenuState::Gameplay)
				{
					if (MenuWidget->GetMainMenuIndex(EMenuState::Gameplay) == 2)
					{
						GI->SetActiveWidget(EMenuState::Options, 2);
						UHordeGameUserSettings* userSettings = Cast<UHordeGameUserSettings>(UHordeGameUserSettings::GetGameUserSettings());
						if (userSettings)
						{
							MenuWidget->WBP_Gameplay->SaveGameplaySettings();
						}
					}
				}
			}
		}
	}
}

void AMenuController::Back()
{
	if (GI)
	{
		if (GI->Player1.Num() <= 0)return;
		if (UGameplayStatics::GetPlayerControllerFromID(GetWorld(), GI->Player1[0]) == this)
		{
			if (MenuWidget)
			{
				if (MenuWidget->GetMenuState() == EMenuState::MainMenu)
				{
					GI->SetActiveWidget(EMenuState::StartMenu, 0);
					GI->Player1.Empty();
				}
				else if (MenuWidget->GetMenuState() == EMenuState::Options || MenuWidget->GetMenuState() == EMenuState::Multiplayer)
				{
					GI->SetActiveWidget(EMenuState::MainMenu, 1);
				}
				else if (MenuWidget->GetMenuState() == EMenuState::Audio|| MenuWidget->GetMenuState() == EMenuState::Gameplay || MenuWidget->GetMenuState() == EMenuState::Graphics)
				{
					GI->SetActiveWidget(EMenuState::Options, 2);
				}
				else if (MenuWidget->GetMenuState() == EMenuState::MultiplayerSettings || MenuWidget->GetMenuState() == EMenuState::Lobbys)
				{
					GI->SetActiveWidget(EMenuState::Multiplayer, 4);
				}

			}
		}
	}
}
