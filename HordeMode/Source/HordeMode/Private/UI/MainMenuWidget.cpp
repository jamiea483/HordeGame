// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextBlock.h"
#include "UI/ServerWidget.h"
#include "UI/GameplayWidget.h"
#include "UI/AudioWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/PanelWidget.h"


UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/Blueprint/UI/WBP_ServerRow"));

		ServerRow = ServerRowBPClass.Class;

		MaxPlayerIndex = 2;
}

int32 UMainMenuWidget::GetMainMenuIndex(EMenuState State)
{
	switch (State)
	{

	case EMenuState::MainMenu:return MenuIndex;
		break;
	case EMenuState::Options:return OptionIndex;
		break;
	case EMenuState::Audio:return AudioIndex;
		break;
	case EMenuState::Gameplay:return GameplayIndex;
		break;
	case EMenuState::Graphics:return GraphicIndex;
		break;
	case EMenuState::Multiplayer:return MultplayerIndex;
		break;
	case EMenuState::MultiplayerSettings:return MultplayerSettingsIndex;
		break;
	case EMenuState::Lobbys:return LobbyIndex;
		break;
	default: return 0;
		break;
	}
}

void UMainMenuWidget::SetMainMenuIndex(EMenuState State, float Value)
{
	switch (State)
	{
	case EMenuState::StartMenu:
		break;
	case EMenuState::MainMenu:MenuIndex += Value;
		break;
	case EMenuState::Options:OptionIndex += Value;
		break;
	case EMenuState::Audio:AudioIndex += Value;
		break;
	case EMenuState::Gameplay:	GameplayIndex += Value;
		break;
	case EMenuState::Graphics:GraphicIndex += Value;
		break;
	case EMenuState::Multiplayer:MultplayerIndex += Value;
		break;
	case EMenuState::MultiplayerSettings:MultplayerSettingsIndex += Value;
		break;
	case EMenuState::Lobbys:LobbyIndex += Value;
		break;
	default:
		break;
	}
}

float UMainMenuWidget::GetMainMenuXIndex(EMenuState State)
{
	float value = 0;
	switch (State)
	{
	case EMenuState::Audio:
		if (GetMainMenuIndex(State) == 0)
			value = WBP_Audio->GetMVSValue();
		else if (GetMainMenuIndex(State) == 1)
			value = WBP_Audio->GetMSSVValue();
		else if (GetMainMenuIndex(State) == 2)
			value = WBP_Audio->GetSFXVSValue();
		return value;
		break;
	case EMenuState::Gameplay:
		if (GetMainMenuIndex(State) == 0)
			value = WBP_Gameplay->GetVertical();
		else if (GetMainMenuIndex(State) == 1)
			value = WBP_Gameplay->GetHorizaontal();
		return value;
		break;
	case EMenuState::MultiplayerSettings:
		if (GetMainMenuIndex(State) == 1)
			value = GetMaxNumberPlayersIndex();
		else if (GetMainMenuIndex(State) == 2)
			value = GetPrivateSlotsIndex();
		return value;
		break;
	default: return 0;
		break;
	}
}

void UMainMenuWidget::SetMainMenuXIndex(EMenuState State, float value)
{
	switch (State)
	{
	case EMenuState::Audio:
		if (GetMainMenuIndex(State) == 0)
			WBP_Audio->SetMVSValue(value*5);
		else if (GetMainMenuIndex(State) == 1)
			WBP_Audio->SetMSSVValue(value*5);
		else if (GetMainMenuIndex(State) == 2)
			WBP_Audio->SetSFXVSValue(value*5);
		break;
	case EMenuState::Gameplay:
		if (GetMainMenuIndex(State) == 0)
			WBP_Gameplay->SetVertical(value*0.2);
		else if (GetMainMenuIndex(State) == 1)
			WBP_Gameplay->SetHorizaontal(value*0.2);
		break;
	case EMenuState::MultiplayerSettings:
		if (GetMainMenuIndex(State) == 1)
			SetMaxNumberPlayersIndex(value);
		else if (GetMainMenuIndex(State) == 2)
			SetPrivateSlotsIndex(value);
		break;
	default:
		break;
	}

}


void UMainMenuWidget::SetMaxNumberPlayersIndex(int32 value)
{
	MaxPlayerIndex += value;
}

int32 UMainMenuWidget::GetMaxNumberPlayersIndex()
{
	return MaxPlayerIndex;
}

void UMainMenuWidget::SetPrivateSlotsIndex(int32 value)
{
	privateSlotIndex += value;
}

int32 UMainMenuWidget::GetPrivateSlotsIndex()
{
	return privateSlotIndex;
}

int32 UMainMenuWidget::GetIndexCap(EMenuState State)
{
	switch (State)
	{
	case EMenuState::StartMenu:return 0;
		break;
	case EMenuState::MainMenu:return MainMenuButtonArray.Num()-1;
		break;
	case EMenuState::Options:return OptionButtonArray.Num()-1;
		break;
	case EMenuState::Audio:return AudioTextArray.Num();
		break;
	case EMenuState::Gameplay:return GameplayTextArray.Num();
		break;
	case EMenuState::Graphics:return MainMenuButtonArray.Num()-1;
		break;
	case EMenuState::Multiplayer:return MultiplayerButtonArray.Num()-1;
		break;
	case EMenuState::MultiplayerSettings:return MultplayerSettingTextArray.Num();
		break;
	case EMenuState::Lobbys:return ServerList->GetAllChildren().Num()-1;
		break;
	default:return 0;
		break;
	}
}

int32 UMainMenuWidget::GetIndexCapXAxis(EMenuState State)
{
	switch (State)
	{

	case EMenuState::Audio: return 100;
		break;
	case EMenuState::Gameplay: return 1;
		break;
	case EMenuState::MultiplayerSettings: return 4;
		break;
	default: return 0;
		break;
	}
}

void UMainMenuWidget::SetServerList(TArray<FServerStruct> Servers)
{
	UWorld* world = this->GetWorld();
	if (!world)return;
	counter = 0;
	
	for (const FServerStruct& ServerData : Servers)
	{	
		UServerWidget* Server = CreateWidget<UServerWidget>(world, ServerRow);
		if (!Server)return;

		Server->ServerNameText->SetText(FText::FromString(ServerData.Name));
		FString PlayerCount = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		Server->PlayerNumText->SetText(FText::FromString(PlayerCount));
		Server->HostNameText->SetText(FText::FromString(ServerData.UserName));

		ServerList->AddChild(Server);
		counter++;
	}
}

void UMainMenuWidget::RefreshList()
{
	ServerList->ClearChildren();
}

void UMainMenuWidget::EMenuStateChanged(EMenuState State)
{
	EMenu = State;
}
