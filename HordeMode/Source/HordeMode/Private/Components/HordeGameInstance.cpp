// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HordeGameInstance.h"
#include "Engine/World.h"
#include "Controller/MenuController.h"
#include "UI/MainMenuWidget.h"
#include "UI/EnterPasswordWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"

static const FName SESSION_NAME = TEXT("GameSession");
static const FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");
static const FName PASSWORD_SETTINGS_KEY = TEXT("Password");

UHordeGameInstance::UHordeGameInstance()
{
}

void UHordeGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	if (SubSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s."), *SubSystem->GetSubsystemName().ToString());
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			//Binds Session Creation Delegates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UHordeGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UHordeGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UHordeGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UHordeGameInstance::OnSessionJoinComplete);
			UE_LOG(LogTemp, Warning, TEXT("Found Online Session Interface."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No subsystem found."))
	}
}

void UHordeGameInstance::Host(const FString ServerName, const FString password, int32 MaxPlayers, int32 PrivateSlots)
{
	if (SessionInterface.IsValid())
	{
		if (ServerName == "")
		{
			ServerSettings.ServerName = TEXT("Test");
		}
		else
		{
			ServerSettings.ServerName = ServerName;
		}
		ServerSettings.SetPassword = password;
		ServerSettings.PublicSlots = MaxPlayers;
		ServerSettings.PrivateSlots = PrivateSlots;
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (!ExistingSession)
		{
			CreateSession();
		}
		else
		{
			LeavingSession = false;
			DestroyExistingSession();
		}
	}
}

void UHordeGameInstance::DestroyExistingSession()
{
	SessionInterface->DestroySession(SESSION_NAME);
}

void UHordeGameInstance::CreateSession()
{
	FOnlineSessionSettings SessionSettings;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		SessionSettings.bIsLANMatch = true;
	}
	else
	{
		SessionSettings.bIsLANMatch = false;
	}

	SessionSettings.NumPublicConnections = ServerSettings.PublicSlots;
	SessionSettings.NumPrivateConnections = ServerSettings.PrivateSlots;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	UE_LOG(LogTemp, Warning, TEXT("Server Name is %s."), *ServerSettings.ServerName);
	SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, ServerSettings.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	if (ServerSettings.SetPassword != "")
	{
		SessionSettings.Set(PASSWORD_SETTINGS_KEY, ServerSettings.SetPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		UE_LOG(LogTemp, Warning, TEXT("Password Was Set."));
	}

	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UHordeGameInstance::FindGameSessions()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		if (Menuclass)
		{
			Menuclass->RefreshList();
		}
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		UE_LOG(LogTemp, Warning, TEXT("Looking for Sessions."))
	}
}



void UHordeGameInstance::Join(int32 Index)
{
	if (!SessionInterface.IsValid())return;
	if (UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Player1[0]))
	{
		FString SetPassword;
		SessionSearch->SearchResults[Index].Session.SessionSettings.Get(PASSWORD_SETTINGS_KEY, SetPassword);
		if (SetPassword != "")
		{
			if (Menuclass && Menuclass->PasswordWidget)
			{
				UEnterPasswordWidget* Password = CreateWidget<UEnterPasswordWidget>(this, Menuclass->PasswordWidget);

				FInputModeGameAndUI Inputmode;
				Inputmode.SetWidgetToFocus(Password->EnteredPassword->TakeWidget());
				UGameplayStatics::GetPlayerControllerFromID(GetWorld(),Player1[0])->SetInputMode(Inputmode);
				Password->AddToViewport();
				Password->Pass = SetPassword;
				Password->index = Index;
			}
		}
		else
		{
			JoinFunction(Index);
		}
	}
}

void UHordeGameInstance::JoinFunction(const int32 &Index)
{
	FInputModeGameOnly Inputmode;
	UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Player1[0])->SetInputMode(Inputmode);
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UHordeGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

void UHordeGameInstance::OnCreateSessionComplete(FName Session, bool Success)
{
	if (!Success)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Maps/Lobby?listen");
	}
}

void UHordeGameInstance::OnDestroySessionComplete(FName Session, bool Success)
{
	if (Success)
	{
		if (LeavingSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("Destroyed session."));

		}
		else
		{
			CreateSession();
		}
	}
}

void UHordeGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && Menuclass && SessionSearch.IsValid())
	{
		if (SessionSearch->SearchResults.Num() > 0)
		{
			TArray<FServerStruct> ServerData;
			for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
			{
				UE_LOG(LogTemp, Warning, TEXT("found Sessions called %s."), *SearchResult.GetSessionIdStr());
				FServerStruct Data;
				
				Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
				Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
				Data.UserName = SearchResult.Session.OwningUserName;
				FString NewServerName;
				if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, NewServerName))
				{
					Data.Name = NewServerName;
				}
				else
				{
					Data.Name = "Could not find name.";
				}

				ServerData.Add(Data);
			}
			Menuclass->SetServerList(ServerData);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("no Server found"));
		}
	
	}
}

void UHordeGameInstance::OnSessionJoinComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))return;
	{
		UGameplayStatics::GetPlayerControllerFromID(GetWorld(), Player1[0])->ClientTravel(Address, ETravelType::TRAVEL_Relative);
	}
}

void UHordeGameInstance::SetActiveWidget(EMenuState Screen, int32 WidgetNumber)
{
	Menuclass->SetMenuState(Screen);
	Menuclass->MenuWidget->SetActiveWidgetIndex(WidgetNumber);
}